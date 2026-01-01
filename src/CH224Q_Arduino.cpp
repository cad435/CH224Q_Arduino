#include "CH224Q_Arduino.h"
//#include "CH224Q_Registers.h"
//#include "CH224Q_PDO_Decoder.h"


CH224Q::CH224Q(bool logging, TwoWire* _wire)
{
    wire = _wire;
}


int8_t CH224Q::begin(uint8_t address)
{
    //Serial.println("test");
    addr = address;
    if (!wire) return -1; //return false if no wire instance
    wire->begin(); //initialize I2C bus

    uint8_t value = 0;

    readRegister(CH224Q_STATUS, value);

#ifdef CH224Q_DEBUG
    Serial.print("[CH224Q|Info] CH224Q.begin(): Probed register 0x0");
    Serial.print(CH224Q_STATUS, HEX);
    Serial.print(", found 0x");
    Serial.println(value, HEX);
#endif

    //something must be coming back
    if (value == 0)
        return -1;
    

    // simple probe by zero-length transmission
    wire->beginTransmission(addr); 
    if (wire->endTransmission() != 0)
        return -1;
    
    return 0;

}

int8_t CH224Q::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t err;

    wire->beginTransmission(addr);
    wire->write(reg);
    wire->write(value);
    err = wire->endTransmission(true); // End transmission and release bus

    /*Serial.print("Wrote 0x");
    Serial.print(value, HEX);
    Serial.print(" to register 0x");
    Serial.println(reg, HEX);*/

    return err; //0:success, 1:data too long, 2:NACK on address, 3:NACK on data, 4:other error

}

int8_t CH224Q::readRegister(uint8_t reg, uint8_t &value)
{
    if (!wire) return -1; //check if Wire is initialized

    wire->beginTransmission(addr);
    wire->write(reg);
    wire->endTransmission(false); // Keep connection alive for reading

    uint8_t ret;
    wire->requestFrom(0x22, 1); // Request 1 byte
    if (wire->available()) {
        ret = wire->read();
        /*Serial.print("Register 0x");
        Serial.print(reg, HEX);
        Serial.print(" value: 0x");
        Serial.println(ret, HEX);*/
    }
    else
        return -1; //no read

    value = ret;
    
    uint8_t r = wire->endTransmission(); //end transmission, 

    return r; //0:success, 1:data too long, 2:NACK on address, 3:NACK on data, 4:other error

}

int8_t CH224Q::setMode(uint8_t Mode)
{
    // Write the mode value to the MODE_CTRL register

    int8_t err = writeRegister(CH224Q_VOLTAGEMODE_CTRL, Mode);

    if (err != 0)
    {
        Serial.println("[CH224Q|ERR] CH224Q.setMode() unsuccessfull, I2C Error Code: " + err);
        return err; // Return error code if write failed
    }
        

        
    //datasheet specifies we cant read the CH224Q_VOLTAGEMODE_CTRL register back to confirm mode, so we just look if any Protocoll Handshake has happened 
    //and assume this means the PSU accepted our request
    delay(100); // Small delay to allow mode change to take effect

    uint8_t Mode_Readback = getStatus();

    if (Mode_Readback == 0) //if none of the mode bits are set, handshake failed
    {
        Serial.print("[CH224Q|ERR] CH224Q.setMode() probed register 0x");
        Serial.print(CH224Q_STATUS, HEX);
        Serial.println(": could not get a validate handshake from PSU!");
        CurrentMode = CH224Q_MODE_UNKNOWN; //reset current mode
        return -1; // Handshake failed
    }

    CurrentMode = Mode; // Update current mode

    return 0; // Success

} 

uint8_t CH224Q::getStatus()
{

    uint8_t registerValue = 0;
    readRegister(CH224Q_STATUS, registerValue);

    //check which Bit is set and return corresponding status
    if (registerValue & CH224Q_STATUS_BC_ACTIVATED) {
        return CH224Q_STATUS_BC_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_QC2_ACTIVATED) {
        return CH224Q_STATUS_QC2_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_QC3_ACTIVATED) {
        return CH224Q_STATUS_QC3_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_PD_ACTIVATED) {
        return CH224Q_STATUS_PD_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_EPR_ACTIVATED) {
        return CH224Q_STATUS_EPR_ACTIVATED;
    }
    else {
        return CH224Q_STATUS_NONE_ACTIVATED;
    }
    
    return -1;
}

uint32_t CH224Q::getPDORawValue(uint8_t index)
{

    uint8_t Meta[2] = {0};
    readRegister(CH224Q_SRCCAP_START, Meta[0]);
    readRegister(CH224Q_SRCCAP_START, Meta[1]);

#ifdef CH224Q_DEBUG

    Serial.print("PDO Metadata: 0x");
    Serial.print(Meta[0], HEX);
    Serial.print("|0x");
    Serial.println(Meta[1]);
    
#endif
    // Each PDO is 4 bytes, starting from CH224Q_SRCCAP_START
    uint8_t regAddress = CH224Q_SRCCAP_START + (index * 4);
    uint32_t pdoValue = 0;

    uint8_t bytes[4] = {0};
    uint8_t err = 0;



    // Read 4 bytes of the PDO
    for (uint8_t i = 0; i < 4; i++) {
        err = readRegister(regAddress + i, bytes[i]);

        if (err != 0) {
            // Error reading register, return invalid PDOInfo
            Serial.print("[CH224Q|ERR] CH224Q.decodePDOInfo(): Error reading PDO Index ");
            Serial.println(index);
            return 0;
        }
    }
    //LSB
    pdoValue = ( (uint32_t)bytes[0]) | ( (uint32_t)bytes[1] <<  8 ) | ( (uint32_t)bytes[2] << 16 ) | ( (uint32_t)bytes[3] << 24 );

#ifdef CH224Q_DEBUG
    Serial.print("Raw PDO Bytes: 0x");
    Serial.print(bytes[3], HEX);
    Serial.print("|0x");
    Serial.print(bytes[2], HEX);
    Serial.print("|0x");
    Serial.print(bytes[1], HEX);
    Serial.print("|0x");
    Serial.println(bytes[0], HEX);
#endif

    return pdoValue;
}

int8_t CH224Q::getNumberPDOs()
{
    // Read the source capabilities range
    // check every PDO register until we hit an invalid one
    uint8_t count = 0;  
    for (uint8_t reg = CH224Q_SRCCAP_START; reg <= CH224Q_SRCCAP_END; reg += 4) {
        uint8_t byteValue = 0;
        if (readRegister(reg, byteValue) != 0) {
            break; // Error reading register, stop counting
        }
        // If we read a valid PDO (non-zero), increment count
        if (byteValue != 0) {
            count++;
        } else {
            break; // Stop at first zero PDO
        }
    }

    return count;
}

int8_t CH224Q::requestPPSVoltage_mv(uint16_t voltage_mV)
{
    // Check if voltage is within PPS range (5000 to 28000 mV)
    if (voltage_mV < 5000 || voltage_mV > 28000) {
        return -1; // Invalid voltage
    }

    // Calculate the register values based on voltage
    uint16_t rawValue = voltage_mV / 100; // PPS uses 100mV units


    // Write to PPS voltage control register
    if (writeRegister(CH224Q_PPS_VOLTAGE_CTRL, rawValue) != 0)
        return -1; // Error writing PPS_CTRL


    //if current mode is not PPS mode, switch to PPS mode
    if (CurrentMode != CH224Q_MODE_PPS) {
        uint8_t err = setMode(CH224Q_MODE_PPS);
        if (err != 0)
            return err; // Return error code if mode switch failed
    }

    return 0; // Success
}

int8_t CH224Q::requestAVSVoltage_mv(uint16_t voltage_mV)
{
    // Check if voltage is within AVS range (5000 to 20000 mV)
    if (voltage_mV < 5000 || voltage_mV > 20000) {
        return -1; // Invalid voltage
    }

    // Calculate the register values based on voltage
    uint16_t rawValue = voltage_mV / 100; // AVS uses 100mV units

    uint8_t highByte = (rawValue >> 8) & 0x7F; // Upper 7 bits
    uint8_t lowByte = rawValue & 0xFF;         // Lower 8 bits

    // Set the enable bit in the high byte
    highByte |= 0x80; // Set highest bit to enable AVS

    // Write to AVX control registers
    if (writeRegister(CH224Q_AVX_CTRL1, highByte) != 0)
        return -1; // Error writing AVX_CTRL1
    if (writeRegister(CH224Q_AVX_CTRL2, lowByte) != 0)
        return -1; // Error writing AVX_CTRL2

    //if current mode is not AVS mode, switch to AVS mode
    if (CurrentMode != CH224Q_MODE_AVS) {
        uint8_t err = setMode(CH224Q_MODE_AVS);
        if (err != 0)
            return err; // Return error code if mode switch failed
    }
    return 0; // Success
}

uint16_t CH224Q::getMaxCurrent_mA()
{
    readRegister(CH224Q_CURRENT_CAPABILTY, (uint8_t&)CurrentMaxCurrentLimit_mA);
    return CurrentMaxCurrentLimit_mA;
}