import serial, re
from threading import Thread
from queue import LifoQueue
from time import sleep
import serial_connection


def poll_US_messages(numberOfSensors, sharedQueue, serialConnection, debug=False):
    
    # Return immediately if provided bad parameters
    if (numberOfSensors <= 0 or not sharedQueue or not serialConnection):
        return
    
    # Initialize the existing state of the array and queue
    us_sensors = [0.00 for i in range(numberOfSensors)]
    sharedQueue.put(us_sensors)
    
    # Define Regex format for comparison
    us_format = re.compile("^U([\d.]+U){"+ str(numberOfSensors) +"}$")
    
    global end_us_thread
    end_us_thread = False
    
    # Main updating loop for the queue
    while not end_us_thread:
        received_data = ''
        
        try:
            received_data = serialConnection.readline().decode("utf-8").strip('\x00')
        except KeyboardInterrupt:
            break
        except Exception as e:
            continue
        
        # If regex matches format, add
        if (received_data and us_format.match(received_data)):
            
            # Parse Data into array
            us_sensors = [float(received_data.split("U")[i+1]) for i in range(numberOfSensors-1)]
            
            if (debug): print("Unfiltered Input", str(received_data))
            if (debug): print("Array:", str(us_sensors))
            
            try:
                while(not sharedQueue.empty()): (sharedQueue.get_nowait())
            except:
                pass
            sharedQueue.put(us_sensors)
        
    serial_connection.close_serial(serialConnection)

# Used to update the given array if a new state is available
def get_updated_us_values(oldArray, sharedQueue):
    try:
        return sharedQueue.get_nowait() if(not sharedQueue.empty()) else (oldArray)
    except:
        return oldArray

# Call to stop a currently running us thread
def stop_us_thread():
    global end_us_thread
    end_us_thread = True

# Call to initialize an us thread and returns the shared Data queue
def init_us_sensor_array(numberOfSensors, serial, baud_rate):
    serialConnection = serial_connection.init_serial(serial, baud_rate)
    
    sharedQueue = LifoQueue()
    
    us_thread = Thread(target=poll_US_messages, args=(numberOfSensors, sharedQueue, serialConnection,))
    us_thread.start()
    
    return sharedQueue
    

# Example Use Case:
# 
if __name__ == '__main__':
    us_sensor_array = [0.00 for i in range(5)]

    print("Initialize Sensor Collection")
    us_value_queue = init_us_sensor_array(3, "/dev/ttyS0", 9600)
    sleep(5)
     
     
    us_sensor_array = get_updated_us_values(us_sensor_array, us_value_queue)
     
    print("Saved Values:")
    print("\n"+str(us_sensor_array)+"\n")
     
    stop_us_thread()
    
    
