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

//#define CH224Q_DEBUG

#define CH224Q_DEFAULT_I2C_ADDRESS 0x22



class CH224Q {
public:

    CH224Q(bool logging = false, TwoWire* _wire = &Wire); //Constructor
    int8_t begin(uint8_t address = CH224Q_DEFAULT_I2C_ADDRESS);

    int8_t setMode(uint8_t Mode); //requests either Fixeds PDO or PPS/AVX mode from the PD-Source
    uint8_t getStatus(); //returns CH224Q_STATUS_REGISTER status bits. Indicate if a protocol handshake was successful and if so which one

    int8_t getNumberPDOs(); //how many PDOs are available from the source capabilities. CH224Q can handle up to 12 PDOs
    uint32_t getPDORawValue(uint8_t index); //get raw PDO value at given index (0-based)

    int8_t requestPPSVoltage_mv(uint16_t voltage_mV); //requests the desired PPS voltage in mV (5000 to 28000 mV) from the PD-Source. Will automatically request PPS mode if not already set
    int8_t requestAVSVoltage_mv(uint16_t voltage_mV); //requests the desired AVS voltage in mV (5000 to 20000 mV) from the PD-Source. Will automatically request AVS mode if not already set


    uint16_t getMaxCurrent_mA(); //get currently set max current in mA. Might be invalid if chip operates in QC/BC mode

    int8_t writeRegister(uint8_t reg, uint8_t value);


private:

    int8_t readRegister(uint8_t reg, uint8_t &value);    

    TwoWire* wire;
    uint8_t addr;

    uint8_t CurrentMode = CH224Q_MODE_UNKNOWN; //default 5V PDO mode

    uint16_t CurrentMaxCurrentLimit_mA = 0; //currently set current limit in mA (0 if not set). Might be invalid if chip operates in QC/BC mode

};