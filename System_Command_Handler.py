import sys
sys.path.append("./Communication_Systems/SSL_Communication_Services")
import GPS_Communication_Client

import central_code
import struct
from threading import Thread

mainDataFormat = "20s"
testGetGPSDataFormat = "ff"

def parseCommand(command):

    # Decide on action based on the received message
    if command == "Start":
        main_thread = Thread(target=central_code.main(), args=(, ))
        main_thread.start()
        return struct.pack(mainDataFormat, "Started Robot")

    elif command == "Shutdown":
        
        return struct.pack(mainDataFormat, "Stopped Robot")

    elif command == "TestGetGPS":
        # Get current GPS location
        currentLocation = GPS_Communication_Client.gpsPingClient(debug=1)
        return struct.pack(mainDataFormat, (str(currentLocation[0]) + " " + str(currentLocation[1])))
    
    else:
        return struct.pack(mainDataFormat, "Bad Command")
