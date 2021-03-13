from socket import *
import ssl
import struct
import geocoder

# Initialize Constants needed for connections
serverCert = "gpsServer.crt"
serverKey = "gpsServer.key"
clientCertFolder = "./BaseStationClientCerts"
serverIP = "127.0.0.1"
serverPort = 14700
clientDataFormat = "20s"
serverDataFormat = "ff"

# Returns a Simulated Latitude and Longitude Pair
def simulatedGPS():
    return geocoder.ipinfo('me').latlng



def gpsPingServer(debug = False):
    
    # Configure the Server Certificates
    print("Creating Server ...")
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(certfile=serverCert, keyfile=serverKey)
    context.load_verify_locations(capath=clientCertFolder)
    context.check_hostname = False
    context.verify_mode =ssl.CERT_NONE
    
    # Create thhe socket and link it to port 14700
    receivingSocket = socket(AF_INET, SOCK_STREAM)
    receivingSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    receivingSocket.bind((serverIP, serverPort))
    receivingSocket.listen(5)
    
    # Begin Accepting Requests from the Client
    while True:
        clientConnection = ""
        try:
            # Establish Incoming Connection from Client
            if (debug): print("gpsServer: --- Waiting on Request ---")
            clientSocket, clientAddress = receivingSocket.accept()
            
            clientConnection = context.wrap_socket(clientSocket, server_side=True)
        

            
            if (debug): print("gpsServer: --- Waiting for Data ---")
            
            # Receive and Parse incoming data
            data = clientConnection.recv(4096)
            message = struct.unpack(clientDataFormat, data)
            
            if (debug): print("gpsServer: --- Recieved Message:", message, "---")
            
            # Decide on action based on the received message
            if bytes.decode(message[0]).strip("\x00") == "GetGPS":
                
                # Get current GPS location
                currentLocation = simulatedGPS()
                
                returnMessage = struct.pack(serverDataFormat, currentLocation[0], currentLocation[1])
                clientConnection.send(returnMessage)
                
                if (debug): print("gpsServer: --- Sent Message:", currentLocation, "---")
        
        except Exception as e:
            print(e)
            # On Error, close the server's socket and exit the waiting loop
            print("Closing Server ...")
            if clientConnection:
                clientConnection.close()
            break
    

# Start the server with debug active
if __name__ == '__main__':
    gpsPingServer(debug=1)
    