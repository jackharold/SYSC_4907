import serial
from time import sleep



def init_serial():
    serialConnection = serial.Serial("/dev/ttyS0", 9600, timeout=10)
    print("Starting Connection:")
    return serialConnection

def close_serial(serialConnection):
    serialConnection.close()
    print("Closing Connection")

def send_message(serialConnection, message):
    serialConnection.write(message.encode())
    


def poll_for_messages(serialConnection):
    
    while True:
    
        received_data = ''
        
        try:
            received_data = serialConnection.readline().decode("utf-8").strip('\x00')
            
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(e)
            break
        
        if (received_data): print(received_data)




if __name__ == '__main__':
    
    ser = init_serial()
    
    #send_message(ser, "1")
    #sleep(2)
    #send_message("2")
    
    poll_for_messages(ser)
    
    close_serial(ser)
    
