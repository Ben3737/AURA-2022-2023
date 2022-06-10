/* Jacob Birch
   6-9-22 ;)
   Teensy 4.1
   This code will detect launch, apogee, and deployment
   MPL3115A2 Sensor
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
float errorMes = .5; // Error measured by sensor (FROM DATA SHEETS is .05!)
float KG = errorEst / (errorEst + errorMes); //
float unfVals[3];
float fVals[3];
float estimate[3];
float estOLD;
float firstVal;
float secondVal;
float thirdVal;
float oVal;

// int iKG = 1; // was originally 2, I think it's because MATLAB starts at 1
int iKGold = 0;
int inew;
float Alt = 3; // Doesn't matter will get reset
float Alt1;
float getOrigVal;



void setup() {
  Serial.begin(9600);
  while (!Serial);
  if (!baro.begin()) {
    //    Serial.println("Could not find sensor. Check wiring.");
    while (1);
  }

  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  baro.setSeaPressure(1013.25); // hPa

//  oVal = baro.getAltitude();
//  Alt1 = kalman_Func();
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
  // compared to the matlab code, estimate is equivalent to fVals. Size is predetermined, once it exceeds size it resets itself to save memory
  // Also fuck Arduino code, if I forget to write another semi colon Im going to fucking lose it
  unfVals[0] = baro.getAltitude();
  //  delay(200);
  unfVals[1] = baro.getAltitude();

  fVals[0] = unfVals[0];


  int iKG = 1;
  while (iKG <= 2) { //

    KG = errorEst / (errorEst + errorMes);
    //  Estimate(i) = Estimate(i-1) + KG*(unfilteredData(i) - Estimate(i-1));

    fVals[iKG] = fVals[iKG - 1] + KG * (unfVals[iKG] - fVals[iKG - 1]);

    errorEst = (1 - KG) * errorEst;

    // Check if the value changes significantly
    if (abs(unfVals[iKG]) > abs(unfVals[iKG - 1]) + errorMes  || (abs(unfVals[iKG]) < abs(unfVals[iKG - 1]) - errorMes)) {


      errorEst = abs(unfVals[iKG] - unfVals[iKG - 1]);

      // Check to see how if the error is reset too often
      inew = iKG;
      if (inew < iKGold + 1) {
//        Serial.println("Error is too large!");

//        Serial.print("Inew: "); Serial.println(inew);
//        Serial.print("Ikgold: "); Serial.println(iKGold);

      }
      iKGold = inew;

    }


    iKG = iKG + 1;

  }

  firstVal = fVals[1];
  secondVal = fVals[2];
  thirdVal = fVals[3];

  return firstVal; // Can't return more than one val?

}
