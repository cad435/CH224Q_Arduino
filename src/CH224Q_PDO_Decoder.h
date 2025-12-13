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

    // For fixed: voltage_mV = nominal, min/max equal nominal.
    // For variable/battery/APDO: min/max valid if set (0 if unused).
    uint32_t voltage_mV = 0;      // nominal (for fixed) or max for types where applicable
    uint32_t min_voltage_mV = 0;
    uint32_t max_voltage_mV = 0;

    // Current fields (max/current depending on type). 0 if unused.
    uint32_t current_mA = 0;      // max current for fixed/variable/APDO
    uint32_t max_power_mW = 0;    // for battery PDO (max power)

    // Convenience: computed power (mW) when voltage and current are available (voltage_mV * current_mA / 1000)
    uint32_t computed_power_mW() const {
        if (voltage_mV && current_mA) {
            return static_cast<uint32_t>((uint64_t)voltage_mV * current_mA / 1000u);
        }
        return 0;
    }

    bool valid() const { return type != PDOType::Unknown; }
};
    // Decode a single 32-bit PDO into PDOInfo.
    PDOInfo decodePDO(uint32_t pdo);


#endif // CH224Q_PDO_DECODER_H


/*

#include <cstdint>
#include <iostream>

struct PDOInfo {
    uint32_t voltage_mV;
    uint32_t current_mA;
    uint32_t power_mW;
    std::string type;
};

PDOInfo decodePDO(uint32_t pdo) {
    PDOInfo info;
    uint8_t pdoType = (pdo >> 30) & 0x3;

    if (pdoType == 0) {
        // Fixed Supply PDO
        uint16_t voltage = (pdo >> 10) & 0x3FF;
        uint16_t current = pdo & 0x3FF;
        info.voltage_mV = voltage * 50;
        info.current_mA = current * 10;
        info.power_mW = info.voltage_mV * info.current_mA / 1000;
        info.type = "Fixed";
    } else if (pdoType == 1) {
        // Battery Supply PDO
        uint16_t maxVoltage = (pdo >> 20) & 0x3FF;
        uint16_t minVoltage = (pdo >> 10) & 0x3FF;
        uint16_t maxPower = pdo & 0x3FF;
        info.voltage_mV = maxVoltage * 50;
        info.current_mA = 0; // Not directly available
        info.power_mW = maxPower * 1000;
        info.type = "Battery";
    } else if (pdoType == 2) {
        // Variable Supply PDO
        uint16_t maxVoltage = (pdo >> 20) & 0x3FF;
        uint16_t minVoltage = (pdo >> 10) & 0x3FF;
        uint16_t current = pdo & 0x3FF;
        info.voltage_mV = maxVoltage * 50;
        info.current_mA = current * 10;
        info.power_mW = info.voltage_mV * info.current_mA / 1000;
        info.type = "Variable";
    } else {
        info.type = "Unknown";
        info.voltage_mV = 0;
        info.current_mA = 0;
        info.power_mW = 0;
    }

    return info;
}
Usage Example
cpp
int main() {
    uint32_t pdo = 0x2A20; // Example PDO value
    PDOInfo info = decodePDO(pdo);
    std::cout << "Type: " << info.type << "\n";
    std::cout << "Voltage: " << info.voltage_mV << " mV\n";
    std::cout << "Current: " << info.current_mA << " mA\n";
    std::cout << "Power: " << info.power_mW << " mW\n";
    return 0;
}


*/