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
bool apogee;
bool deploy;
bool L;
bool A;
bool D;
float ho; // h original
int depHeight = 1;
int hrange = 1; // CHANGE LATER!!
int trange = 1;
elapsedMillis t;
elapsedMillis to;
float h_old; // for detecting apogee
int h;
int i;
int j;
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

float testAlt;
int testBound = 10; // meters



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
  testAlt = baro.getAltitude();
  ho = kalman_Func();

  if (abs(ho - testAlt) >= testBound) {
    // This checks and makes sure that the unfiltered data and filtered data are somewhat close together (within testBound meters)
    // If not, it will print an error message.
    Serial.println("Original height does not match kalman filtered original height. Check code.");
  }


  //  Serial.println(ho);


  //  oVal = baro.getAltitude();
  //  Alt1 = kalman_Func();
}

void loop() {

  //Serial.print("Altitude = ");

  //Serial.print("Starting kal func"); // for debugging

  Alt = kalman_Func();
  Serial.print("Altitude: "); Serial.print(Alt); Serial.println(" m");


  //Serial.print("Ending kal func"); // for debugging

  // CHECK LAUNCH

  if (!launch) {
    RocketStatus = '0';
    if (Alt > ho + hrange) { // if current altitude is greater than original altitude + selected range (in meters)
      launch = true;
      Serial.println("Launch detected");
    }
  }

  // CHECK APOGEE
  if (launch) {
    if (!apogee) {

      if (!A) {

        h_old = Alt;
        i = 0;
        A = true;
      }

      h = Alt;

      if (h < h_old) { // if current height is less than old height after 10 iterations
        if (i > 10) {
          apogee = true;
          RocketStatus = 'A';
          Serial.println("Apogee reached");
        }
        i = i + 1;
      }
      if (h > h_old) {
        A = false;

      }
    }
  } // End of apogee phase

  if (apogee) {
    if (!deploy) { // if apogee and has not deployed yet
      if (Alt < depHeight) { // if altitude is less than deploy height (remember it is going down at this point)
        D = true;
        Serial.println("Reset");
      }

      if (D) {
        if (j > 10) {
          deploy = true;
          RocketStatus = 'D';
          Serial.println("Deployment started");

        }
      }

      j = j + 1;

      if (Alt > depHeight) {
        D = false;
        j = 0;
      }

    }


  } // end of deployment phase

} // End of void loop


// KALMAN FILTER FUNCTION!!

float kalman_Func() {



  // 0.05 is error in measurement on sensor
  // compared to the matlab code, estimate is equivalent to fVals. Size is predetermined, once it exceeds size it resets itself to save memory
  // Also fuck Arduino code, if I forget to write another semi colon Im going to fucking lose it

  unfVals[0] = baro.getAltitude();
  unfVals[1] = baro.getAltitude();
  unfVals[2] = baro.getAltitude();



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

  firstVal = fVals[0];
  secondVal = fVals[1];
  thirdVal = fVals[2];

  return firstVal; // Can't return more than one val :(
  // TRY FIRST VAL, IF IT DOES NOT WORK USE SECOND VAL WHEN TESTING!!

}
