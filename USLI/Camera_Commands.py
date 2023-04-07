# Jacob Birch
# 4-6-23

import os
import time
from picamera import PiCamera

 

camera = PiCamera()

 


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
