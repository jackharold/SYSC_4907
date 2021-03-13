import serial

def init_serial(connection, baud_rate):
    serialConnection = serial.Serial(connection, baud_rate, timeout=10)
    print("Starting Serial Connection:")
    return serialConnection

def close_serial(serialConnection):
    serialConnection.close()
    print("Closing Serial Connection")

def send_message(serialConnection, message):
    serialConnection.write(message.encode())
    
    