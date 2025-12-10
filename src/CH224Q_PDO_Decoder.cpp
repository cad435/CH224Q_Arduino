#include "CH224Q_PDO_Decoder.h"

PDOInfo CH224Q_PDO_Decoder::decode(uint32_t pdo) {
        PDOInfo info;

        info.raw = pdo;
        uint8_t t = static_cast<uint8_t>((pdo >> 30) & 0x3u);
        switch (t) {
            case 0: // Fixed
                info.type = PDOType::Fixed;
                info.voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u; // 50 mV units
                info.min_voltage_mV = info.voltage_mV;
                info.max_voltage_mV = info.voltage_mV;
                info.current_mA = (pdo & 0x3FFu) * 10u; // 10 mA units
                // max_power_mW left 0 (not part of fixed PDO)
                break;

            case 1: // Battery
                info.type = PDOType::Battery;
                info.max_voltage_mV = ((pdo >> 20) & 0x3FFu) * 50u; // 50 mV units
                info.min_voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u;
                info.voltage_mV = info.max_voltage_mV; // nominal set to max for convenience
                info.max_power_mW = (pdo & 0x3FFu) * 250u; // 250 mW units
                break;

            case 2: // Variable
                info.type = PDOType::Variable;
                info.max_voltage_mV = ((pdo >> 20) & 0x3FFu) * 50u;
                info.min_voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u;
                info.voltage_mV = info.max_voltage_mV;
                info.current_mA = (pdo & 0x3FFu) * 10u;
                break;

            case 3: // Augmented / APDO (PPS)
                info.type = PDOType::Augmented;
                // APDO uses different widths: Vmax[29:17]=13bits (100mV), Vmin[16:8]=9bits (100mV), I[7:0]=8bits (50mA)
                info.max_voltage_mV = ((pdo >> 17) & 0x1FFFu) * 100u; // 100 mV units
                info.min_voltage_mV = ((pdo >> 8) & 0x1FFu) * 100u;   // 100 mV units
                info.voltage_mV = info.max_voltage_mV;
                info.current_mA = (pdo & 0xFFu) * 50u;                // 50 mA units
                break;

            default:
                info.type = PDOType::Unknown;
                break;
        }
        return info;
    }