#include "TinyGPS++.h"
#include "SoftwareSerial.h"
TinyGPSPlus gps;

//SoftwareSerial GPS_Serial(13, 14); // RX, TX
SoftwareSerial ss(13, 14);

void setup() {
  Serial.begin(9600);
  //gps.begin(9600); 
}


void loop() {
   while (ss.available() > 0)
    gps.encode(ss.read);
   if (gps.location.isUpdated()){
    Serial.print("LAT="); Serial.print(gps.location.lat(), 6);
    Serial.print("LNG="); Serial.println(gps.location.lng(), 6);
  }
      
}
