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

#========================================================================
# Create objects
#========================================================================
# Create camera object
#camera = picamera.PiCamera() # this creates an object required to interface with the camera library
#camera.resolution = (1920, 1080) # this deffines the quality of the images

# Set the callsign
callsign = "KQ4CTL-6"


#========================================================================
# Functions:
#========================================================================


def takepicture():
    # This function uses the Pi camera library to take a picture
    camera.start_preview()
    time.sleep(2)
    When = str(time.localtime()[3]) + '_' + str(time.localtime()[4]) + '_' + str(time.localtime()[5]) + '_' + str(time.localtime()[2]) + '_' + str(time.localtime()[1]) + '_' + str(time.localtime()[0])
    camera.annotate_text = When
    camera.annotate_text_size = 50
    camera.capture('/home/pi/Desktop/payloadDemo2023/Photos/' + When + '.png')
    camera.stop_preview()
    
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
# Camera Control loop:
#========================================================================
# Set the time between message transmissions
messageDelay = 60
# Set the name of the file
file = "log.txt.txt"

while True:
    # Read the second to last line of the file
    f = open(file, 'r')
    callsignLine = f.readlines()[-3]
    # Read the last line of the file
    f = open(file, 'r')
    message = f.readlines()[-1]
    # Print the information and close the file
    #print(callsignLine)
    #print(message)
    f.close()

    # check if the message came from the correct call sign
    i = 0
    while(i <= len(callsignLine)-8):
        callsignFound = str(callsignLine[i]) + str(callsignLine[i+1]) + str(callsignLine[i+2]) + str(callsignLine[i+3]) + str(callsignLine[i+4]) + str(callsignLine[i+5]) + str(callsignLine[i+6]) + str(callsignLine[i+7])
        if (callsignFound == callsign):
            importantMessage = True
            i = len(callsignLine)
            print("important message received")
        else:
            importantMessage = False
            i = i + 1
    
    # if a message from the correct call sign was received,  read the commands
    if (importantMessage):
        skipover = False
        readingTime = 0 #message.find('{') + 1
            
        while(skipover == False):
            command = str(message[readingTime]) + str(message[readingTime+1])
                    
            readingTime = readingTime + 1
            if (str(message[readingTime]) == '{'):
                skipover = True
            readingTime = readingTime + 1
            if (str(message[readingTime]) == '{'):
                skipover = True
            readingTime = readingTime + 1
            if (str(message[readingTime]) == '{'):
                skipover = True
                        
            print(command)
            #print(readingTime)
            #print(message[readingTime])
            
            
    
            #========================================================================
            # Task 1: A1—Turn camera 60º to the right
            #========================================================================
            if (command == 'A1'):
                # This task is done using a stepper motor
                backwards(0.001, 90)
                print("A1 done")
    
    
            #========================================================================
            # Task 2: B2—Turn camera 60º to the left
            #========================================================================
            if(command == 'B2'):
                # This task is done using a stepper motor
                forward(0.001, 90)
                print("B2 done")
    
            #========================================================================
            # Task 3: C3—Take picture
            #========================================================================
            if (command == 'C3'):
                # This task takes a picture
                takepicture()
                print("C3 done")

            
            #========================================================================
            # Task 4: D4—Change camera mode from color to grayscale
            #========================================================================
            if (command == 'D4'):
                camera.color_effects = (128, 128)
                print("D4 done")
    
            #========================================================================
            # Task 5: E5—Change camera mode back from grayscale to color
            #========================================================================
            if (command == 'E5'):
                camera.color_effects = (0,0)
                print("E5 done")
    
            #========================================================================
            # Task 6: F6—Rotate image 180º (upside down). camera.rotation = 180
            #========================================================================
            if (command == 'F6'):
                camera.rotation = 180
                print("F6 done")
    
            #========================================================================
            # Task 7: G7—Special effects filter (Apply any filter or image distortion you want and
            # state what filter or distortion was used).
            #========================================================================
            if (command == 'G7'):
                camera.image_effect = 'colorswap'
                print("G7 done")
    
            #========================================================================
            # Task 9: H8—Remove all filters.
            #========================================================================
            if (command == 'H8'):
                camera.image_effect = 'none'
                print("H8 done")
            
            
            #========================================================================
            # Sleep for one minute to wait for a new message
            #========================================================================
        print("All commands done")
        time.sleep(messageDelay)
            