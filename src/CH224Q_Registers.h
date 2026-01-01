#pragma once

//registers
#define CH224Q_STATUS                   0x09  //Protocol Status Register, Bit 0-4 inicates weather a protocoll handshake was successfull | Read-Only
#define CH224Q_VOLTAGEMODE_CTRL         0x0A  //Voltage Mode Control Register, write some bits into it to request Fixed PDO's or PPS-Mode or AVS-Mode | Write-Only
#define CH224Q_CURRENT_CAPABILTY        0x50  //Contains the maximum available current under the current PD-Mode. (50mA per LSB) | Read-Only
#define CH224Q_AVX_CTRL1                0x51  //AVX_CTRL1/2 Registers contains two bytes that requests a Voltage in AVX-Mode (100mV per LSB) | Write-Only
#define CH224Q_AVX_CTRL2                0x52  //AVX_CTRL2 contains the lower 8 Bits, AVX_CTRL1 the upper 7 Bits plus a enable Bit (hightes bit)
#define CH224Q_PPS_VOLTAGE_CTRL         0x53  //Contains the desired PPS Voltage in 100mV per LSB when in PPS-Mode | Write-Only

#define CH224Q_SRCCAP_META              0x60  //Contains the Source Capabilities as sent by the Power-Source. There are registers 0x60 to 0x8F per datasheet. The first 2 registers however are not used for the actual data
                                              //Most likely Metadata is stored there.
#define CH224Q_SRCCAP_START             0x62  //Contains the Source Capabilities as sent by the Power-Source. Datasheet does not specify what registers 0x60 & 0x61 are holding, but this does not contain valid PDO Data
#define CH224Q_SRCCAP_END               0x8F  //End of Source Capabilities

//defines

//Status defines, can be read out of CH224Q_STATUS register to get which protocoll is activated
#define CH224Q_STATUS_NONE_ACTIVATED    (0)   //No Protocol Activated, not a real register, value 0 indicates no bits set in status register at all
#define CH224Q_STATUS_BC_ACTIVATED      (1)   //Bit 0: Battery Charging Protocol Activated (up to 1.5A @ 5V), Uses USB D+/D- for handshaking
#define CH224Q_STATUS_QC2_ACTIVATED     (2)   //Bit 1: Quick-Charge 2 Protocol Activated (up to 2A @ 5V, 9V or 12V), Uses USB D+/D- for handshaking
#define CH224Q_STATUS_QC3_ACTIVATED     (4)   //Bit 2: QC3 Protocol Activated (up to 3A @ 3.6V to 20V), Uses USB D+/D- for handshaking
#define CH224Q_STATUS_PD_ACTIVATED      (8)   //Bit 3: USB-PD Protocol Activated (up to 5A @ 5V to 20V), Uses CC1/CC2 for handshaking
#define CH224Q_STATUS_EPR_ACTIVATED     (16)  //Bit 4: USB-PD with Extended Power Range Activated (CH224 can handle up to 28V @ 5A), Uses CC1/CC2 for handshaking
#define CH224Q_STATUS_EPR_CAPABILTY     0x20  //Bit 5: is there EPR capability in the used PSU? Not documented in official Datasheet, found here: https://github.com/felixardyansyah/FGV_CH224X/blob/main/src/CH224X_I2C.h
#define CH224Q_STATUS_AVS_CAPABILTY     0x40  //Bit 6: is there AVS capability in the used PSU? Not documented in official Datasheet, found here: https://github.com/felixardyansyah/FGV_CH224X/blob/main/src/CH224X_I2C.h


//mode defines, can be written into CH224Q_VOLTAGEMODE_CTRL register to request a Voltage
#define CH224Q_MODE_UNKNOWN             (-1)   //No Mode Selected, default state after power-on
#define CH224Q_MODE_5V                  0x00  //Bit 4: Set this bit to request 5V from the Power-Source. Works with PD, QC2, QC3 and BC Protocols
#define CH224Q_MODE_9V                  0x01  //Bit 5: Set this bit to request 9V from the Power-Source. Works with PD, QC2, QC3 Protocols
#define CH224Q_MODE_12V                 0x02 //Bit 6: Set this bit to request 12V from the Power-Source. Works with PD, QC2, QC3 Protocols
#define CH224Q_MODE_15V                 0x03 //Bit 7: Set this bit to request 15V from the Power-Source. Works with PD and QC3 Protocols
#define CH224Q_MODE_20V                 0x04 //Bit 8: Set this bit to request 20V from the Power-Source. Works with PD and QC3 Protocols
#define CH224Q_MODE_28V                 0x05 //Bit 9: Set this bit to request 28V from the Power-Source. Works with EPR (Extended Power Range) PD Protocols
#define CH224Q_MODE_PPS                 0x06 //Bit 10: Set this bit to enable PPS-Mode (Programmable Power Supply) if supported by the Power-Source. Requires PD3.x Protocol
#define CH224Q_MODE_AVS                 0x07 //Bit 11: Set this bit to enable AVS-Mode (Adjustable Voltage Supply) if supported by the Power-Source. Requires PD3.2 Protocol