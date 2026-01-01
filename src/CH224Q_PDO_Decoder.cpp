#include "CH224Q_PDO_Decoder.h"

PDOInfo decodePDO(uint32_t pdoRawValue) {
    PDOInfo info;

    info.raw = pdoRawValue;
    uint8_t t = static_cast<uint8_t>((pdoRawValue >> 30) & 0x3u);

    uint16_t voltage;
    uint16_t current;

    switch (t) {
        case 0: // Fixed

            // Fixed Supply PDO
            voltage = (pdoRawValue >> 10) & 0x3FF; //Voltage[19:10]=10bits (50mV)
            current = pdoRawValue & 0x3FF; //Current [9:0]=10bits (10mA)
            info.min_voltage_mV = voltage * 50;
            info.max_voltage_mV = info.min_voltage_mV;
            info.max_current_mA = current * 10;
            info.max_power_mW = info.max_voltage_mV * info.max_current_mA / 1000;
            info.type = PDOType::Fixed;
            break;

        case 1: // Battery
            info.type = PDOType::Battery;
            info.max_voltage_mV = ((pdoRawValue >> 20) & 0x3FFu) * 50u; // MaxVoltage[19:10]=10bits (50mV)
            info.min_voltage_mV = ((pdoRawValue >> 10) & 0x3FFu) * 50u; // MinVoltage[9:0]=10bits (50mV)
            info.max_power_mW = (pdoRawValue & 0x3FFu) * 250u;          // MaxPower[9:0]=10bits (250mW)
            info.max_current_mA = 0; // not part of battery PDO
            break;

        case 2: // Variable
            
            info.type = PDOType::Variable;
            info.max_voltage_mV = ((pdoRawValue >> 20) & 0x3FFu) * 50u; // MaxVoltage[19:10]=10bits (50mV)
            info.min_voltage_mV = ((pdoRawValue >> 10) & 0x3FFu) * 50u; // MinVoltage[9:0]=10bits (50mV)
            info.max_current_mA = (pdoRawValue & 0x3FFu) * 10u;         // Current[9:0]=10bits (10mA)
            break;

        case 3: // Augmented / APDO (PPS)
            info.type = PDOType::Augmented;
            // APDO uses different widths: Vmax[24:17]=8bits (100mV), Vmin[15:8]=8bits (100mV), I[6:0]=7bits (50mA)
            info.max_voltage_mV = ((pdoRawValue >> 17) & 0xFFu) * 100u; // 100 mV units
            info.min_voltage_mV = ((pdoRawValue >> 8) & 0xFFu) * 100u;  // 100 mV units
            info.max_current_mA = (pdoRawValue & 0x7Fu) * 50u;          // 50 mA units
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
            *str = "Augmented PDO (PPS): " + String(pdo.max_current_mA/1000.0f) + "A from " + String(pdo.min_voltage_mV/1000.0f) + "V to " + String(pdo.max_voltage_mV/1000.0f) + "V";
            break;
        default:
            *str = "Unknown PDO Type";
            break;
    }
}