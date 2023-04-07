#========================================================================
# Ben  Davis
# 2/1/23
# This is the code for the raspberry pi zero on board the payload during the
# payload demo launch.  It is designed to perform each of the commands required
# for the NASA competetion launch later this year.

# All of the code for the cameras can be taken from this website:
# https://projects.raspberrypi.org/en/projects/getting-started-with-picamera/7

# All of the code for the stepper motor can be taken from this website:
# https://learn.adafruit.com/adafruits-raspberry-pi-lesson-10-stepper-motors/software
 
# All of the code for the servo can be taken from this website:

# Learn more about the Madgwick AHRS here:
# https://ahrs.readthedocs.io/en/latest/filters/madgwick.html

# All of the code for interfacing with the barometer can be found here:

# All of the code for interfacing with the MPU6050 can be found here:


 
# for this program to start when the pi is turned on, it is necessary to add it to the startup procedures

#========================================================================
# import files
#========================================================================
import picamera # This is used to interface with the camera
import time         # This is used for "sleep" functions and other utilities involving time
import digitalio    # This is used to interface with the motors, and servos
import machine  # This is used to for board utilities and communicating with sensors
from bmp280 import BMP280
#========================================================================
# Create camera objects
#========================================================================
camera = PiCamera() # this creates an object required to interface with the camera library

camera.resolution = (1920, 1080) # this deffines the quality of the images


#========================================================================
# Create DC motor objects
#========================================================================
# Define the outputs that are dedicated to MMORPG
MMORPGa = Pin(14, Pin.OUT) 
MMORPGb = Pin(15, Pin.OUT)
# Define the outputs that are dedicated to RAPS
RAPSa = Pin(14, Pin.OUT)
RAPSb = Pin(15, Pin.OUT)
# lock MMORPG and RAPS in place by setting all outputs to "HIGH"
MMORPGa.high()
MMORPGb.high()
RAPSa.high()
RAPSb.high()


#========================================================================
# Create stepper motor objects
#========================================================================
# Stepper motors are complex, if you care how they actually work, I sugest reading the artical linked in
# the headder
enable_pin = digitalio.DigitalInOut(board.D18)
coil_A_1_pin = digitalio.DigitalInOut(board.D4)
coil_A_2_pin = digitalio.DigitalInOut(board.D17)
coil_B_1_pin = digitalio.DigitalInOut(board.D23)
coil_B_2_pin = digitalio.DigitalInOut(board.D24)
enable_pin.direction = digitalio.Direction.OUTPUT
coil_A_1_pin.direction = digitalio.Direction.OUTPUT
coil_A_2_pin.direction = digitalio.Direction.OUTPUT
coil_B_1_pin.direction = digitalio.Direction.OUTPUT
coil_B_2_pin.direction = digitalio.Direction.OUTPUT
enable_pin.value = True

    
#========================================================================
# Determine the state of the payload using avalible sensor data:
#========================================================================
Launched = False
Apogee = False
Landed = False
count = 0
startHeight = 

while not Landed:
    while not Launched:
        
        if (pitch < 45) and (altitude > startHeight + 500) and (roll < 45):
            count += 1
        if (pitch > 45) or (altitude < startHeight + 500) or (roll > 45):
            count = 0
        if (count > 10):
            Launched = True
    if Launched:
        count = 0
        while not Apogee:
            altitude = 
            verticalSpeed = altitude - oldAltitude
            oldAltitude = altitude
            if (verticalSpeed < 0):
                count += 1
            if (verticalSpeed > 0):
                count = 0
            if (count > 10):
                Apogee = True
        count = 0
        if not Apogee:
            altitude = 
            verticalSpeed = altitude - oldAltitude
            oldAltitude = altitude
            if (verticalSpeed < 10) or (verticalSpeed > -10):
                count += 1
            if (verticalSpeed > 10) or (verticalSpeed < -10):
                count = 0
            if (count > 10):
                Apogee = True
        if Apogee:
            break


try:
       from smbus2 import SMBus
except ImportError:
       from smbus import SMBus

#========================================================================
# Task 1: A1—Turn camera 60º to the right
#========================================================================
# This task is done using a stepper motor
backward(0.1, 60)
takepicture()


#========================================================================
# Task 2: B2—Turn camera 60º to the left
#========================================================================
# This task is done using a stepper motor
forward(0.1, 60)
takepicture()

#========================================================================
# Task 3: C3—Take picture
#========================================================================
# This task is redundent and it will be performed after every other task

#========================================================================
# Task 4: D4—Change camera mode from color to grayscale
#========================================================================
camera.color_effects = (128, 128)
takepicture()

#========================================================================
# Task 5: E5—Change camera mode back from grayscale to color
#========================================================================
camera.image_effect = 'none'
takepicture()

#========================================================================
# Task 6: F6—Rotate image 180º (upside down). camera.rotation = 180
#========================================================================
camera.rotation = 180
takepicture()
camera.rotation = 0

#========================================================================
# Task 7: G7—Special effects filter (Apply any filter or image distortion you want and
# state what filter or distortion was used).
#========================================================================
camera.image_effect = 'colorswap'
takepicture()

#========================================================================
# Task 9: H8—Remove all filters.
#========================================================================
camera.image_effect = 'none'
takepicture()


#========================================================================
# Functions:
#========================================================================

def takepicture():
    # This is a function that uses the "picamera" library to take and timestamp an image
    # for the camera to "wake up" and set the correct light level, the library says to sleep for at least
    # two seconds before taking a picture
    camera.start_preview() 
    sleep(2)
    # these two lines use the "time" library to get the date and time and then annotate the image with that information
    When = str(time.localtime()[3]) + '_' + str(time.localtime()[4]) + '_' + str(time.localtime()[5])+ '_ ' +str(time.localtime()[2]) + '_' +str(time.localtime()[1]) + '_'+ str(time.localtime()[0])
    camera.annotate_text = When 
    camera.annotate_text_size = 12
    camera.capture('/home/pi/Desktop/%s.jpg', When)
    # make sure to stop the preview after taking the picture
    camera.stop_preview() 

def MMORPG():
    # MMORPG uses a "H-Bridge" motor driver, this means that we only need
    # two wires running from the controler to the motor driver, and the controler does not
    # need to supply the power for the motor.  Every motor driver has a different wiring diagram,
    # but in general, you need two wires supplying either a "HIGH" or "LOW" signal to the motor driver.
   
    leveled = False
    while not leveled:
        # these if statements determine the shortest path for the system to achieve "level", so that
        # it does not turn the wrong direction.  The direction of the motor can be commanded by setting
        # one of the two wires "HIGH" and the other "LOW"  
        if (roll > 4) and (roll < 90):
            MMORPGa.high() 
            MMORPGb.low()
        if (roll <176) and (roll > 90):
            MMORPGa.low()
            MMORPGb.high()
        if (roll > 176) or (roll < 4):
            leveled = Ture
    if leveled:
        # A motor can be locked by setting both wires to "HIGH" this stops the system from drifting
        # after achieving level
        MMORPGa.high()
        MMORPGb.high()
    
        
def RAPS():
    # set one motor "HIGH" and the other "LOW" to move the motor
    RAPSa.high()
    RAPSb.low()
    # Wait for the motor to move into place
    time.sleep(2)
    # Lock the motor in place by setting both wires to "HIGH"
    RAPSa.high()
    RAPSb.high()
    

def forward(delay, steps):
    # This is a function for controling the stepper motor.
    # It is important to have a short delay after every step to
    # make sure that the step is performed correctly
    i = 0
    while i in range(0, steps):
        setStep(1, 0, 1, 0)
        time.sleep(delay)
        setStep(0, 1, 1, 0)
        time.sleep(delay)
        setStep(0, 1, 0, 1)
        time.sleep(delay)
        setStep(1, 0, 0, 1)
        time.sleep(delay)
        i += 1

def backwards(delay, steps):
    # This is a function for controling the stepper motor.
    # It is important to have a short delay after every step to
    # make sure that the step is performed correctly
    i = 0
    while i in range(0, steps):
        setStep(1, 0, 0, 1)
        time.sleep(delay)
        setStep(0, 1, 0, 1)
        time.sleep(delay)
        setStep(0, 1, 1, 0)
        time.sleep(delay)
        setStep(1, 0, 1, 0)
        time.sleep(delay)
        i += 1

def setStep(w1, w2, w3, w4):
    # This is a function for controling the stepper motor.
    # It is important to have a short delay after every step to
    # make sure that the step is performed correctly
    coil_A_1_pin.value = w1
    coil_A_2_pin.value = w2
    coil_B_1_pin.value = w3
    coil_B_2_pin.value = w4

#%% CAMERA COMMAND STUFF USING PiCamera

import os
 

# camera = PiCamera() -> Ben already did this

 
# Read the commands from the file
with open("file.txt", "r") as f:
    commands = f.read().strip().split()

# Process each command
for command in commands:
    if command == "A1":
        camera.rotate(60)
    elif command == "B2":
        camera.rotate(-60)
    elif command == "C3":
        camera.capture("image.jpg")
        time.sleep(5) # to take pic
    elif command == "D4":
        camera.color_effects = (128, 128)
    elif command == "E5":
        camera.color_effects = None
    elif command == "F6":
        camera.rotation = 180
    elif command == "G7":
        camera.image_effect = "colorbalance"
    elif command == "H8":
        camera.image_effect = None


# Remove the image file?

# os.remove('image.jpg')


