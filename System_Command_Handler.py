import sys
sys.path.append("./Communication_Systems/SSL_Communication_Services")
#import GPS_Communication_Client

import central_code
import struct
from threading import Thread

mainDataFormat = "20s"

def parseCommand(command):

    # Decide on action based on the received message
    if command == "Start":
        main_thread = Thread(target=central_code.main(), args=())
        main_thread.start()
        return package_reply("Started Robot")

    elif command == "Shutdown":
        central_code.stop_program()
        return package_reply("Stopped Robot")

    elif command == "TestGetGPS":
        # Get current GPS location, TEMPORARILY REMOVED
        #currentLocation = GPS_Communication_Client.gpsPingClient(debug=1)
        return package_reply("0,0") #(str(currentLocation[0]) + " " + str(currentLocation[1])))
    
    else:
        return package_reply("Bad Command")

def package_reply(reply):
    return struct.pack(mainDataFormat, bytes(reply, "utf-8"))