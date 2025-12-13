/*
    CH224Q_Arduino.h - Main header for CH224Q Arduino library
    A minimal, portable library skeleton providing I2C access and basic API.
    Place this file in src/ of your PlatformIO/Arduino library.
    License: MIT (adjust as needed)
*/

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "CH224Q_Registers.h"
#include "CH224Q_PDO_Decoder.h"

/*
    CH224Q device wrapper (generic)
    - Uses TwoWire (I2C) by default
    - Minimal read/write register helpers suitable for many I2C-based devices
    - Basic interrupt attach helper
*/

#define CH224Q_DEFAULT_I2C_ADDRESS 0x22


class CH224Q {
public:

    CH224Q(TwoWire* wire = &Wire); //Constructor
    uint8_t begin(uint8_t address = CH224Q_DEFAULT_I2C_ADDRESS);

    uint8_t requestMode(uint8_t Mode);
    uint8_t getStatus();

    uint8_t getNumberPDOs(); //how many PDOs are available from the source capabilities
    PDOInfo getPDOInfo(uint8_t index); //get decoded PDO info at given index (0-based)

    uint8_t setPPSVoltage_mv(uint16_t voltage_mV); //set desired PPS voltage in mV (5000 to 28000 mV)




private:

    uint8_t writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg, uint8_t &value);

    TwoWire* _wire;
    uint8_t _addr;

    uint16_t PPS_Voltage_mV = 5000; //for PPS mode: currently set voltage in mV

    uint8_t CurrentMode = CH224Q_MODE_UNKNOWN; //default 5V PDO mode

};