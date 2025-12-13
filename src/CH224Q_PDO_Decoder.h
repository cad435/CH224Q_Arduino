// CH224Q_PDO_Decoder.h
// Decode 32-bit USB-PD Source Capability (PDO/APDO) entries into voltages, currents and power.
// Intended for use with USB-PD v2/v3 common PDO encodings.
//
// Notes (common encodings used here):
//  - PDO Type: bits 31..30 (0 = Fixed, 1 = Battery, 2 = Variable, 3 = Augmented/APDO (PPS))
//  - Fixed:   V = bits[19:10] * 50 mV, I = bits[9:0] * 10 mA
//  - Battery: MaxV = bits[29:20] * 50 mV, MinV = bits[19:10] * 50 mV, P = bits[9:0] * 250 mW
//  - Variable: MaxV = bits[29:20] * 50 mV, MinV = bits[19:10] * 50 mV, I = bits[9:0] * 10 mA
//  - APDO (PPS): MaxV = bits[29:17] * 100 mV, MinV = bits[16:8] * 100 mV, I = bits[7:0] * 50 mA
//
// This header provides a lightweight decoder that converts a raw 32-bit PDO into human-friendly units.
// It intentionally focuses on voltage/current/power fields used most frequently.

#ifndef CH224Q_PDO_DECODER_H
#define CH224Q_PDO_DECODER_H

#include <cstdint>

enum PDOType{
        Fixed     = 0,
        Battery   = 1,
        Variable  = 2,
        Augmented = 3, // APDO / PPS
        Unknown   = 0xFF
    };

struct PDOInfo {
    uint32_t raw = 0;             // original 32-bit PDO value
    PDOType  type = PDOType::Unknown;

    // For fixed: voltage_mV = nominal =  min = max.
    // For variable/battery/APDO: min/max valid if set (0 if unused).
    uint32_t min_voltage_mV = 0;
    uint32_t max_voltage_mV = 0;

    // Current fields (max/current depending on type). 0 if unused.
    uint32_t current_mA = 0;      // max current for fixed/variable/APDO
    uint32_t max_power_mW = 0;    // for battery PDO (max power)

    bool valid() const { return type != PDOType::Unknown; }
};
    // Decode a single 32-bit PDO into PDOInfo.
    PDOInfo decodePDO(uint32_t pdo);


#endif // CH224Q_PDO_DECODER_H
