/*
 * MiniESP32-PD Example: USB Power Delivery (PD) Info Reader
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


  uint32_t pdoValue[12] = {0};
  PDOInfo pdo[12];

  uint8_t num_pdos = ch224q->getNumberPDOs();
  Serial.printf("Number of PDOs: %d\n", num_pdos);

  for (uint8_t i = 0; i < num_pdos; i++)
  {
    pdoValue[i] = ch224q->getPDORawValue(i); //get raw PDO value
    pdo[i] = decodePDO(pdoValue[i]); //Decode the Raw PDO value into a PDO Object
  }


  for (uint8_t i = 0; i < num_pdos; i++)
  {
    String pdoStr;
    PDO2String(pdo[i], &pdoStr);
    Serial.println(pdoStr);
  }

}

void loop() {



  //get all PDO Infos and print them


  delay(2000); //wait 10 seconds before next read

}
