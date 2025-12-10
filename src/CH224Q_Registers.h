#pragma once

#define CH224Q_STATUS                   0x09  //Protocol Status Register, Bit 0-4 inicates weather a protocoll handshake was successfull | Read-Only
#define CH224Q_MODE_CTRL                0x0A  //Voltage Mode Control Register, write some bits into it to request Fixed PDO's or PPS-Mode or AVS-Mode | Write-Only
#define CH224Q_CURRENT_MEAS             0x50  //Contains the maximum available current under the current PD-Mode. (50mA per LSB) | Read-Only
#define CH224Q_AVX_CTRL1                0x51  //AVX_CTRL1/2 Registers contains two bytes that requests a Voltage in AVX-Mode (100mV per LSB) | Write-Only
#define CH224Q_AVX_CTRL2                0x52  //AVX_CTRL2 contains the lower 8 Bits, AVX_CTRL1 the upper 7 Bits plus a enable Bit (hightes bit)

#define CH224Q_SRCCAP_START             0x60  //Contains the Source Capabilities as sent by the Power-Source. There are registers 0x60 to 0x8F this registers are holding a series of 32bit PDO information which have to be decoded | Read-Only
#define CH224Q_SRCCAP_END               0x8F  //End of Source Capabilities


#define CH224Q_STATUS_BC_ACTIVATED      (1)  //Bit 0: Battery Charging Protocol Activated (up to 1.5A @ 5V), Uses USB D+/D- for handshaking
#define CH224Q_STATUS_QC2_ACTIVATED     (2)  //Bit 1: Quick-Charge 2 Protocol Activated (up to 2A @ 5V, 9V or 12V), Uses USB D+/D- for handshaking
#define CH224Q_STATUS_QC3_ACTIVATED     (4)  //Bit 2: QC3 Protocol Activated (up to 3A @ 3.6V to 20V), Uses USB D+/D- for handshaking
#define CH224Q_STATUS_PD_ACTIVATED      (8)  //Bit 3: USB-PD Protocol Activated (up to 5A @ 5V to 20V), Uses CC1/CC2 for handshaking
#define CH224Q_STATUS_EPR_ACTIVATED     (16)  //Bit 4: USB-PD with Extended Power Range Activated (up to 5A @ 5V to 28V), Uses CC1/CC2 for handshaking

#define CH224Q_MODE_CTRL_PDO_5V_BIT     (1)  //Bit 4: Set this bit to request 5V PDO from the Power-Source
#define CH224Q_MODE_CTRL_PDO_9V_BIT     (2)  //Bit 5: Set this bit to request 9V PDO from the Power-Source
#define CH224Q_MODE_CTRL_PDO_12V_BIT    (4) //Bit 6: Set this bit to request 12V PDO from the Power-Source
#define CH224Q_MODE_CTRL_PDO_15V_BIT    (8) //Bit 7: Set this bit to request 15V PDO from the Power-Source
#define CH224Q_MODE_CTRL_PDO_20V_BIT    (16) //Bit 8: Set this bit to request 20V PDO from the Power-Source
#define CH224Q_MODE_CTRL_PDO_28V_BIT    (32) //Bit 9: Set this bit to request 28V PDO from the Power-Source (EPR Mode)
#define CH224Q_MODE_CTRL_PPS_MODE_BIT   (64) //Bit 10: Set this bit to enable PPS-Mode (Programmable Power Supply) if supported by the Power-Source
#define CH224Q_MODE_CTRL_AVS_MODE_BIT   (128) //Bit 11: Set this bit to enable AVS-Mode (Adjustable Voltage Supply) if supported by the Power-Source