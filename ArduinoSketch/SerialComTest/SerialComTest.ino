
/*
  SERCOM Test
  
  Test the ability to add extra hardware serial ports to the MKR1000
  This sketch has the following serial interfaces:
    Serial  - Native USB interface
    Serial1 - Default serial port on D13, D14 (Sercom 5)
    Serial2 - Extra serial port on D0, D1 (Sercom 3)
    Serial3 - Extra serial port on D4, D5 (Sercom 4)
    
  Good explanation of sercom funcationality here: 
  https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/muxing-it-up

  This sketch will echo characters recieved on any of the 4 serial ports to all other serial ports.

  for Arduino MKR1000
  by Tom Kuehn
  26/06/2016

*/

#include <Arduino.h>                              // required before wiring_private.h
#include <wiring_private.h>
#include "TinyGPS++.h"

TinyGPSPlus gps;

static const char MKR1000_LED       = 6;


void setup()
{
  static boolean state = HIGH;
  static unsigned char count = 0;
  
  pinMode(MKR1000_LED, OUTPUT);
  Serial1.begin(9600);
   
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(9600);
  while (!Serial && (count < 30) )
  {
    delay(200);     // Wait for serial port to connect with timeout. Needed for native USB
    digitalWrite(MKR1000_LED, state);
    state = !state;
    count++;
  }

 digitalWrite(MKR1000_LED, HIGH);
 Serial.println("Setup Complete");
 Serial1.println("Setup Complete");

}

void loop()
{
  static unsigned long ms = 0;
  static boolean state = HIGH;
  static unsigned char c = 0;

  if (Serial.available())
  {
    char c = Serial.read();
//    Serial.print(c);
//    Serial1.print(c);
//    gps.encode(c);
//    if (gps.altitude.isUpdated()){
//      Serial.println("I am in");
//      Serial.println(gps.altitude.meters());
//      Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
//      Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
//    }

  }
  if (Serial1.available())
  {
    char c = Serial1.read();
//    Serial.print(c);
//    Serial1.print(c);
//    Serial2.print(c);
//    Serial3.print(c);
    gps.encode(c);
    if (gps.altitude.isUpdated()){
      Serial.println("I am in");
      Serial.println(gps.altitude.meters());
      Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
      Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
    }
  }



  if (millis() - ms > 100)
  {
    ms = millis();
    digitalWrite(MKR1000_LED, state);
    state = !state;
  }

}
