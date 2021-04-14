/*
  By: Ragini Bakshi
  Using Structs by SparkFun Electronics
  License: MIT. 

  This sketch configures the High Navigation Rate on the NEO-M8U. It takes inputs the home point, 
  parking point, node coordinate vector, number of nodes to traverse, moving distance and moving angle 
  from a python A* algorithm, and using an infinite loop, updates the current latitude and longitude 
  positions as new data became available. If within the right tolerance, checkNode function updates the node
  numbers and the vehicle moves towards the destination point.

  This example uses "autoHNR" from Sparkfun to receive the HNR data automatically.

  NEO-M8U is running UDR firmware >= 1.31. 

  Serial monitor: 115200 baud to see the output

*/

#include <Wire.h> //Needed for I2C to GPS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

#define PI 3.1415926535897932384626433832795
boolean usingAutoHNRAtt = false;
boolean usingAutoHNRDyn = false;
boolean usingAutoHNRPVT = false;

double pos_array[5];

// DATA FROM PYTHON FILE
int distance_to_go[] = {5, 7, 7, 7, 5, 5, 5, 5, 7, 5};                  // in meters to next node
int angles_at_nodes[] = {90, 45, 45, 45, 90, 90, 90, 90, 135, 180};     // angles
double home_point[] = {45.38631, -75.69268};                            // Form: latitude, longitude: 2,15 graph AKA 2,16 in python
double park_point[] = {45.38673, -75.69261};                            // Form: latitude, longitude: 3,6 graph AKA 3,7 in python
double node_coords[] = {//45.38631, -75.69268, //home point
                        45.38636, -75.69268, //2,14
                        45.38641, -75.69262, //3,13
                        45.38645, -75.69256, //4,12
                        45.38650, -75.69249, //5,11
                        45.38654, -75.69249, //5,10
                        45.38659, -75.69249, //5,9
                        45.38663, -75.69249, //5,8
                        45.38668, -75.69249, //5,7
                        45.38673, -75.69255, //4,6
                        45.38673, -75.69261}; //park point                

// Arrays relevant to moving 
int node_number;
int loop_counter;
int moving_distance;
int moving_angle;
double start_point[] = {0,0};                                           // for each moving iteration
double target_point[] = {0,0};                                          // target for each moving iteration
double current_pos[] = {0,0};


// update pos array to have meter value
void checkNode(double pos_array[]) {
  if(node_number > 9){
    //end the program, parking reached
    //stop serial comm
   while(1); 
  }
  if((abs(current_pos[0] - target_point[0])<= 0.00001) && (abs(current_pos[1] - target_point[1])<=0.00002)){ // we are the next node, update all values
    node_number = node_number + 1;
    start_point[0] = current_pos[0];
    start_point[1] = current_pos[1];
    target_point[0] = node_coords[node_number*2];
    target_point[1] = node_coords[node_number*2 + 1];
    moving_distance = distance_to_go[node_number];
    moving_angle = angles_at_nodes[node_number]; 
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println(F("SparkFun u-blox Example"));

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable debug messages on Serial

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("Warning! u-blox GPS did not begin correctly."));
    Serial.println(F("(This may be because the I2C port is busy with HNR messages.)"));
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  if (myGNSS.setHNRNavigationRate(10) == true) //Set the High Navigation Rate to 10Hz
    Serial.println(F("setHNRNavigationRate was successful"));
  else
    Serial.println(F("setHNRNavigationRate was NOT successful"));

  usingAutoHNRAtt = myGNSS.setAutoHNRATT(true); //Attempt to enable auto HNR attitude messages
  if (usingAutoHNRAtt)
    Serial.println(F("AutoHNRATT successful"));

  usingAutoHNRDyn = myGNSS.setAutoHNRINS(true); //Attempt to enable auto HNR vehicle dynamics messages
  if (usingAutoHNRDyn)
    Serial.println(F("AutoHNRINS successful"));

  usingAutoHNRPVT = myGNSS.setAutoHNRPVT(true); //Attempt to enable auto HNR PVT messages
  if (usingAutoHNRPVT)
    Serial.println(F("AutoHNRPVT successful"));

  loop_counter = 0;
  node_number = 0;
  start_point[0] = home_point[0]; 
  start_point[1] = home_point[1]; 
  target_point[0] = node_coords[node_number*2];
  target_point[1] = node_coords[node_number*2 + 1];
  moving_distance = distance_to_go[node_number];
  moving_angle = angles_at_nodes[node_number];
}

void loop()
{
  // KEEP TRACK OF LOOP ITERATIONS
  loop_counter = loop_counter + 1;
    
  if (usingAutoHNRPVT && (myGNSS.getHNRPVT() == true)) // If setAutoHNRPVT was successful and new data is available
  {
    Serial.print(F("ns: ")); // Print selected data
    Serial.print(myGNSS.packetUBXHNRPVT->data.nano);
    Serial.print(F(" Lat: "));
    Serial.print(myGNSS.packetUBXHNRPVT->data.lat);
    current_pos[0] = myGNSS.packetUBXHNRPVT->data.lat;
    
    Serial.print(F(" Lon: "));
    Serial.println(myGNSS.packetUBXHNRPVT->data.lon);
    current_pos[1] = myGNSS.packetUBXHNRPVT->data.lon;
    checkNode(current_pos);
    
    myGNSS.flushHNRPVT(); // Mark data as stale
  }

}
