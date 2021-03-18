from socket import *
import ssl
import struct

# Initialize Constants needed for connections
gpsServerCert = "gpsServer.crt"
gpsClientCert = "carServer.crt"
clientCert = "debugClient.crt"
clientKey = "debugClient.key"
gpsServerName = "testServer.com"
gpsClientName = "carServer.com"
encryption_cipher = "ECDHE-ECDSA-AES128-GCM-SHA256"
#serverIP = "127.0.0.1"
#serverPort = 14700
clientDataFormat = "20s"
serverDataFormat = "ff"



def debugClient(serverType = "", serverIp = "", serverPort = "", requestCommand = "", debug = False):
    
    # If not provided, prompt the user for the connection details
    
    while serverType == "":
        serverType = str(input("Enter Server Type: "))
        
    if(serverType == "Base"):
        serverCert = gpsServerCert
        serverName = gpsServerName
    elif(serverType == "Car"):
        serverCert = gpsClientCert
        serverName = gpsClientName
    else:
        return "Invalid Server Type"
        
    while serverIp == "":
        serverIp = str(input("Enter Server IP Address: "))
        
    while serverPort == "":
        serverPort = int(input("Enter Server Port Number: "))
        
    while requestCommand == "":
        requestCommand = str(input("Enter Command: "))
    
    print(serverType, serverIp, serverPort, requestCommand)
    if (debug): print("debugClient: --- Establishing TCP Connection ---")
    
    # Configure the Client Certificates and socket
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=serverCert)
    context.load_cert_chain(certfile=clientCert, keyfile=clientKey)
    context.set_ciphers(encryption_cipher)

    clientSocket = socket(AF_INET, SOCK_STREAM)
    
    
    # Attempt to connect to the server, returning if no connection possible
    try:
        clientSocket.connect((serverIp, serverPort))
    except Exception as e:
        print(e)
        print("Connection Unsuccessful: Exiting ...")
        return "Connection Unsuccessful"
    
    clientConnection = context.wrap_socket(clientSocket, server_side=False, server_hostname=serverName)
    
    # Send a single Request to get the server's gps location   
    message = struct.pack(clientDataFormat, bytes(requestCommand, "utf-8"))
    clientConnection.sendall(message)
    
    if (debug): print("debugClient: --- Sent Request ---")
    
    # Wait for response and unpack returned data
    data = clientConnection.recv(4096)
    
    if (debug): print("debugClient: --- Received Reply ---")
    
    reply = struct.unpack(clientDataFormat, data)[0]

    return bytes.decode(reply).strip("\x00")
    

# Start the server with debug active and print the Location
if __name__ == '__main__':
    #print("Curent Base Station GPS Location:", debugClient(debug=1))
    while True:
        try:
            print("Returned:", debugClient(serverType = "Car", serverIp="127.0.0.1", serverPort=14800, debug=1))
        except KeyboardInterrupt as e:
            print("\nExiting Client ...")
            break
        except Exception as e:
            print(e)
            break
