# Import libraries
import RPi.GPIO as GPIO
import time

# Set GPIO numbering mode
GPIO.setmode(GPIO.BOARD)

# Set pin 11 as an output, and set servo1 as pin 11 as PWM
GPIO.setup(11,GPIO.OUT)
servo1 = GPIO.PWM(11,50) # Note 11 is pin, 50 = 50Hz pulse

#start PWM running, but with value of 0 (pulse off)
servo1.start(0)
count = 12
#for dc in range(12):
#    servo1.ChangeDutyCycle(count)
#    time.sleep(0.1)
#    count = count - 1 
servo1.ChangeDutyCycle(12)
time.sleep(2)
  
servo1.stop()

GPIO.cleanup()
print ("Goodbye")

