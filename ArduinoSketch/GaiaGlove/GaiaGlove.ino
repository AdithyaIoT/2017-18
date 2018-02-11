
#include <SPI.h>
#include <WiFi101.h>

int rptBPM[20];
char ssid[] = "AmsKumsHaus";        // your network SSID (name)
char pass[] = "aryashakthi";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
char server[] = " http://77b1bcaf.ngrok.io";    // NGROK server where the JSP for calling the python program for passing the BPM to WATSON is present.
WiFiClient client;

int pushPin = 1; //Push Button Pin
int ledPin = 6; // LED PIN where the pulse is shown
boolean buttonPressed = false;

//Variables to read the pulse from the pulse sensor
int pulsePin = 0; //Pin for reading heart beat.            
//int ledPin = 6;  //Pin for showing the push button status
unsigned long sampleCounter = 0; 
unsigned long lastBeatTime = 0;  
unsigned long lastTime = 0, N;

int ibiRate[10]; //Storing InterBeat Interval for 10 readings       
int BPM = 0; // Beats per minute
int IBI = 0; //Storing one Inter Beat Interval in this variable
int peakSignal = 512;  // Midpoint of the range the pulse sensor signal.  The range is from 0 - 1023
int troughSignal = 512; //Set the troubh of the heart beat to be the midpoint
int threshold = 512;  //Set the threshold to be the midpoint
int amplitude = 100;  //Set the amplitude of the heart beat to be 100                   
int Signal;
boolean pulseFound = false;
boolean firstBeat = true;          
boolean secondBeat = true;
boolean qualitySignal = false;    // Only after the first 10 readings of the BPM has been found then start transmitting the values.
boolean toggleBeat=true;  // To make the led pin beat with the heart beat.


void setup() {
  Serial.begin(9600);
  pinMode(pushPin, INPUT_PULLUP);
  pinMode(ledPin,OUTPUT);         // LED pin that will blink to your heartbeat!
  pinMode(pulsePin,INPUT);         // Analog pin from which we will read your heartbeat!
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


  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
   int buttonValue = digitalRead(pushPin);
   if (buttonValue == LOW){
      // If button pushed, turn LED on
      buttonPressed = true;
      digitalWrite(ledPin,HIGH);
      Serial.println("Button Pressed");
   } 
   else {
      // Otherwise, turn the LED off
      digitalWrite(ledPin, LOW);
      Serial.println("Button is in Released State");
      buttonPressed=false;
   }

   if (buttonPressed){
    readTransmitPulse();
   }

}

int readTransmitPulse(){
  long curTime = millis();
  int counter=0;
  Serial.println("In read20Pulse");
  while(millis() <= curTime+(60000L*30L)){
    /*if (counter == 20) {
      counter = 0;

    }*/
    if (qualitySignal == true) {  //Once I read 10 beats and average it I start Transmitting/ printing the results.
      Serial.println("BPM: "+ String(BPM));
      rptBPM[counter]=BPM;
      counter++;
      if (counter == 20){
        for (int i = 0; i < counter; i++) {          
          publishPulseWatson(rptBPM[i]);
          delay(3000);
        }
        counter=0;
        Serial.println("Counter =" + String(counter));
        while(millis() <= curTime+60000L);
      }
      
      qualitySignal = false;
    } else if (millis() >= (lastTime + 2)) {  //Taking reading every 2 milliseconds
      readPulse(); // Read the pulse from the sensor every 2 millisecond
      lastTime = millis();
    }     

  }
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

//Function for reading the heartbeat.  
void readPulse() {

  Signal = analogRead(pulsePin);         
  sampleCounter += 2;                           
  int N = sampleCounter - lastBeatTime;  
  
  /* 
  Serial.println("N|samplecounter|lastBeatTime");
  Serial.print(N);
  Serial.print("|");
  Serial.print(sampleCounter);
  Serial.print("|");
  Serial.print(lastBeatTime);
  Serial.print("\n");*/
  
  //Find the peak and trough in the next 250 readings.  
  detectSetHighLow();  //Function for setting the peak and the trough

  if (N > 250) {  
    if ( (Signal > threshold) && (pulseFound == false) && (N > (IBI / 5) * 3) )
      pulseDetected();
      /*     char startByte = '0';
          while((int)startByte != '1'){
          startByte = Serial.read();
      }*/
  }

  //Once the pulse has been found and processed, reset the variables for the next beat.
  if (Signal < threshold && pulseFound == true) {  
    pulseFound = false;
    amplitude = peakSignal - troughSignal;
    threshold = amplitude / 2 + troughSignal;  
    peakSignal = threshold;
    troughSignal = threshold;
    /* char startByte = '0';
     while((int)startByte != '1'){
       startByte = Serial.read();
     }*/
  }

  if (N > 2500) {
    threshold = 512;
    peakSignal = 512;
    troughSignal = 512;
    lastBeatTime = sampleCounter;
    firstBeat = true;            
    secondBeat = true;           
  }

}

void detectSetHighLow() {

  if (Signal < threshold && N > (IBI / 5) * 3) {
    if (Signal < troughSignal) {                       
      troughSignal = Signal;                         
    }
  }

  if (Signal > threshold && Signal > peakSignal) {    
    peakSignal = Signal;                           
  }                                       

}

void pulseDetected() {
  pulseFound = true;
  if (toggleBeat)   {
    digitalWrite(ledPin,HIGH);
    toggleBeat=false;
  }else {
    digitalWrite(ledPin,LOW);
    toggleBeat=true;
  }
  
                        
  IBI = sampleCounter - lastBeatTime;     
  lastBeatTime = sampleCounter;           

  if (firstBeat) {                       
    firstBeat = false;                 
    return;                            
  }
  if (secondBeat) {                    
    secondBeat = false;                
    for (int i = 0; i <= 9; i++) {   
      ibiRate[i] = IBI;
    }
  }

  word runningTotal = 0;                   

  for (int i = 0; i <= 8; i++) {          
    ibiRate[i] = ibiRate[i + 1];            
    runningTotal += ibiRate[i];          
  }

  ibiRate[9] = IBI;                      
  runningTotal += ibiRate[9];            
  runningTotal /= 10;                 
  BPM = 60000 / runningTotal;         
  qualitySignal = true;                              
}
