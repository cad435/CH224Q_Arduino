#include "CH224Q_Arduino.h"
#include "CH224Q_Registers.h"
#include "CH224Q_PDO_Decoder.h"

CH224Q::CH224Q(TwoWire* wire)
{
    _wire = wire;
}

int8_t CH224Q::begin(uint8_t address)
{
    _addr = address;
    if (!_wire) return false; //return false if no wire instance
    _wire->begin(); //initialize I2C bus
    // simple probe by zero-length transmission
    _wire->beginTransmission(_addr); 
    return (_wire->endTransmission() == 0);

}

int8_t CH224Q::writeRegister(uint8_t reg, uint8_t value)
{
    if (!_wire) return -1; //check if Wire is initialized

    _wire->beginTransmission(_addr); 
    _wire->write(reg); //write register address
    _wire->write(value); //write value
    int r = _wire->endTransmission(); //end transmission, 

    return r; //0:success, 1:data too long, 2:NACK on address, 3:NACK on data, 4:other error

}

int8_t CH224Q::readRegister(uint8_t reg, uint8_t &value)
{
     if (!_wire) return -1; //check if Wire is initialized
    _wire->beginTransmission(_addr); 
    _wire->write(reg); //write register address
    _wire->endTransmission(false); //end transmission, do not send stop
    uint8_t got = _wire->requestFrom(_addr, (uint8_t)1);
    if (got == 0) 
            return -1; //error: no data
    value = got;
    
    int r = _wire->endTransmission(); //end transmission, 
    return r; //0:success, 1:data too long, 2:NACK on address, 3:NACK on data, 4:other error

}

int8_t CH224Q::requestMode(uint8_t Mode)
{
    // Write the mode value to the MODE_CTRL register

    int8_t err = writeRegister(CH224Q_VOLTAGEMODE_CTRL, Mode);

    if (err != 0)
        return err; // Return error code if write failed

        
    //datasheet specifies we cant read the CH224Q_VOLTAGEMODE_CTRL register back to confirm mode, so we just look if any Protocoll Handshake has happened 
    //and assume this means the PSU accepted our request
    delay(100); // Small delay to allow mode change to take effect

    if (getStatus() == 0) //if none of the mode bits are set, handshake failed
    {
        CurrentMode = CH224Q_MODE_UNKNOWN; //reset current mode
        return -1; // Handshake failed
    }

    CurrentMode = Mode; // Update current mode

    switch (Mode)
    {
    case CH224Q_MODE_CTRL_5V_BIT:
        CurrentVoltage_mV = 5000;
        break;
    case CH224Q_MODE_CTRL_9V_BIT:
        CurrentVoltage_mV = 9000;
        break;  
    case CH224Q_MODE_CTRL_12V_BIT:
        CurrentVoltage_mV = 12000;
        break;  
    case CH224Q_MODE_CTRL_15V_BIT:
        CurrentVoltage_mV = 15000;
        break;
    case CH224Q_MODE_CTRL_20V_BIT:
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
    readRegister(CH224Q_STATUS_REGISTER, registerValue);

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
}

PDOInfo CH224Q::getPDOInfo(uint8_t index)
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
    if (CurrentMode != CH224Q_MODE_CTRL_PPS_MODE_BIT) {
        uint8_t err = requestMode(CH224Q_MODE_CTRL_PPS_MODE_BIT);
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
    if (CurrentMode != CH224Q_MODE_CTRL_AVS_MODE_BIT) {
        uint8_t err = requestMode(CH224Q_MODE_CTRL_AVS_MODE_BIT);
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
    readRegister(CH224Q_CURRENT_MEAS, (uint8_t&)CurrentMaxCurrentLimit_mA);
    return CurrentMaxCurrentLimit_mA;
}

uint16_t CH224Q::getMaxPower_mW()
{
    // Multiplay current (in mA) by voltage (in mV) and divide by 1000 to get power in mW
    
    return (CurrentMaxCurrentLimit_mA * CurrentVoltage_mV) / 1000;

}