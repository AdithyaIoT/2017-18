
int buttonPin = 8;
int LED = 6;

void setup() {
   // Define pin #12 as input and activate the internal pull-up resistor
   pinMode(buttonPin, INPUT_PULLUP);
   // Define pin #13 as output, for the LED
   pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop(){
   // Read the value of the input. It can either be 1 or 0
   int buttonValue = digitalRead(buttonPin);
   if (buttonValue == LOW){
      // If button pushed, turn LED on
      digitalWrite(LED,LOW);
      Serial.println("Ari is a bugger");
   } 
   else {
      // Otherwise, turn the LED off
      digitalWrite(LED, HIGH);
      Serial.println("Shakthi is a bugger");
   }
}
