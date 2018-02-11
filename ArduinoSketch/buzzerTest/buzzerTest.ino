//The following function plays an alert sound in the piezo buzzer
void playSound() {
  int i, duration;
  const int BUZZERPIN=9;
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
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 while(!Serial);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  playSound();
  delay(6000000);

}
