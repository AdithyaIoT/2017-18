//FOR DPS 310 PRESSURE SENSOR
#include <Dps310.h>

// Dps310 Opject
Dps310 Dps310PressureSensor = Dps310();

const float sea_press = 1013.25; // DECLARING SEA LEVEL PRSSURE AS CONSTANT
int alttitude = 0;         // the sensor value
int minimumalttitude = 100000;        // minimum sensor value (changes after calibration)
int maximumalttitude = 0;           // maximum sensor value(changes after calibration)



void setup(){
  Serial.begin(9600);             
   // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
   // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
   while (!Serial);
  Wire.begin();


  //Call begin to initialize Dps310
  //The parameter 0x76 is the bus address. The default address is 0x77 and does not need to be given.
  Dps310PressureSensor.begin(Wire,0x76);

  //Dps310.begin(Wire, 0x76);
  //Use the commented line below instead of the one above to use the default I2C address.
  //if you are using the Pressure 3 click Board, you need 0x76
  //Dps310.begin(&Wire);
  
  // IMPORTANT NOTE
  //If you face the issue that the DPS310 indicates a temperature around 60 C although it should be around 20 C (room temperature), you might have got an IC with a fuse bit problem
  //Call the following function directly after begin() to resolve this issue (needs only be called once after startup)
  //Dps310.correctTemp();

  Serial.println("Init complete!");
  calibration(); //calling calibration function beacause we cant fix a altitude because it varies every were
                   //so we calibrate the maximum altitude and minimum altitude for 3 seconds

}


//  Where the Magic Happens
void loop(){
   // Blynk.run();

  long int temperature;
  long int pressure;
  int oversampling = 7;
  int ret;
  Serial.println();

  //lets the Dps310 perform a Single temperature measurement with the last (or standard) configuration
  //The result will be written to the paramerter temperature
  //ret = Dps310.measureTempOnce(temperature);
  //the commented line below does exactly the same as the one above, but you can also config the precision
  //oversampling can be a value from 0 to 7
  //the Dps 310 will perform 2^oversampling internal temperature measurements and combine them to one result with higher precision
  //measurements with higher precision take more time, consult datasheet for more information
  ret = Dps310PressureSensor.measureTempOnce(temperature, oversampling);
  //ret = Dps310PressureSensor.measureTempOnce(temperature);
  if (ret != 0)
  {
    //Something went wrong.
    //Look at the library code for more information about return codes
    Serial.print("FAIL! ret = ");
    Serial.println(ret);
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" degrees of Celsius");
  }
 
  //Pressure measurement behaves like temperature measurement
  ret = Dps310PressureSensor.measurePressureOnce(pressure);
  //ret = Dps310PressureSensor.measurePressureOnce(pressure, oversampling);
  if (ret != 0)
  {
    //Something went wrong.
    //Look at the library code for more information about return codes
    Serial.print("FAIL! ret = ");
    Serial.println(ret);
  }
  else
  {
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" Pascal");
  }

  float getaltitude=(((pow((sea_press/(pressure)), 1/5.257) - 1.0)*(temperature +273.15)))/0.0065; //hypsometric formula to covert presure and temperature of a region to altitude
 
 Serial.print(getaltitude);  //print the obtained altitude
 
    if(alttitude < (maximumalttitude - .5)) //if calibrated MAXIMUM alttitude is 3M. IF a SUDDEN FALL OCCURS WHCIH IS LESS THAN 2.5M (maximumalttitude - .5)  THEN WE CAN ASSUME A FALL OCCURS
    {                                       // we take a diiference of .5 meter decrement as falling situation. you can change as per your need
                                            // if you need to send the BPM rate coreesponding to this time you can add it in the email option
     Serial.println("alttitude drop");
//  Blynk.email("your_email@mail.com", "subject :alttitude drop occurs ", "your randma/grandpa 's mAY fall down and required assistance"); // provide your email id
    }
    
    if(alttitude  > (maximumalttitude + 1)) //if calibrated MAXIMUM alttitude is 3M IF a rise occurs more than 1 METER  which means cmlimbing up 
   
    { 

      Serial.println("alttitude rise");
      //  Blynk.email("your_email@mail.com", "subject :alttitude rise occurs ", "your randma/grandpa 's climbing up a ladder or something "); // provide your email id
   
    }

   
    
} 

void calibration() //we canot fix a height because its changes with location so we calibrate the HEIGHT for 3 seconds
{
   while (millis() < 3000) // calibration time you can change it as per your need
   {

 long int temperature;
  long int pressure;
  int oversampling = 7;
  int ret;
  //ret = Dps310PressureSensor.measureTempOnce(temperature, oversampling);
  ret = Dps310PressureSensor.measureTempOnce(temperature); 
  //ret = Dps310PressureSensor.measurePressureOnce(pressure, oversampling);
  ret = Dps310PressureSensor.measurePressureOnce(pressure);

    float getaltitude=(((pow((sea_press/(pressure/10)), 1/5.257) - 1.0)*(temperature +273.15)))/0.0065; //hypsometric formula
    alttitude = getaltitude;

    // record the maximum altitude value
    if (alttitude > maximumalttitude) {
      maximumalttitude = alttitude;
    }

    // record the minimum altitue value
    if (alttitude < minimumalttitude) {
      minimumalttitude = alttitude;
    }
    }   Serial.println("");
    Serial.print(maximumalttitude);
    Serial.print(" -- maximumalttitude");
     Serial.println("");
      Serial.print(minimumalttitude);
    Serial.print("  --minimumalttitude");
    Serial.println("");
    
}


