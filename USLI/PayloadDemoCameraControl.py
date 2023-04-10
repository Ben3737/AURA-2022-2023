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
#import RPi.GPIO as GPIO
#GPIO.setmode(GPIO.BOARD)
import picamera # This is used to interface with the camera
import time         # This is used for "sleep" functions and other utilities involving time
import digitalio    # This is used to interface with the motors, and servos
import board
import adafruit_mpu6050
import bmpsensor
from fusion import Fusion

#========================================================================
# Create objects
#========================================================================
# Create camera object

camera = picamera.PiCamera() # this creates an object required to interface with the camera library
camera.resolution = (1920, 1080) # this deffines the quality of the images
#Create MPU6050 object
#i2c = board.I2C()  # uses board.SCL and board.SDA
#mpu = adafruit_mpu6050.MPU6050(i2c)

# Create motor objects


#Motor1A = 38
#Motor1B = 40
#Motor2A = 37
#Motor2B = 35

#GPIO.setup(Motor1A, GPIO.OUT)
#GPIO.setup(Motor1B, GPIO.OUT)

#GPIO.setup(Motor2A, GPIO.OUT)
#GPIO.setup(Motor2B, GPIO.OUT)

#========================================================================
# Functions:
#========================================================================

def takepicture():
    camera.start_preview()
    time.sleep(2)
    When = str(time.localtime()[3]) + '_' + str(time.localtime()[4]) + '_' + str(time.localtime()[5]) + '_' + str(time.localtime()[2]) + '_' + str(time.localtime()[1]) + '_' + str(time.localtime()[0])
    camera.annotate_text = When
    camera.annotate_text_size = 50
    camera.capture('/home/pi/Desktop/payloadDemo2023/Photos/' + When + '.png')
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
        roll = 0
        GPIO.output(Motor1A, GPIO.LOW)
        GPIO.output(Motor1B, GPIO.HIGH)
        time.sleep(1)
        GPIO.output(Motor1A, GPIO.LOW)
        GPIO.output(Motor1B, GPIO.LOW)
            
        if (roll > 4) and (roll < 90):
            GPIO.output(Motor1A, GPIO.LOW)
            GPIO.output(Motor1B, GPIO.HIGH)
        if (roll <176) and (roll > 90):
            GPIO.output(Motor1A, GPIO.HIGH)
            GPIO.output(Motor1B, GPIO.LOW)
        if (roll > 176) or (roll < 4):
            leveled = True
    if leveled:
        # A motor can be locked by setting both wires to "HIGH" this stops the system from drifting
        # after achieving level
        GPIO.output(Motor1A, GPIO.LOW)
        GPIO.output(Motor1B, GPIO.LOW)
    
        
def RAPS():
    GPIO.output(Motor2A, GPIO.HIGH)
    GPIO.output(Motor2B, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(Motor2A, GPIO.LOW)
    GPIO.output(Motor2B, GPIO.LOW)
    
    

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
    
def getAltitude():
    temp, pressure, altitude = bmpsensor.readBmp180()
    return altitude

def angles():
    accel = mpu.acceleration
    gyro = mpu.gyro
    data = Fusion.update_nomag(accel, gyro)
    return data

    
#========================================================================
# Create DC motor objects
#========================================================================
# Define the outputs that are dedicated to MMORPG



#========================================================================
# Create stepper motor objects
#========================================================================
# Stepper motors are complex, if you care how they actually work, I sugest reading the artical linked in
# the headder
coil_A_1_pin = digitalio.DigitalInOut(board.D17)
coil_A_2_pin = digitalio.DigitalInOut(board.D27)
coil_B_1_pin = digitalio.DigitalInOut(board.D22)
coil_B_2_pin = digitalio.DigitalInOut(board.D23)

coil_A_1_pin.direction = digitalio.Direction.OUTPUT
coil_A_2_pin.direction = digitalio.Direction.OUTPUT
coil_B_1_pin.direction = digitalio.Direction.OUTPUT
coil_B_2_pin.direction = digitalio.Direction.OUTPUT

    
#========================================================================
# Determine the state of the payload using avalible sensor data:
#========================================================================
Launched = False
Apogee = False
Landed = False
count = 0
#startHeight = getAltitude()
#print(startHeight)
"""
while not Landed:  
    while not Launched:
        pitch = 0
        roll = 0
        altitude = getAltitude()
        print(altitude)
        if (pitch < 45) and (altitude > startHeight + 100) and (roll < 45):
            count += 1
        if (pitch > 45) or (altitude < startHeight + 100) or (roll > 45):
            count = 0
        if (count > 10):
            Launched = True
            print("Launched")
            #oldAltitude = getAltitude()
    if Launched:
        count = 0
        while not Apogee:
            altitude = getAltitude()
            verticalSpeed = altitude - oldAltitude
            oldAltitude = altitude
            if (verticalSpeed < 0):
                count += 1
            if (verticalSpeed > 0):
                count = 0
            if (count > 2):
                Apogee = True
                print("Apogee")
                count = 0
        while not Landed:
            altitude = getAltitude()
            verticalSpeed = altitude - oldAltitude
            oldAltitude = altitude
            if (altitude < startHeight + 1):
                count += 1
                print("count +")
            if (altitude > startHeight + 1):
                count = 0
                print("count-")
            if (count > 10):
                Landed = True
                print("landing")
        if Landed:
            print("Landing")
            break
"""
# add a delay here for one minute
#MMORPG()
time.sleep(60)
#RAPS()

#========================================================================
# Task 1: A1—Turn camera 60º to the right
#========================================================================
# This task is done using a stepper motor
#backwards(0.01, 90)
takepicture()


#========================================================================
# Task 2: B2—Turn camera 60º to the left
#========================================================================
# This task is done using a stepper motor
#forward(0.01, 90)
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
camera.color_effects = (0,0)
camera.image_effect = 'none'
takepicture()

#========================================================================
# Task 6: F6—Rotate image 180º (upside down). camera.rotation = 180
#========================================================================
camera.rotation = 180
takepicture()

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





