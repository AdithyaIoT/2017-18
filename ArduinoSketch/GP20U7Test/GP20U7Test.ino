#include <Arduino.h>                              // required before wiring_private.h
#include <wiring_private.h>
#include "TinyGPS++.h"
TinyGPSPlus gps;



void setup()
{
   static boolean state = HIGH;
  static unsigned char count = 0;
  

  Serial1.begin(9600);
   
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(9600);
  while(!Serial);
}
  

void loop()
{
  static unsigned long ms = 0;
  static boolean state = HIGH;
  static unsigned char c = 0;

  if (Serial.available())
  {
    char c = Serial.read();
 
  }
  if (Serial1.available())
  {
    char c = Serial1.read();
    gps.encode(c);
    if (gps.altitude.isUpdated()){
      Serial.println(gps.altitude.meters());
      Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
      Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
    }
  }

 

}
