#include "CH224Q_Arduino.h"
#include "CH224Q_Registers.h"
#include "CH224Q_PDO_Decoder.h"

CH224Q::CH224Q(TwoWire* wire)
{
    _wire = wire;
}

void CH224Q::begin(uint8_t address = CH224Q_DEFAULT_I2C_ADDRESS)
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

uint8_t CH224Q::requestMode(CH224Q_MODE Mode)
{

    // Write the mode value to the MODE_CTRL register
    return writeRegister(CH224Q_MODE_CTRL, Mode);
} 

CH224Q_STATUS CH224Q::getStatus()
{

    uint8_t registerValue = 0;
    readRegister(CH224Q_STATUS, &registerValue);

    //check which Bit is set and return corresponding status
    if (registerValue & CH224Q_STATUS_BC_ACTIVATED) {
        return CH224Q_BC_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_QC2_ACTIVATED) {
        return CH224Q_QC2_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_QC3_ACTIVATED) {
        return CH224Q_QC3_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_PD_ACTIVATED) {
        return CH224Q_PD_ACTIVATED;
    }
    else if (registerValue & CH224Q_STATUS_EPR_ACTIVATED) {
        return CH224Q_EPR_ACTIVATED;
    }
    else {
        return CH224Q_NONONE_ACTIVATED;
    }
    

        
}