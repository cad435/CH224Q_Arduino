/*
 * CH224Q Example: USB Power Delivery PPS Conscutive Loop requests 
 * 
 * This example demonstrates how to read and display Power Delivery Object (PDO)
 * information from a USB PD charger using the CH224Q controller.
 * 
 * The program queries all available PDOs and prints their specifications
 * then it attempts to loop through all avalilable PPS Voltages in 1V steps.
 * 
 * by 4R3N(cad435) 2026-01-11
 * 
 */

#include <Arduino.h>
#include <CH224Q_Arduino.h>
#include <CH224Q_Registers.h>
#include <CH224Q_PDO_Decoder.h>


CH224Q* ch224q;

uint32_t min_pps_voltage_mV = 28000; //start with max PPS voltage
uint32_t max_pps_voltage_mV = 0; //start with min PPS voltage

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(2000);
  while (!Serial); //wait for serial

  Serial.println("CH224Q Example");

  ch224q = new CH224Q();
  delay(500); //wait for charger to setup everything

  int8_t e = ch224q->begin();
  if (e != 0)
  {
    Serial.println("CH224Q initialisation failed!");
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

  //getting the minimum and maximum PPS Voltage values from the available APDOs


  for (uint8_t i = 0; i < num_pdos; i++)
  {
    if (pdo[i].type == Variable || pdo[i].type == Augmented) //Variable or APDO (PPS)
    {
      if (pdo[i].min_voltage_mV < min_pps_voltage_mV)
      {
        min_pps_voltage_mV = pdo[i].min_voltage_mV;
      }
      if (pdo[i].max_voltage_mV > max_pps_voltage_mV)
      {
        max_pps_voltage_mV = pdo[i].max_voltage_mV;
      }
    }    
  }  


  if (min_pps_voltage_mV > max_pps_voltage_mV) //the values were not updated, thus no APDOs found
  {
    Serial.println("No PPS/APDO PDOs found, aborting!");
    while (true)
    { }
  }
  else
  {
    Serial.print("PPS Voltage Range: ");
    Serial.print(min_pps_voltage_mV);
    Serial.print("mV to ");
    Serial.print(max_pps_voltage_mV);
    Serial.println("mV");
  }
  

}

void loop() {

  //loop through all Fixed PDO modes

  for (uint32_t i = min_pps_voltage_mV; i <= max_pps_voltage_mV; i=i+1000)
  {
    Serial.print("Request ");
    Serial.print(i);
    Serial.println("mV PPS"); 
    ch224q->requestPPSVoltage_mv(i); //request 18V in PPS mode
    delay(3000); //wait for mode to settle. 3s seems to be a good value for consecutive requests without the power-supplys crashing
  }
  
}