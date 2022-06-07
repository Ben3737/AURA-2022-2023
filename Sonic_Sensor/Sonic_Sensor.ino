/*  Jacob Birch
 *  6-6-22
 *  Will detect when sensor has exited rocket
 *  Arduino Nano
 *  Make sure right processor is selected!!
 */



#include <HCSR04.h>

byte echoPin = 2; // attach pin D2 Arduino to pin Echo of HC-SR04 -> probably need to change these
byte trigPin = 3; //attach pin D3 Arduino to pin Trig of HC-SR04

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

int leavDist = 400; // variable to determine if it has left rocket. In centimeters)
bool leftRocket = true; // variable for if statement

// setup
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04");
}

// loop
void loop() {

  if(leftRocket == false){ // Only check if it has left the rocket if it has not left the rocket
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  
  // The speed of sound is 0.034 centimeter per microsecond, it is measured in microseconds so results in centimeters
  
  // Displays the distance on the Serial Monitor

  // --- can comment this out when finished, just for testing
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  // ---

  if(distance >= leavDist){ // if distance measured is greater or equal to leavDist then it must have left the rocket
    leftRocket = true;
    }
  } 
  
  // ------- If it has left the rocket, it will loop the section below

  if(leftRocket == true){
    
  }


}
