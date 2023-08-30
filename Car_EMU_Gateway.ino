#include <Arduino.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <SPI.h>
#include <itss46x.h>
#include <tss46x_van.h>
#include <mcp_can.h>
#include "Module_Definitions.h"
#include "External_Config.h"
#include "Data_Handler.h"
#include "Read_Can.h"
#include <tss463.h>

SPIClass* spi;
ITss46x* vanSender;
//TSS46X_VAN* VANInterface;

const int VAN_CS_PIN = 9;  //Pro Mini

unsigned long currentTime = millis();
unsigned long previousTime = millis();

void setup() {
  Serial.begin(19200);
  Serial.println("TSS463 dashboard experiment");

pinMode(oil_Level    , INPUT);
pinMode(fuel_Level   , INPUT);
pinMode(left_Blinker , INPUT);
pinMode(right_Blinker, INPUT);
pinMode(hazards      , INPUT);
pinMode(low_Beam     , INPUT);
pinMode(high_Beam    , INPUT);
pinMode(front_Fog    , INPUT);
pinMode(rear_Fog     , INPUT);

  // Read EEprom Config Stored Values
  // if (EEPROM.read(0) != 255) GaugeD  = EEPROM.read(0);
  // Gatewahy Type
  //byte Can_Can = 0;
  //byte Can_Van = 1;

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s Witb PCB othewise 8
  while (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {
    Serial.println("CAN0 MCP Init Fail");
    delay(500);
  }
  if(Can_Can == 1){
    while (CAN1.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {
      Serial.println("CAN1 MCP Init Fail");
      delay(500);
    }
  }
  // Set Filters CAN0
  pinMode(CAN0_INT, INPUT);          // Configuring pin for /INT input
  CAN0.init_Mask(0, 0, 0x07E80000);  // Init first mask...
  CAN0.init_Filt(0, 0, 0x07E80000);  // Init first filter...
  CAN0.init_Mask(1, 0, 0x07FF0000);  // Init second mask...
  CAN0.init_Filt(2, 0, 0x06880000);  // Init third filter...
  CAN0.setMode(MCP_NORMAL);  // Set operation mode to normal so the MCP2515 sends acks to received data.

  // Calculate data and load Memory HERE !!!

  spi = new SPIClass();
  spi->begin();

  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  OCR1A = 12499; // = 16000000 / (64 * 20) - 1 (must be <65536)
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei(); 

  // Set Watchdog Timer
  WDTCSR |= 0b00011000;
  WDTCSR = 0b00001000 | WDTO_2S;  //Set WDT based user setting and for 2 second interval
  wdt_reset();

  vanSender = new Tss463(VAN_CS_PIN, spi);
  VANInterface = new TSS46X_VAN(vanSender, VAN_125KBPS);
  VANInterface->begin();
  Send4FC_V1(0);
  //VANInterface->set_value_in_channel(0, 0, 0x80);
  //delay(1000);
  //VANInterface->reactivate_channel(0);
  //delay(1000);
  Send824(1);
  QueryInstrumentClusterForMileage(2);
  Ack664(3);
  Send8A4(4, 18);
  Send524(5);
  delay(1000);
  VANInterface->set_value_in_channel(0, 5, 0x00);
}

ISR(TIMER1_COMPA_vect){//timer1 interrupt 20Hz 
  ReadData();
  wdt_reset();
}

void loop() {
  currentTime = millis();
  if ((currentTime - previousTime) >= 50) {
      previousTime = currentTime;
    //VANInterface->set_value_in_channel(1, 0, Test);Test++;
    // reactivating a channel is much faster than setting up the whole data every time
    Send4FC_Data();
    VANInterface->set_value_in_channel(0, 3, Service_A);
    VANInterface->set_value_in_channel(0, 4, Service_B);
    VANInterface->set_value_in_channel(0, 7, Oil_temp);
    VANInterface->set_value_in_channel(0, 8, Fuel_Level);
    VANInterface->set_value_in_channel(0, 9, Oil_Level);

    VANInterface->set_value_in_channel(1, 3, Service_A);

    VANInterface->reactivate_channel(0);  // Oilt level , temp , Lightning / Signaling data 6
    VANInterface->reactivate_channel(1);  // RPM and Speed
    VANInterface->reactivate_channel(2);  //
    VANInterface->reactivate_channel(3);
    VANInterface->reactivate_channel(4);
    //
    VANInterface->reactivate_channel(5); // Handbrake water temp high and other indications
    wdt_reset();
  }
}