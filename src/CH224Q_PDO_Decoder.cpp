#include "CH224Q_PDO_Decoder.h"

PDOInfo decodePDO(uint32_t pdo) {
        PDOInfo info;

        info.raw = pdo;
        uint8_t t = static_cast<uint8_t>((pdo >> 30) & 0x3u);
        switch (t) {
            case 0: // Fixed
                info.type = PDOType::Fixed;
                info.min_voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u; // 50 mV units
                info.max_voltage_mV = info.min_voltage_mV;
                info.current_mA = (pdo & 0x3FFu) * 10u; // 10 mA units
                // max_power_mW left 0 (not part of fixed PDO)
                break;

            case 1: // Battery
                info.type = PDOType::Battery;
                info.max_voltage_mV = ((pdo >> 20) & 0x3FFu) * 50u; // 50 mV units
                info.min_voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u;
                info.max_power_mW = (pdo & 0x3FFu) * 250u; // 250 mW units
                break;

            case 2: // Variable
                info.type = PDOType::Variable;
                info.max_voltage_mV = ((pdo >> 20) & 0x3FFu) * 50u;
                info.min_voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u;
                info.current_mA = (pdo & 0x3FFu) * 10u;
                break;

            case 3: // Augmented / APDO (PPS)
                info.type = PDOType::Augmented;
                // APDO uses different widths: Vmax[29:17]=13bits (100mV), Vmin[16:8]=9bits (100mV), I[7:0]=8bits (50mA)
                info.max_voltage_mV = ((pdo >> 17) & 0x1FFFu) * 100u; // 100 mV units
                info.min_voltage_mV = ((pdo >> 8) & 0x1FFu) * 100u;   // 100 mV units
                info.current_mA = (pdo & 0xFFu) * 50u;                // 50 mA units
                break;

            default:
                info.type = PDOType::Unknown;
                break;
        }
        return info;
    }


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