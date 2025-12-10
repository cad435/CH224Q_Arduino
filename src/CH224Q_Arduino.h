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

/*
    CH224Q device wrapper (generic)
    - Uses TwoWire (I2C) by default
    - Minimal read/write register helpers suitable for many I2C-based devices
    - Basic interrupt attach helper
*/

#define CH224Q_DEFAULT_I2C_ADDRESS 0x22

enum CH224Q_MODE {
    CH224Q_PDO_5V = CH224Q_MODE_CTRL_PDO_5V_BIT,
    CH224Q_PDO_9V = CH224Q_MODE_CTRL_PDO_9V_BIT,
    CH224Q_PDO_12V = CH224Q_MODE_CTRL_PDO_12V_BIT,
    CH224Q_PDO_15V = CH224Q_MODE_CTRL_PDO_15V_BIT,
    CH224Q_PDO_20V = CH224Q_MODE_CTRL_PDO_20V_BIT,
    CH224Q_PDO_28V = CH224Q_MODE_CTRL_PDO_28V_BIT,
    CH224Q_PPS_MODE = CH224Q_MODE_CTRL_PPS_MODE_BIT,
    CH224Q_AVS_MODE = CH224Q_MODE_CTRL_AVS_MODE_BIT
};

enum CH224Q_STATUS {
    CH224Q_NONONE_ACTIVATED = 0,
    CH224Q_BC_ACTIVATED = CH224Q_STATUS_BC_ACTIVATED,
    CH224Q_QC2_ACTIVATED = CH224Q_STATUS_QC2_ACTIVATED,
    CH224Q_QC3_ACTIVATED = CH224Q_STATUS_QC3_ACTIVATED,
    CH224Q_PD_ACTIVATED = CH224Q_STATUS_PD_ACTIVATED,
    CH224Q_EPR_ACTIVATED = CH224Q_STATUS_EPR_ACTIVATED
};


class CH224Q {
public:

    CH224Q(TwoWire* wire = &Wire); //Constructor
    uint8_t begin(uint8_t address = CH224Q_DEFAULT_I2C_ADDRESS);

    uint8_t requestMode(CH224Q_MODE Mode);
    CH224Q_STATUS getStatus();




private:

    uint8_t writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg, uint8_t &value);

    TwoWire* _wire;
    uint8_t _addr;
};