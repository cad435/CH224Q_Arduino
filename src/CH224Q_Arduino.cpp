#include "CH224Q_Arduino.h"
//#include "CH224Q_Registers.h"
//#include "CH224Q_PDO_Decoder.h"




int8_t CH224Q::begin(uint8_t address)
{
    _addr = address;
    if (!_wire) return false; //return false if no wire instance
    _wire->begin(); //initialize I2C bus
    // simple probe by zero-length transmission
    //_wire->beginTransmission(_addr); 
    return 0; //(_wire->endTransmission() == 0);

}

int8_t CH224Q::writeRegister(uint8_t reg, uint8_t value)
{
    Serial.print("CH224Q::writeRegister("); Serial.print(reg, HEX); Serial.print(", "); Serial.print(value, HEX); Serial.println(")");
    if (!_wire) return -1; //check if Wire is initialized

    uint8_t err;

    _wire->beginTransmission(_addr+1); 
    err = _wire->write(reg); //write register address
    Serial.println(err);
    err = _wire->write(value); //write value
    Serial.println(err);
    int r = _wire->endTransmission(); //end transmission, 
    Serial.println(r);
    return r; //0:success, 1:data too long, 2:NACK on address, 3:NACK on data, 4:other error

}

int8_t CH224Q::readRegister(uint8_t reg, uint8_t &value)
{
    if (!_wire) return -1; //check if Wire is initialized

    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false); // Keep connection alive for reading

    uint8_t ret;
    _wire->requestFrom(0x22, 1); // Request 1 byte
    if (_wire->available()) {
        ret = _wire->read();
        //Serial.print("Register 0x");
        //Serial.print(reg, HEX);
        //Serial.print(" value: 0x");
        //Serial.println(ret, HEX);
    }
    else
        return -1; //no read

    value = ret;
    
    uint8_t r = _wire->endTransmission(); //end transmission, 

    return r; //0:success, 1:data too long, 2:NACK on address, 3:NACK on data, 4:other error

}

int8_t CH224Q::requestMode(uint8_t Mode)
{
    // Write the mode value to the MODE_CTRL register

    int8_t err = writeRegister(CH224Q_VOLTAGEMODE_CTRL, Mode);

    Serial.print("err: ");
    Serial.println(err);

    if (err != 0)
        return err; // Return error code if write failed

        
    //datasheet specifies we cant read the CH224Q_VOLTAGEMODE_CTRL register back to confirm mode, so we just look if any Protocoll Handshake has happened 
    //and assume this means the PSU accepted our request
    delay(100); // Small delay to allow mode change to take effect

    uint8_t Mode_Readback = getStatus();

    Serial.print("Readback: ");
    Serial.println(Mode_Readback);

    if (Mode_Readback == 0) //if none of the mode bits are set, handshake failed
    {
        CurrentMode = CH224Q_MODE_UNKNOWN; //reset current mode
        return -1; // Handshake failed
    }

    CurrentMode = Mode; // Update current mode

    switch (Mode)
    {
    case CH224Q_MODE_5V:
        CurrentVoltage_mV = 5000;
        break;
    case CH224Q_MODE_9V:
        CurrentVoltage_mV = 9000;
        break;  
    case CH224Q_MODE_12V:
        CurrentVoltage_mV = 12000;
        break;  
    case CH224Q_MODE_15V:
        CurrentVoltage_mV = 15000;
        break;
    case CH224Q_MODE_20V:
        CurrentVoltage_mV = 20000;
        break;    
    default:
        CurrentVoltage_mV = 0; //default to 0V
        break;
    }

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

PDOInfo CH224Q::decodePDOInfo(uint8_t index)
{
    PDOInfo pdoInfo;
    
    // Each PDO is 4 bytes, starting from CH224Q_SRCCAP_START
    uint8_t regAddress = CH224Q_SRCCAP_START + (index * 4);
    uint32_t pdoValue = 0;

    // Read 4 bytes of the PDO
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t byteValue = 0;
        if (readRegister(regAddress + i, byteValue) != 0) {
            // Error reading register
            return pdoInfo;
        }
        pdoValue |= (static_cast<uint32_t>(byteValue) << (i * 8));
    }

    // Decode the PDO value
    pdoInfo = decodePDO(pdoValue);
    return pdoInfo;
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
        return -1; // Error writing AVX_CTRL1


    CurrentVoltage_mV = voltage_mV; // Update current PPS voltage


    //if current mode is not PPS mode, switch to PPS mode
    if (CurrentMode != CH224Q_MODE_PPS) {
        uint8_t err = requestMode(CH224Q_MODE_PPS);
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

    CurrentVoltage_mV = voltage_mV; // Update current AVS voltage
    //if current mode is not AVS mode, switch to AVS mode
    if (CurrentMode != CH224Q_MODE_AVS) {
        uint8_t err = requestMode(CH224Q_MODE_AVS);
        if (err != 0)
            return err; // Return error code if mode switch failed
    }
    return 0; // Success
}

uint16_t CH224Q::getCurrentSetVoltage_mV()
{
    return CurrentVoltage_mV;
}

uint16_t CH224Q::getMaxCurrent_mA()
{
    readRegister(CH224Q_CURRENT_CAPABILTY, (uint8_t&)CurrentMaxCurrentLimit_mA);
    return CurrentMaxCurrentLimit_mA;
}

uint16_t CH224Q::getMaxPower_mW()
{
    // Multiplay current (in mA) by voltage (in mV) and divide by 1000 to get power in mW
    
    return (CurrentMaxCurrentLimit_mA * CurrentVoltage_mV) / 1000;

}