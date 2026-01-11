# CH224Q_Arduino
Arduino Library to control the CH224Q &amp; CH224A via I2C


This is an In-Progress Library. Its tested with the CH224Q. Connect the CH224Q via the I2C lines to your MCU.

Tested:
 - Setting Fixed PDO's (e.g. 5V, 9V, 12V, 15V, 20V)
 - Setting PPS Voltages (from 3.3V to 21V)
 - Reading and Decoding Source Capabilities ("getPDORawValue()" and "decodePDO()") --> Some Sources give strange PDO's an the highest capabiliy PDO, others are perfectly fine

Untested: 
 - AVS Mode with EPR (I currently do not have a PSU on hand which has AVS capabilities explicitly marked)


See https://github.com/cad435/CH224Q_Arduino/tree/main/examples/ for examples

Should be working for any MCU as long as the TwoWire Interface from arduino (Wire.h) is accessible.


Release under the MIT License: 2025 4R3N(cad435)
