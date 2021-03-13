import serial, re
from threading import Thread
from queue import LifoQueue
from time import sleep

def init_serial(connection):
    serialConnection = serial.Serial(connection, 9600, timeout=10)
    print("Starting Connection:")
    return serialConnection

def close_serial(serialConnection):
    serialConnection.close()
    print("Closing Connection")

def send_message(serialConnection, message):
    serialConnection.write(message.encode())
    
    

def poll_for_messages(numberOfSensors, sharedQueue, serialConnection):
    if (numberOfSensors <= 0 or not sharedQueue or not serialConnection):
        return
    
    ir_sensors = [0 for i in range(numberOfSensors)]
    sharedQueue.put(ir_sensors)
    
    # Regex format for comparison
    ir_format = re.compile("I\d{" + str(numberOfSensors) + "}U")
    
    global end_ir_thread
    end_ir_thread = False
    
    while not end_ir_thread:
    
        received_data = ''
        
        try:
            received_data = serialConnection.readline().decode("utf-8").strip('\x00')
        except KeyboardInterrupt:
            break
        except Exception as e:
            #print(e)
            continue
        
        if (received_data and ir_format.match(received_data)):
            ir_sensors = [int(received_data[i+1]) for i in range(numberOfSensors)]
            #print(received_data)
            #print(ir_sensors)
            try:
                while(not sharedQueue.empty()): (sharedQueue.get_nowait())
            except:
                pass
            sharedQueue.put(ir_sensors)
        
    close_serial(serialConnection)


def get_updated_ir_values(oldArray, sharedQueue):
    try:
        return sharedQueue.get_nowait() if(not sharedQueue.empty()) else (oldArray)
    except:
        return oldArray

def stop_ir_thread():
    global end_ir_thread
    end_ir_thread = True

def init_ir_sensor_array(numberOfSensors, serial):
    serialConnection = init_serial(serial)
    
    sharedQueue = LifoQueue()
    
    ir_thread = Thread(target=poll_for_messages, args=(numberOfSensors, sharedQueue, serialConnection,))
    ir_thread.start()
    
    return sharedQueue
    

if __name__ == '__main__':
    ir_sensor_array = [-1 for i in range(5)]
    
    ir_value_queue = init_ir_sensor_array(5, "/dev/ttyS0")
    print("Initialized")
    sleep(5)
    
    print("Saved Values:")
    ir_sensor_array = get_updated_ir_values(ir_sensor_array, ir_value_queue)
    print("\n"+str(ir_sensor_array)+"\n")
    
    stop_ir_thread()
    
    