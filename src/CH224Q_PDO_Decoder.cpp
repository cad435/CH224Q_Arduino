#include "CH224Q_PDO_Decoder.h"

PDOInfo decodePDO(uint32_t pdo) {
    PDOInfo info;

    info.raw = pdo;
    uint8_t t = static_cast<uint8_t>((pdo >> 30) & 0x3u);

    uint16_t voltage;
    uint16_t current;

    switch (t) {
        case 0: // Fixed

            // Fixed Supply PDO
            voltage = (pdo >> 10) & 0x3FF;
            current = pdo & 0x3FF;
            info.min_voltage_mV = voltage * 50;
            info.max_voltage_mV = info.min_voltage_mV;
            info.max_current_mA = current * 10;
            info.max_power_mW = info.max_voltage_mV * info.max_current_mA / 1000;
            info.type = PDOType::Fixed;
            break;

        case 1: // Battery
            info.type = PDOType::Battery;
            info.max_voltage_mV = ((pdo >> 20) & 0x3FFu) * 50u; // 50 mV units
            info.min_voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u;
            info.max_power_mW = (pdo & 0x3FFu) * 250u; // 250 mW units
            info.max_current_mA = 0; // not part of battery PDO
            break;

        case 2: // Variable
            info.type = PDOType::Variable;
            info.max_voltage_mV = ((pdo >> 20) & 0x3FFu) * 50u;
            info.min_voltage_mV = ((pdo >> 10) & 0x3FFu) * 50u;
            info.max_current_mA = (pdo & 0x3FFu) * 10u;
            break;

        case 3: // Augmented / APDO (PPS)
            info.type = PDOType::Augmented;
            // APDO uses different widths: Vmax[29:17]=13bits (100mV), Vmin[16:8]=9bits (100mV), I[7:0]=8bits (50mA)
            info.max_voltage_mV = ((pdo >> 17) & 0x1FFFu) * 100u; // 100 mV units
            info.min_voltage_mV = ((pdo >> 8) & 0x1FFu) * 100u;   // 100 mV units
            info.max_current_mA = (pdo & 0xFFu) * 50u;                // 50 mA units
            break;

        default:
            info.type = PDOType::Unknown;
            break;
    }
    return info;
}

void PDO2String(PDOInfo pdo, String* str)
{
    if (!pdo.valid()) {
        *str = "Invalid PDO";
        return;
    }

    switch (pdo.type) {
        case PDOType::Fixed:
            *str = "Fixed PDO: " + String(pdo.max_current_mA/1000.0f) + "A @ " + String(pdo.min_voltage_mV/1000.0f) + "V";
            break;
        case PDOType::Variable:
            *str = "Variable PDO: " + String(pdo.max_current_mA/1000.0f) + "A from " + String(pdo.min_voltage_mV/1000.0f) + "V to " + String(pdo.max_voltage_mV/1000.0f) + "V";
            break;
        case PDOType::Battery:
            *str = "Battery PDO: " + String(pdo.max_power_mW/1000.0f) + "W from " + String(pdo.min_voltage_mV/1000.0f) + "V to " + String(pdo.max_voltage_mV/1000.0f) + "V";
            break;
        case PDOType::Augmented:
            *str = "Augmented PDO (PPS): " + String(pdo.max_current_mA/1000.0f) + "mA from " + String(pdo.min_voltage_mV/1000.0f) + "V to " + String(pdo.max_voltage_mV/1000.0f) + "V";
            break;
        default:
            *str = "Unknown PDO Type";
            break;
    }
}