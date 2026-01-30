/*
 * MiniESP32-PD Example: USB Power Delivery Fixed PDO Request
 * 
 * This example demonstrates how to read and display Power Delivery Object (PDO)
 * information from a USB PD charger using the CH224Q controller.
 * 
 * The program queries all available PDOs and prints their specifications
 * including voltage range, maximum current, and power capacity.
 * 
 * by 4R3N(cad435) 2025-12-13
 * 
 */

#include <Arduino.h>
#include <CH224Q_Arduino.h>
#include <CH224Q_Registers.h>
#include <CH224Q_PDO_Decoder.h>


CH224Q* ch224q;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(2000);
  while (!Serial); //wait for serial

  Serial.println("CH224Q Example");

  ch224q = new CH224Q(true); //set logging enabled

  delay(500); //wait for charger to setup everything

  int8_t e = ch224q->begin();
  if (e != 0)
  {
    Serial.println("CH224Q initialisation failed!");
    Serial.println("Is the powersupply used capable of USB-PD?");
    while(true);
  }
  Serial.println("CH224Q initialisation success!");
  
  //wait 5s
  delay(5000);

}

void loop() {

  Serial.println("Request 5V Fixed PDO");  
  ch224q->setMode(CH224Q_MODE_5V);
  delay(5000); //wait for mode to settle
  
  Serial.println("Request 9V Fixed PDO");  
  ch224q->setMode(CH224Q_MODE_9V);
  delay(5000); //wait for mode to settle 
  
  Serial.println("Request 12V Fixed PDO");  
  ch224q->setMode(CH224Q_MODE_12V);
  delay(5000); //wait for mode to settle
  
  Serial.println("Request 15V Fixed PDO");
  ch224q->setMode(CH224Q_MODE_15V);
  delay(5000); //wait for mode to settle
  
  Serial.println("Request 20V Fixed PDO");
  ch224q->setMode(CH224Q_MODE_20V);
  delay(5000); //wait for mode to settle
  
  Serial.println("Request 28V Fixed PDO(EPR)");
  ch224q->setMode(CH224Q_MODE_28V);
  delay(5000); //wait for mode to settle


}
