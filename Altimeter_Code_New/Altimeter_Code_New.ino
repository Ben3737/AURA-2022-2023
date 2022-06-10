/* Jacob Birch
 * 6-9-22 ;)
 * Teensy 4.1
 * This code will detect launch, apogee, and deployment
 * MPL3115A2 Sensor
 */

#include <Adafruit_MPL3115A2.h>

Adafruit_MPL3115A2 baro;

// Variables

// Define Variables
bool launch;
bool Apygee;
bool Deploy;
bool L;
bool A;
bool D;
int ho;
int depHight = 1;
int hrange = 1;
int trange = 1;
elapsedMillis t;
elapsedMillis to;
int h_old;
int h;
int i;
int j;
int depHeight;
char RocketStatus;

int waiting;

// kalman
float errorEst = 1; // Error estimated
float errorMes = 0.05; // Error measured by sensor (FROM DATA SHEETS)
float KG = errorEst / (errorEst + errorMes); //
float unfVal_1;
float unfVal_2;
float fVals[2];

// int iKG = 1; // was originally 2, I think it's because MATLAB starts at 1
int iKGold = 0;
int inew;
float Alt = 3; // Doesn't matter will get reset



void setup() {
  Serial.begin(9600);
  while(!Serial);
  if (!baro.begin()) {
//    Serial.println("Could not find sensor. Check wiring.");
    while(1);
  }

  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  baro.setSeaPressure(1013.25); // hPa
}

void loop() {

//Serial.print("Altitude = ");

//Serial.print("Starting kal func"); // for debugging

Alt = kalman_Func();

//Serial.print("Ending kal func"); // for debugging

Serial.println(Alt);
  
}


// KALMAN FILTER FUNCTION!!

float kalman_Func() {




// 0.05 is error in measurement on sensor
// compared to the matlab code, estimate is equivalent to unfVals. Size is predetermined, once it exceeds size it resets itself to save memory
// Also fuck Arduino code, if I forget to write another semi colon Im going to fucking lose it
unfVal_1 = baro.getAltitude();
delay(200);
unfVal_2 = baro.getAltitude();




// Serial.println("Got Values");
for(int iKG = 0; iKG==1; iKG++){

    KG = errorEst / (errorEst + errorMes);
    fVals[iKG] = unfVal_1 + ( KG*(unfVal_2 - unfVal_1) );
      // if there is an error in the filtering, it is likely in the line above. I'm not sure what estimate actually is mathematically
    errorEst = (1-KG)*errorEst;

    // Check if the value changes significantly
  if(abs(unfVal_2) > abs(unfVal_1) + errorMes  || (abs(unfVal_2) < abs(unfVal_1) - errorMes)) {

      
        errorEst = abs(unfVal_2 - unfVal_1);

        // Check to see how if the error is reset too often  
        inew = iKG;
        if(inew < iKGold + 50){
          Serial.println("Error is too large!");
        }
        iKGold = inew;

    }
}

// Serial.println("Finished if loop");
  
// Serial.println("Finished while loop");

// Above loop should be done, resetting iKG


/*
  while(iKG<=2){

  Serial.println(fVals[iKG]);

  iKG = iKG + 1;
} // This prints the data

*/

return fVals[1];

}
