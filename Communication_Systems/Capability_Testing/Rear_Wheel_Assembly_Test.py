import RPi.GPIO as GPIO
import time

startMotorPin = 37

def start_motor():
    
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(startMotorPin, GPIO.OUT)
    
    print("\nHit Enter to Restart Motor or 'exit' to exit program")
    testInput = ""
    while testInput != "exit":
        
        testInput = input()
        if (testInput != ""): continue
        
        GPIO.output(startMotorPin, 1)
        time.sleep(1)
        GPIO.output(startMotorPin, 0)
        
    
    GPIO.cleanup(startMotorPin)



if __name__ == '__main__':
    start_motor()
