#include "CH224Q_Arduino.h"
#include "CH224Q_Registers.h"
#include "CH224Q_PDO_Decoder.h"

CH224Q::CH224Q(TwoWire* wire)
{
    _wire = wire;
}

uint8_t CH224Q::begin(uint8_t address)
{
    _addr = address;
    if (!_wire) return false; //return false if no wire instance
    _wire->begin(); //initialize I2C bus
    // simple probe by zero-length transmission
    _wire->beginTransmission(_addr); 
    return (_wire->endTransmission() == 0);

}

uint8_t CH224Q::writeRegister(uint8_t reg, uint8_t value)
{
    if (!_wire) return -1; //check if Wire is initialized

    _wire->beginTransmission(_addr); 
    _wire->write(reg); //write register address
    _wire->write(value); //write value
    int r = _wire->endTransmission(); //end transmission, 

    return r; //0:success, 1:data too long, 2:NACK on address, 3:NACK on data, 4:other error

}

uint8_t CH224Q::readRegister(uint8_t reg, uint8_t &value)
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

uint8_t CH224Q::requestMode(uint8_t Mode)
{
    // Write the mode value to the MODE_CTRL register
    return writeRegister(CH224Q_MODE_CTRL, Mode);
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