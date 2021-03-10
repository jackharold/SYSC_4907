import serial
import time

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.flush()
ser.write(b"reposition right")
time.sleep(5)
ser.write(b"reposition left")