from socket import *
import ssl
import struct
import geocoder

# Initialize Constants needed for connections
serverCert = "gpsServer.crt"
clientCert = "gpsClient.crt"
clientKey = "gpsClient.key"
server_name = "testServer.com"
serverIP = "127.0.0.1"
serverPort = 14700
clientDataFormat = "20s"
serverDataFormat = "ff"



def gpsPingClient(debug = False):
    if (debug): print("gpsClient: --- Establishing TCP Connection ---")
    
    # Configure the Client Certificates and socket
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=serverCert)
    context.load_cert_chain(certfile=clientCert, keyfile=clientKey)

    clientSocket = socket(AF_INET, SOCK_STREAM)
    
    
    # Attempt to connect to the server, returning if no connection possible
    try:
        clientSocket.connect((serverIP, serverPort))
    except:
        print("Connection Unsuccessful: Exiting ...")
        return "Connection Unsuccessful"
    
    clientConnection = context.wrap_socket(clientSocket, server_side=False, server_hostname=server_name)
    
    # Send a single Request to get the server's gps location
    requestType = "GetGPS"    
    message = struct.pack(clientDataFormat, bytes(requestType, "utf-8"))
    clientConnection.sendall(message)
    
    if (debug): print("gpsClient: --- Sent Request ---")
    
    # Wait for response and unpack returned data
    data = clientConnection.recv(4096)
    
    if (debug): print("gpsClient: --- Received Reply ---")
    
    reply = struct.unpack(serverDataFormat, data)
    
    
    # After the request is fulfilled, close connection 
    clientConnection.close()
    
    # Test formatting against example
    if (type(reply) == type( (0.0, -1.2) )) :
        return reply
    else:
        return "Request Unsuccessful"
    

# Start the server with debug active and print the Location
if __name__ == '__main__':
    print("Curent Base Station GPS Location:", gpsPingClient(debug=1))
    