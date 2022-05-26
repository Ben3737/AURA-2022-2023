// Ben Davis
// 3/30/2022
// This is for an Altimeter and launch detection.
#include <SPI.h>
#include <SD.h>
#define BUFFERSIZE 512
const int chipSelect = 10;
char inputString[101];
String buff= "";
uint8_t inputStringLength = 0;
bool stringComplete = false;  // whether the string is complete
File dataFile;
///// For the IMU /////
#include "ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU


#define SERIAL_PORT Serial
#define WIRE_PORT Wire // Your desired Wire port.      
#define AD0_VAL 1      // The value of the last bit of the I2C address.                \
                       // On the SparkFun 9DoF IMU breakout the default is 1, and when \
                       // the ADR jumper is closed the value becomes 0


ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object
///// For the Altimeter /////


// Include Libraries
#include <TimeLib.h>
#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;

// Define Variables
bool launch;
bool Apygee;
bool Deploy;
bool L;
bool A;
bool D;
int ho;
int depHight = 1;
int hrange = 5;
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


void setup() {
   ///// For the SD card /////
    pinMode(10,OUTPUT);
  // initialize serial:
  Serial.begin(9600);
  // see if the card is present and can be initialized:
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
   return;
  }
  Serial.println("card initialized.");
  dataFile = SD.open("DATA.TXT", FILE_WRITE);
   if( ! dataFile ) {
    int i = millis() + 5000; // 5 seconds
    Serial.println("Card write failed!  Halted...");
      } else {
      Serial.println(F("Ready to accept serial data..."));
    }
    dataFile.close();
 ///// For the IMU /////
 WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);

  bool initialized = false;
  while (!initialized)
  {
    myICM.begin(WIRE_PORT, AD0_VAL);
    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
  ///// For the Altemeter /////
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
// Set full scale ranges for both acc and gyr
  ICM_20948_fss_t myFSS; // This uses a "Full Scale Settings" structure that can contain values for all configurable sensors

  myFSS.a = gpm16; // (ICM_20948_ACCEL_CONFIG_FS_SEL_e)
                  // gpm2
                  // gpm4
                  // gpm8
                  // gpm16

  myFSS.g = dps2000; // (ICM_20948_GYRO_CONFIG_1_FS_SEL_e)
                    // dps250
                    // dps500
                    // dps1000
                    // dps2000

  myICM.setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS);
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    SERIAL_PORT.print(F("setFullScale returned: "));
    SERIAL_PORT.println(myICM.statusString());
  }

  // Set up Digital Low-Pass Filter configuration
  ICM_20948_dlpcfg_t myDLPcfg;    // Similar to FSS, this uses a configuration structure for the desired sensors
  myDLPcfg.a = acc_d473bw_n499bw; // (ICM_20948_ACCEL_CONFIG_DLPCFG_e)
                                  // acc_d246bw_n265bw      - means 3db bandwidth is 246 hz and nyquist bandwidth is 265 hz
                                  // acc_d111bw4_n136bw
                                  // acc_d50bw4_n68bw8
                                  // acc_d23bw9_n34bw4
                                  // acc_d11bw5_n17bw
                                  // acc_d5bw7_n8bw3        - means 3 db bandwidth is 5.7 hz and nyquist bandwidth is 8.3 hz
                                  // acc_d473bw_n499bw

  myDLPcfg.g = gyr_d361bw4_n376bw5; // (ICM_20948_GYRO_CONFIG_1_DLPCFG_e)
                                    // gyr_d196bw6_n229bw8
                                    // gyr_d151bw8_n187bw6
                                    // gyr_d119bw5_n154bw3
                                    // gyr_d51bw2_n73bw3
                                    // gyr_d23bw9_n35bw9
                                    // gyr_d11bw6_n17bw8
                                    // gyr_d5bw7_n8bw9
                                    // gyr_d361bw4_n376bw5

  myICM.setDLPFcfg((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myDLPcfg);
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    SERIAL_PORT.print(F("setDLPcfg returned: "));
    SERIAL_PORT.println(myICM.statusString());
  }

  // Choose whether or not to use DLPF
  // Here we're also showing another way to access the status values, and that it is OK to supply individual sensor masks to these functions
  ICM_20948_Status_e accDLPEnableStat = myICM.enableDLPF(ICM_20948_Internal_Acc, false);
  ICM_20948_Status_e gyrDLPEnableStat = myICM.enableDLPF(ICM_20948_Internal_Gyr, false);
  SERIAL_PORT.print(F("Enable DLPF for Accelerometer returned: "));
  SERIAL_PORT.println(myICM.statusString(accDLPEnableStat));
  SERIAL_PORT.print(F("Enable DLPF for Gyroscope returned: "));
  SERIAL_PORT.println(myICM.statusString(gyrDLPEnableStat));







  
  Serial.begin(9600);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  ho = bmp.readAltitude();
  depHight = ho + 300;
  Serial.println(ho);
}
  
void loop() {
  // check curent height
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");
  Serial.println();
//////////////////////////////////////////////////////////////////////////////////////////////////////  
// Detect launch
  if (!launch){
    RocketStatus = '0';
    if (bmp.readAltitude() > ho+hrange){
        launch = true;
      } 
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Detect Ap
  if (launch){
    if(!Apygee){
      
      if(!A){
        h_old = bmp.readAltitude();
        i = 0;
        A = true;
      }

      h = bmp.readAltitude();
      
      if(h < h_old){
        if(i > 10){
          Apygee = true;
          RocketStatus = 'A';
          }
        i = i + 1;
      }
      if(h > h_old){
        A = false;
      }
      
      
    }  
    
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Detect deployment
  if(Apygee){
    if(!Deploy){
      if (bmp.readAltitude() < depHight){
        D = true;
        Serial.println("reset");
      }

      
      if (D){
        if(j>10){
          Deploy = true;
          RocketStatus = 'D';
          }
       }
       
      j = j + 1;
      
      if (bmp.readAltitude() > depHight){
        D = fFalse;
        j = 0;
      }
    }
  //Serial.print("h = ");
  //Serial.println(bmp.readAltitude());
  //Serial.print("depHeight = ");
  //Serial.println(depHeight);
  //Serial.print("j = ");
  //Serial.println(j);
 }
 
//////////////////////////////////////////////////////////////////////////////////////////////////////
///// For the SD card /////
  //printRawAGMT( myICM.agmt );
  ///// For the IMU /////
  if (myICM.dataReady())
  {
    myICM.getAGMT();         // The values are only updated when you call 'getAGMT'
    printRawAGMT( myICM.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
  }
  else
  {
    Serial.println("Waiting for data");
    delay(500);
  }
  ///// For the Altemeter /////
      // you can get a more precise measurement of altitude
      // if you know the current sea level pressure which will
      // vary with weather and such. If it is 1015 millibars
      // that is equal to 101500 Pascals.
  dataFile = SD.open("INFO.TXT", FILE_WRITE);
  dataFile.print(bmp.readAltitude());
  dataFile.print(",");
  dataFile.println(micros());
  dataFile.close();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions:
void printRawAGMT(ICM_20948_AGMT_t agmt){
  //accelx = ;
  //accely = ;
  //accelz = ;
  //wx = ;
  //wy = ;
  //wz = ;
  //magx = ;
  //magz = ;
  
  dataFile = SD.open("INFO.TXT", FILE_WRITE);
  dataFile.print(agmt.acc.axes.x);
  dataFile.print(",");
  dataFile.print(agmt.acc.axes.y);
  dataFile.print(",");
  dataFile.print(agmt.acc.axes.z);
  dataFile.print(",");
  dataFile.print(agmt.gyr.axes.x);
  dataFile.print(",");
  dataFile.print(agmt.gyr.axes.y);
  dataFile.print(",");
  dataFile.print(agmt.gyr.axes.z);
  dataFile.print(",");
  dataFile.print(agmt.mag.axes.x);
  dataFile.print(", ");
  dataFile.print(agmt.mag.axes.y);
  dataFile.print(", ");
  dataFile.print(agmt.mag.axes.z);
  dataFile.print(", ");
  dataFile.print(agmt.tmp.val);
  dataFile.print("  , ");
  dataFile.print(RocketStatus);
  dataFile.close();
}
