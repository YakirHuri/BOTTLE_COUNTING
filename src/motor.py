
# Import libraries
import RPi.GPIO as GPIO
import time



def main(servoPin):
    # Set GPIO numbering mode
    GPIO.setmode(GPIO.BOARD)

    # Set pin 11 as an output, and set servo1 as pin 11 as PWM
    GPIO.setup(int(servoPin),GPIO.OUT)
    servo1 = GPIO.PWM(int(servoPin),50) # Note 11 is pin, 50 = 50Hz pulse

    #start PWM running, but with value of 0 (pulse off)
    servo1.start(0)
    #print ("Waiting for 2 seconds")
    time.sleep(2)

    #Let's move the servo!
    #print ("Rotating 180 degrees in 10 steps")

    # Define variable duty
    duty = 2

    # Loop for duty values from 2 to 12 (0 to 180 degrees)
    while duty <= 12:
        servo1.ChangeDutyCycle(duty)
        time.sleep(0.3)
        servo1.ChangeDutyCycle(0)
        time.sleep(0.1)
        duty = duty + 1

    # Wait a couple of seconds
    time.sleep(2)

    # Turn back to 90 degrees
    #print ("Turning back to 90 degrees for 2 seconds")
    servo1.ChangeDutyCycle(7)
    time.sleep(0.5)
    servo1.ChangeDutyCycle(0)
    time.sleep(1.5)


    #Clean things up at the end
    servo1.stop()
    GPIO.cleanup()
    #print ("Goodbye")

if __name__ == "__main__":
    import sys

    servoPin = sys.argv[1] 
    main(servoPin)
