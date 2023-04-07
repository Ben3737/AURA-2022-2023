# Jacob Birch
# 4-6-23

import os
import time
from picamera import PiCamera

 

camera = PiCamera()

 

# Define the camera commands

commands = {

    "A1": lambda: camera.rotate(60),

    "B2": lambda: camera.rotate(-60),

    "C3": lambda: camera.capture('image.jpg'),

    "D4": lambda: camera.color_effects((128, 128)),

    "E5": lambda: camera.color_effects(None),

    "F6": lambda: camera.rotation = 180,

    "G7": lambda: camera.image_effect = 'negative',

    "H8": lambda: camera.image_effect = 'none',

}

 

# Read the APRS message from file.txt

with open('file.txt', 'r') as file:

    message = file.read().strip()

 

# Parse the APRS message and execute the camera commands

for command in message.split(' '):

    if command in commands:

        commands[command]()

 

# Wait for the camera to finish capturing the image

time.sleep(5)

 

# Remove the image file

os.remove('image.jpg')
