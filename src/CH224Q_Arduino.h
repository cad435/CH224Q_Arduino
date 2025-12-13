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
    int8_t begin(uint8_t address = CH224Q_DEFAULT_I2C_ADDRESS);

    int8_t requestMode(uint8_t Mode); //requests either Fixeds PDO or PPS/AVX mode from the PD-Source
    uint8_t getStatus(); //returns CH224Q_STATUS_REGISTER status bits. Indicate if a protocol handshake was successful and if so which one

    int8_t getNumberPDOs(); //how many PDOs are available from the source capabilities. CH224Q can handle up to 12 PDOs
    PDOInfo decodePDOInfo(uint8_t index); //get decoded PDO info at given index (0-based)

    int8_t requestPPSVoltage_mv(uint16_t voltage_mV); //requests the desired PPS voltage in mV (5000 to 28000 mV) from the PD-Source. Will automatically request PPS mode if not already set
    int8_t requestAVSVoltage_mv(uint16_t voltage_mV); //requests the desired AVS voltage in mV (5000 to 20000 mV) from the PD-Source. Will automatically request AVS mode if not already set


    //TODO:
    uint16_t getCurrentSetVoltage_mV(); //get currently set max voltage in mV
    uint16_t getMaxCurrent_mA(); //get currently set max current in mA. Might be invalid if chip operates in QC/BC mode
    uint16_t getMaxPower_mW();   //get currently set max power in mW




private:

    int8_t writeRegister(uint8_t reg, uint8_t value);
    int8_t readRegister(uint8_t reg, uint8_t &value);

    TwoWire* _wire;
    uint8_t _addr;

    uint8_t CurrentMode = CH224Q_MODE_UNKNOWN; //default 5V PDO mode

    uint16_t CurrentMaxCurrentLimit_mA = 0; //currently set current limit in mA (0 if not set). Might be invalid if chip operates in QC/BC mode
    uint16_t CurrentVoltage_mV = 0; //currently set voltage in mV (0 if not set)

};