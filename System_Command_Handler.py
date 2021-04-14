import sys
import time
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

    elif command == "TestCommandHandler":
        return package_reply("Command Received") 
    
    elif command == "getCurrentTime":
        # Return Ascii timestamp without year to fit the character limit
        partial_timestamp = str(time.asctime()).rsplit(' ', 1)[0]
        return package_reply(partial_timestamp)

    else:
        return package_reply("Bad Command")

def package_reply(reply):
    return struct.pack(mainDataFormat, bytes(reply, "utf-8"))