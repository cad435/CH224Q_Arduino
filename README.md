# CH224Q_Arduino
Arduino Library to control the CH224Q &amp; CH224A via I2C


This is an In-Progress Library. Its tested with the CH224Q. Connect the CH224Q via the I2C lines to your MCU.

You can read the Source-Capabilities from the Power-Source and request Voltages.
Should be working with PPS (Variable Power-Supplies) as well as EPR (Extended Power-Range).


See https://github.com/cad435/CH224Q_Arduino/tree/main/examples/ for examples

Should be working for any MCU as long as the TwoWire Interface from arduino (Wire.h) is accessible.
