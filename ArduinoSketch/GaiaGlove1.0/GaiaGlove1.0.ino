
#include <SPI.h>
#include <WiFi101.h>
#include <Arduino.h>                              // required before wiring_private.h
#include <wiring_private.h>
#include "TinyGPS++.h"
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

//Max30105 sensor for getting the heart rate.
MAX30105 particleSensor;

//GP20u7 GPS receiver.
TinyGPSPlus gps;

int rptBPM[20];
char ssid[] = "AmsKumsHaus";        // your network SSID (name)
char pass[] = "aryashakthi";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
char server[] = " http://77b1bcaf.ngrok.io";    // NGROK server where the JSP for calling the python program for passing the BPM to WATSON is present.
WiFiClient client;

int greenButtonPin = 8;
int redButtonPin = 7;
int LED = 6;

boolean greenButtonPressed = false;
boolean redButtonPressed = false;
boolean triggered = false;

void setup() {
  //Setup the push button pins
  pinMode(greenButtonPin, INPUT_PULLUP); // Set the push buttons
  pinMode(redButtonPin, INPUT_PULLUP); // Set the push buttons
  // Setup both serials for the GPS receiver
  Serial1.begin(9600);  
  Serial.begin(9600);
  while (!Serial)
  Serial.println("GPS Serial Setup Complete");

  // Initialize the heart rate sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  
  Serial.println("Place your index finger on the heart rate sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  
  
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");

}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonValue = HIGH;
  triggered=false;

  white(!triggered){
    buttonValue = digitalRead(redButtonPin);
    if (buttonValue == LOW){
      triggered = true;
      Serial.println("Alert Button Pressed"); //Red button
    } 
//    else {
//      // Otherwise, turn the LED off
//      digitalWrite(ledPin, LOW);
//      Serial.println("Button is in Released State");
//      buttonPressed=false;
//    }
    if (!triggered){
      triggered = checkPulse();  
    }
    if (triggered){
      callUser();
      long curTime = millis();
      buttonValue = digitalRead(greenButtonPin);
      while((millis()<curTime + 30000) && (buttonValue==HIGH)){
         buttonValue = digitalRead(greenButtonPin);
         
         if (buttonValue == LOW)
          triggered = false;
         playSound();
      }
      
    }
    if (triggered){
      sendSMS();
      callCaregiver();
      sendVitalsWatson();
    }
  }
}

boolean checkPulse(){
  const byte RATE_SIZE = 20; 
  byte bpm[RATE_SIZE]; //Array of heart rates
  byte rateSpot = 0;
  long lastBeat = 0; //Time at which the last beat occurred
  float beatsPerMinute;
  int THRESHOLD = 20;
  int beatAvg;
  boolean irregular = true;

  while (rateSpot <RATE_SIZE){
    long irValue = particleSensor.getIR();
  
    if (checkForBeat(irValue) == true) {
  
      long delta = millis() - lastBeat;
      lastBeat = millis();
  
      beatsPerMinute = 60 / (delta / 1000.0);
  
      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        bpm[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable
  
        //Take average of readings
        beatAvg = 0;
        for (byte i = 0 ; i < rateSpot ; i++)
          beatAvg += bpm[i];
        beatAvg /= rateSpot;
      }
    }
  
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);
  
//    if (irValue < 50000)
//      Serial.print(" No finger?");
  
    Serial.println();
  }
  
  for (byte i = 1 ; i < rateSpot ; i++){
    if (bpm[i] - bpm[i-1] > THRESHOLD) {
      irregular = true;
    }
  }
  return irregular;  
}

//The following function plays an alert sound in the piezo buzzer
void playSound() {
  int i, duration;
  const int BUZZERPIN=5;
  int beats[] = {1,1,1,1,1,1,1,1};
  int tempo = 200;
  int frequencies[] = {392, 330, 349, 0, 349, 330, 349, 0};  //Frequencies for g,e,f, ,f,e,f
   
  for (i = 0; i < 8; i++) // step through the song arrays
  {
    duration = beats[i] * tempo;  // length of note/rest in ms

    if (frequencies[i] == 0 )          
    {
      delay(duration);            // pause for a moment
    }
    else                          // otherwise, play the note
    {
      tone(BUZZERPIN, frequencies[i], duration);
      delay(duration);            // wait for tone to finish
    }
    delay(tempo/10);              // brief pause between notes
  }
   // tone(9, 1000, 500);
    //Serial.println("I played a tone");
} 

void callUser(){

  String userPhone = "5104499967";
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    client.println("GET /AdiHealthBand/Index.jsp?bandParam=CallUser&param1="+userPhone +"&param2=" + String(bpm) + " HTTP/1.1");
    client.println("Host: 77b1bcaf.ngrok.io");
    client.println("Connection: close");
    client.println();
    /*
    char startByte = '0';
    while((int)startByte != '1'){
    startByte = Serial.read();
    }*/

  }
  //while (!client.available());

  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (client.connected()) {
    client.flush();
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
  }
}

void sendVitalsWatson () {
  const byte RATE_SIZE = 20; 
  const byte MAX_SIZE = 500;
  float beatsPerMin[MAX_SIZE];
  float temp[MAX_SIZE];
  int count = 0;
  
  byte bpm[RATE_SIZE]; //Array of heart rates
  byte rateSpot = 0;
  long lastBeat = 0; //Time at which the last beat occurred
  //float beatsPerMinute;
  //int beatAvg;

  while (count<MaxReading) {
    while (rateSpot <RATE_SIZE){
      long irValue = particleSensor.getIR();
    
      if (checkForBeat(irValue) == true) {
    
        long delta = millis() - lastBeat;
        lastBeat = millis();
    
        beatsPerMinute = 60 / (delta / 1000.0);
    
        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
          bpm[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array

          rateSpot %= RATE_SIZE; //Wrap variable
    
          //Take average of readings
          beatAvg = 0;
          for (byte i = 0 ; i < rateSpot ; i++)
            beatAvg += bpm[i];
          beatAvg /= rateSpot;
        }
      }
    
      Serial.print("IR=");
      Serial.print(irValue);
      Serial.print(", BPM=");
      Serial.print(beatsPerMinute);
      Serial.print(", Avg BPM=");
      Serial.print(beatAvg);
      beatsPerMin[count] = (byte)beatsPerMinute; //Store this reading in the array
      temp[count] = particleSensor.readTemperatureF();
      count++;

  }

  
//    if (irValue < 50000)
//      Serial.print(" No finger?");
  
    Serial.println();
   

  }
   publishPulseWatson(beatsPerMinute);
}

void publishPulseWatson(int bpm){
  Serial.println("Inside publishPulseWatson");

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:


  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
/*    client.println("Host: www.google.com");
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Connection: close");
    client.println();
    */
    
    Serial.println("BPM: "+ String(bpm));
    client.println("GET /AdiHealthBand/Index.jsp?bandParam=Publish&param1=pulse&param2=" + String(bpm) + " HTTP/1.1");
    client.println("Host: 77b1bcaf.ngrok.io");
    client.println("Connection: close");
    client.println();
    /*
    char startByte = '0';
    while((int)startByte != '1'){
      startByte = Serial.read();
    }*/

  }
  //while (!client.available());

  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (client.connected()) {
    client.flush();
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
  }
  
}

//int readTransmitPulse(){
//  long curTime = millis();
//  int counter=0;
//  Serial.println("In read20Pulse");
//  while(millis() <= curTime+(60000L*30L)){
//    /*if (counter == 20) {
//      counter = 0;
//
//    }*/
//    if (qualitySignal == true) {  //Once I read 10 beats and average it I start Transmitting/ printing the results.
//      Serial.println("BPM: "+ String(BPM));
//      rptBPM[counter]=BPM;
//      counter++;
//      if (counter == 20){
//        for (int i = 0; i < counter; i++) {          
//          publishPulseWatson(rptBPM[i]);
//          delay(3000);
//        }
//        counter=0;
//        Serial.println("Counter =" + String(counter));
//        while(millis() <= curTime+60000L);
//      }
//      
//      qualitySignal = false;
//    } else if (millis() >= (lastTime + 2)) {  //Taking reading every 2 milliseconds
//      readPulse(); // Read the pulse from the sensor every 2 millisecond
//      lastTime = millis();
//    }     
//
//  }
//}



////Function for reading the heartbeat.  
//void readPulse() {
//
//  Signal = analogRead(pulsePin);         
//  sampleCounter += 2;                           
//  int N = sampleCounter - lastBeatTime;  
//  
//  /* 
//  Serial.println("N|samplecounter|lastBeatTime");
//  Serial.print(N);
//  Serial.print("|");
//  Serial.print(sampleCounter);
//  Serial.print("|");
//  Serial.print(lastBeatTime);
//  Serial.print("\n");*/
//  
//  //Find the peak and trough in the next 250 readings.  
//  detectSetHighLow();  //Function for setting the peak and the trough
//
//  if (N > 250) {  
//    if ( (Signal > threshold) && (pulseFound == false) && (N > (IBI / 5) * 3) )
//      pulseDetected();
//      /*     char startByte = '0';
//          while((int)startByte != '1'){
//          startByte = Serial.read();
//      }*/
//  }
//
//  //Once the pulse has been found and processed, reset the variables for the next beat.
//  if (Signal < threshold && pulseFound == true) {  
//    pulseFound = false;
//    amplitude = peakSignal - troughSignal;
//    threshold = amplitude / 2 + troughSignal;  
//    peakSignal = threshold;
//    troughSignal = threshold;
//    /* char startByte = '0';
//     while((int)startByte != '1'){
//       startByte = Serial.read();
//     }*/
//  }
//
//  if (N > 2500) {
//    threshold = 512;
//    peakSignal = 512;
//    troughSignal = 512;
//    lastBeatTime = sampleCounter;
//    firstBeat = true;            
//    secondBeat = true;           
//  }
//
//}
//
//void detectSetHighLow() {
//
//  if (Signal < threshold && N > (IBI / 5) * 3) {
//    if (Signal < troughSignal) {                       
//      troughSignal = Signal;                         
//    }
//  }
//
//  if (Signal > threshold && Signal > peakSignal) {    
//    peakSignal = Signal;                           
//  }                                       
//
//}
//
//void pulseDetected() {
//  pulseFound = true;
//  if (toggleBeat)   {
//    digitalWrite(ledPin,HIGH);
//    toggleBeat=false;
//  }else {
//    digitalWrite(ledPin,LOW);
//    toggleBeat=true;
//  }
//  
//                        
//  IBI = sampleCounter - lastBeatTime;     
//  lastBeatTime = sampleCounter;           
//
//  if (firstBeat) {                       
//    firstBeat = false;                 
//    return;                            
//  }
//  if (secondBeat) {                    
//    secondBeat = false;                
//    for (int i = 0; i <= 9; i++) {   
//      ibiRate[i] = IBI;
//    }
//  }
//
//  word runningTotal = 0;                   
//
//  for (int i = 0; i <= 8; i++) {          
//    ibiRate[i] = ibiRate[i + 1];            
//    runningTotal += ibiRate[i];          
//  }
//
//  ibiRate[9] = IBI;                      
//  runningTotal += ibiRate[9];            
//  runningTotal /= 10;                 
//  BPM = 60000 / runningTotal;         
//  qualitySignal = true;                              
//}
