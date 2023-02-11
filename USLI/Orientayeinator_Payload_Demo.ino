//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ben Davis
// 2-8-23
// This is the code used for the second payload/vehicle demonstration launch for the 22-23 USLI competition.  During the first 
// launch, Micropython and a Pi Pico was used to handle these tasks, but due to time, space, and parts shortages, the 
// design was changed to use a Teensy 4.1 instead. 
// In general, an IMU and a barometer are used to determine the orientation of the rocket and the altitude of the rocket, 
// and that information is used to determine when it is safe and necessary to deploy.  During the deployment, two DC motors are
// used to level and extend the system.  The Teensy is very powerful, but once the readings from the barometer are taken in 
// addition to the IMU, the filtering algorithm slows down signifigantly and takes too long to converge.
// The filter chosen is a Madgwick filter.  This filter was chosen because it does not have angle singularities, it is 
// computationaly efficient, and it does not require a model of the system in order to opperate.  Information about the 
// filter is widley avalible, and in this implementation the arduino library is used.  Additionaly the Madgwick filter is
// very resistant to drifting and has not drifted in any implementation I have seen it used.

// There are many times in this program that the use of the "Delay" command would have aided in timing.  This command is not used
// because it would hold up the program and prevent the controls from working correctly.  In order for the program to function without
// the use of delays, counters are used to swithc things on and off based on how many times the program has looped.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Import Libraries
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#include <Adafruit_MPU6050.h>
char report[80];                // I don't remember what this does :)
#include <Wire.h>               // This is used for interfacing with various sensors and actuators
#include <Adafruit_BMP085.h>    // This library allows for interfacing with the BMP180 (and the BMP085)
#include <Adafruit_MPU6050.h>   // This library allows for interfacing with the MPU6050
#include "MadgwickAHRS.h"       // This library is the filter used to get the pitch, roll, and heading
#include <SD.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create sensor objects
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This creates the BMP object
Adafruit_BMP085 bmp; 
// This creates the MPU object
Adafruit_MPU6050 mpu;

File dataFile;
// This initiates the Madgwick filter
Madgwick filter;  
// This sets the expected update rate for the Madgwick filter
const float sensorRate = 260; // Hz

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare Variables
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All altitude data is in meters
// All angle data is in degrees
float radToDeg = 180.0/3.1415;  // This variable is used for unit conversion
float heading;                  // This variable stores the "heading" from the Madgwick filter
float pitch;                    // This variable stores the "pitch" from the Madgwick filter
float roll;                     // This variable stores the "roll" from the Madgwick filter
float alt;                      // This variable stores the altitude from the BMP180
int count = 0;                  // This variable is used for timming altimeter readings to aid filter convergance
int baroBuffer = 200;           // This variable is used to set the ammount of loops before updating the barometer data
bool LANDED = false;;            // This is used during the launch detection section of the script
bool LAUNCHED = false;          // This is used during the launch detection section of the script
bool APOGEE = false;            // This is used during the launch detection section of the script
bool postFlight = false;        // This is used to indicate if the payload has activated the Pi, MMORPG and RAPS yet
float oldalt = 0;               // This stores the previous altitude reading
float initialalt = 0;           // This stores the initial altitude of the system
bool altitudefound = false;     // This indicates if an initial altitude has been found
int launchVerification = 0;     // This variable is used to verify that the rocket has launched
int apogeeVerification = 0;     // This variable is used to verify that the rocket has apogee
int landingVerification = 0;    // This variable is used to verify that the rocket has landed
float takeoffMargin = 1.0;      // This is used to create an altitude buffer to prevent accidental launch detection
float landingMargin = 10.0;      // This is used to create an altitude buffer to prevent accidental landing detection
int badDataCheck = 210;                             // This is used as a buffer to make sure data variation does not trigger an error
                                                    // This needs to be larger that the variable "baroBuffer" otherwise it is pointless
int timeToTurn = 10.0;                                 // This is the ammount of time required for the payload to spin 360 degrees
int timePerDegree = (timeToTurn*1000.0)/360.0;               // This is the ammount of time required for the payload to spin 1 degree
int buzzerCount = 0;                                // This is used to time the buzzer
bool buzzerState = false;                           // This indicates if the buzzer is on or off

// For the Teensy 4.1 board, the accessable pins are:
// 1    MMORPG DC motor A
int MMORPGa = 1;
// 2    MMORPG DC motor B
int MMORPGb = 2;
// 3    RAPS DC motor A
int RAPSa = 3;
// 4    RAPS DC motor B
int RAPSb = 4;
// 5    Pi Power A
int Pia = 5;
// 6    Pi Power B
int Pib = 6;
// 7    Buzzer Pin
int BuzzerPin = 10;
// 8
// 9
// 10

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup Function
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // turn on the built in LED
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  // Start serial debug connection
  Serial.begin(9600);
  // Call the function that sets up the sensors.
  setUpSensor();

  // Get the starting altitude of the system
  while(!altitudefound){
    // get a good reading for the initial altitude by taking an average of several readings
    int i = 0;
    while(i < 10){
      getheight();
      initialalt = initialalt + alt;
      i = i + 1;
      // prevent duplicate readings by waiting a short interval
      delay(0.1);
     }
     initialalt = initialalt/10.0;
     Serial.print("The starting height is: ");
     Serial.println(initialalt);
     altitudefound = true;
     delay(1);
   }   

   //sdSetUp();
   // Attach the pins for the motors and Pi
   pinMode(BuzzerPin, OUTPUT);
   pinMode(MMORPGa, OUTPUT);
   pinMode(MMORPGb, OUTPUT);
   pinMode(RAPSa, OUTPUT);
   pinMode(RAPSb, OUTPUT);
   pinMode(Pia, OUTPUT);
   pinMode(Pib, OUTPUT);
   
   // set every pin output to low to prevent errors
   noTone(BuzzerPin);
   digitalWrite(MMORPGa, LOW);
   digitalWrite(MMORPGb, LOW);
   digitalWrite(RAPSa, LOW);
   digitalWrite(RAPSb, LOW);
   digitalWrite(Pia, LOW);
   digitalWrite(Pib, LOW);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Loop Function
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Call the function to update the anlges and 
  getangles();

  // If the barometer is read to often, the filter takes too long to converge
  // This "if" acts as a delay between updates
  count = count + 1;
  if(count == baroBuffer){
    // Reset the interval
    count = 0;
    // Get the barometer's reading
    oldalt = alt;
    getheight(); 
  }
  
  // See if the rocket is sitting, flying, or landed
  if(!LANDED){
    Serial.println("Not Landed");
    
    // Beep the buzzer
    annoyingBuzzer();

    // Check if the vehicle has gotten above a set altitude
    //==============================================
    if(!LAUNCHED){
      Serial.println("Not Launched");
      if(alt > (initialalt + takeoffMargin)){
        launchVerification = launchVerification + 1;
        if(launchVerification >= badDataCheck){
          LAUNCHED = true;
          Serial.println("LAUNCH HAPPENED");
        } 
      }
      if( alt < (initialalt + takeoffMargin)){
        launchVerification = 0;
      }
      
    }
    //==============================================
    if((LAUNCHED) && (!APOGEE)){
      Serial.println("Launched, not at Apogee");
      if(oldalt > alt){
        apogeeVerification = apogeeVerification + 1;
        if(apogeeVerification >= badDataCheck){
          APOGEE = true;
          Serial.println("APOGEE HAPPENED");
        } 
      }
      if(oldalt < alt){
        apogeeVerification = 0;
      }
      
    }
    //==============================================
    if((LAUNCHED) && (APOGEE)){
      Serial.println("Launched, Apogee");
      if(alt < (initialalt + landingMargin)){
        landingVerification = landingVerification + 1;
        if(landingVerification >= badDataCheck){
          LANDED = true;
          Serial.println("LANDING HAPPENED");
        } 
      }
      if(oldalt < alt){
        landingVerification = 0;
      }
      
    }
    //==============================================

  }

  if((LANDED) && (!postFlight)){
    Serial.println("Landed!");
    // Turn off the buzzer
    noTone(BuzzerPin);
    
    // Level using MMORPG and extend RAPS
    MMORPG();
    RAPS();
    piPower();
    // Supply power to the Raspberry Pi
    
    
    postFlight = true;
  }

  
  // Call a function to print to the seral monitor
  printData(); 
  // Call a function to print the data to the SD card
  //writeToSD();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function writes data collected to the SD card
void writeToSD(){
  dataFile = SD.open("DataFile.txt", FILE_WRITE);
  dataFile.print(millis());
  dataFile.print(' ');
  dataFile.print(heading);
  dataFile.print(' ');
  dataFile.print(pitch);
  dataFile.print(' ');
  dataFile.print(roll);
  dataFile.print(' ');
  dataFile.println(alt);
  dataFile.close();
  
}
// This function initializes the SD card
void sdSetUp(){
  const int chipSelect = BUILTIN_SDCARD;    // This is the built in SD card's "Chip Select" pin for SPI communication

  // If the SD card fails to initialize, print errors and hold the program
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset or reopen this serial monitor after fixing your issue!");
    while (true);
  }
  Serial.println("initialization done.");
  dataFile = SD.open("DataFile.txt", FILE_WRITE);
  dataFile.close();
  
}


// This function writes data collected to the serial monitor
void printData(){
  Serial.print(heading);
  Serial.print(' ');
  Serial.print(pitch);
  Serial.print(' ');
  Serial.print(roll);
  Serial.print(' ');
  Serial.println(alt);
}



// send the imu data to the filter and update angles
void getangles(){
  // It is worth noting that the filter will converge to the inertial frame.  This
  // means that the initial position is not set as <0,0,0>, but rather its true orientation 
  // of the IMU relative to gravity.
  // call the sensors to update the data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // update the filter, which computes orientation:
  filter.updateIMU((float(g.gyro.x)*radToDeg), (float(g.gyro.y)*radToDeg), (float(g.gyro.z)*radToDeg), float(a.acceleration.x), float(a.acceleration.y), float(a.acceleration.z));

  // get the parameters from the filter
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();
}

// Get the data from the Barometer
void getheight(){
  alt = bmp.readAltitude();
}



// This function takes care of the setup for the BMP and MPU
void setUpSensor(){
  Serial.println("Adafruit MPU6050 test!");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

  ///// for altimeter /////
  //Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}

void RAPS(){
  
  digitalWrite(RAPSa, LOW);
  digitalWrite(RAPSb, HIGH);
  delay(5000);
  digitalWrite(RAPSa, LOW);
  digitalWrite(RAPSb, LOW);
  Serial.println("RAPS deployed");
  
}

void MMORPG(){
  timeToTurn = int(abs(pitch*timePerDegree));
  Serial.println(timeToTurn);
  delay(2000);
  
  
  if(pitch > 0){
    digitalWrite(MMORPGa, HIGH);
    digitalWrite(MMORPGb, LOW);
    delay(timeToTurn);
    digitalWrite(MMORPGa, LOW);
    digitalWrite(MMORPGb, LOW);
  }

  if(pitch < 0){
    digitalWrite(MMORPGa, LOW);
    digitalWrite(MMORPGb, HIGH);
    delay(timeToTurn);
    digitalWrite(MMORPGa, LOW);
    digitalWrite(MMORPGb, LOW);
  }
  
  Serial.println("MMORPG done!");
  
}

void piPower(){
  digitalWrite(Pia, LOW);
  digitalWrite(Pib, HIGH);
  Serial.println("Pi Has Received Power");
  while(1){
    
  }
}

void annoyingBuzzer(){
  if(buzzerState == false){
      buzzerCount = buzzerCount + 1;
      if (buzzerCount >= 100){
        buzzerState = true;
        buzzerCount = 0;
        tone(BuzzerPin, 85);
        Serial.println("Buzzer");
      }
    }

    if(buzzerState == true){
      buzzerCount = buzzerCount + 1;
      if (buzzerCount >= 100){
        buzzerState = false;
        buzzerCount = 0;
        noTone(BuzzerPin);
      }
    }
}
