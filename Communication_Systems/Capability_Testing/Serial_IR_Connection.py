import serial, re
from threading import Thread
from queue import LifoQueue
from time import sleep
import serial_connection


def poll_IR_messages(numberOfSensors, sharedQueue, serialConnection, debug=False):
    
    # Return immediately if provided bad parameters
    if (numberOfSensors <= 0 or not sharedQueue or not serialConnection):
        return
    
    # Initialize the existing state of the array and queue
    ir_sensors = [0 for i in range(numberOfSensors)]
    sharedQueue.put(ir_sensors)
    
    # Define Regex format for comparison
    ir_format = re.compile("^I\d{" + str(numberOfSensors) + "}U$")
    
    global end_ir_thread
    end_ir_thread = False
    
    # Main updating loop for the queue
    while not end_ir_thread:
        received_data = ''
        
        try:
            received_data = serialConnection.readline().decode("utf-8").strip('\x00')
        except KeyboardInterrupt:
            break
        except Exception as e:
            continue
        
        # If regex matches format, add
        if (received_data and ir_format.match(received_data)):
            
            # Parse Data into array
            ir_sensors = [int(received_data[i+1]) for i in range(numberOfSensors)]
            
            if (debug): print("Unfiltered Input", str(received_data))
            if (debug): print("Array:", str(ir_sensors))
            
            try:
                while(not sharedQueue.empty()): (sharedQueue.get_nowait())
            except:
                pass
            sharedQueue.put(ir_sensors)
        
    serial_connection.close_serial(serialConnection)

# Used to update the given array if a new state is available
def get_updated_ir_values(oldArray, sharedQueue):
    try:
        return sharedQueue.get_nowait() if(not sharedQueue.empty()) else (oldArray)
    except:
        return oldArray

# Call to stop a currently running ir thread
def stop_ir_thread():
    global end_ir_thread
    end_ir_thread = True

# Call to initialize an ir thread and returns the shared Data queue
def init_ir_sensor_array(numberOfSensors, serial, baud_rate):
    serialConnection = serial_connection.init_serial(serial, baud_rate)
    
    sharedQueue = LifoQueue()
    
    ir_thread = Thread(target=poll_IR_messages, args=(numberOfSensors, sharedQueue, serialConnection,))
    ir_thread.start()
    
    return sharedQueue
    

# Example Use Case:
# 
if __name__ == '__main__':
    ir_sensor_array = [0 for i in range(5)]

    print("Initialize Sensor Collection")
    ir_value_queue = init_ir_sensor_array(5, "/dev/ttyS0", 9600)
    sleep(5)
     
     
    ir_sensor_array = get_updated_ir_values(ir_sensor_array, ir_value_queue)
     
    print("Saved Values:")
    print("\n"+str(ir_sensor_array)+"\n")
     
    stop_ir_thread()
    
    