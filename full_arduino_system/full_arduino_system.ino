#include <Servo.h>

Servo servo;
int angle = 0;
//=================
// Motor A
int enA = 9;
int in1 = 8;
int in2 = 7;
// Motor B
int enB = 3;
int in3 = 5;
int in4 = 4;
//=================
const int rightPin = 5;
double rightAoA = 0.0;

#define trigPin 11
#define echoPinFL 2
#define echoPinFM 12
#define echoPinFR 13
#define echoPinRL A0
#define echoPinRM A1
#define echoPinRR A2
#define leftLKA A3 //Left Lane Keeping Assist IR
#define rightLKA A4 //Right Lane Keeping Assist IR
 
float durFL, distFL; // Front Left
float durFM, distFM; // Front Middle
float durFR, distFR; // Front Right
float durRL, distRL; //Rear Left
float durRM, distRM; //Rear Middle
float durRR, distRR; //Rear Right

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
char command[numChars] = {0};
int commandValue = 0;

int rightMinVal = 15; //(1024/5)*0.260 Volt, theretical minimum is 0.0 Volt
int rightMaxVal = 428; //(1024/5)*2.148 Volt, theoretical maximum is 1.8 Volt

double rightVolt;
double x = 0.0;

boolean newData = false;
int path = 1;

int loopnum = 0;

void setup() {
  servo.attach(10);
  Serial.begin(9600);
  
  servo.writeMicroseconds(1305);//0 to 180 map to 500 to 1850; hence 90 map to 1175
  //================
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //================
  pinMode(trigPin, OUTPUT);
  pinMode(echoPinFL, INPUT);
  pinMode(echoPinFM, INPUT);
  pinMode(echoPinFR, INPUT);
  pinMode(echoPinRL, INPUT);
  pinMode(echoPinRM, INPUT);
  pinMode(echoPinRR, INPUT);
  pinMode(leftLKA, INPUT);
  pinMode(rightLKA, INPUT);
  
  while (Serial.available() > 0 ) {
    Serial.read();
  }
}

void drive(){ 
  // Turn on motor A
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // Set speed to 100 out of possible range 0~255
  analogWrite(enA, 255);
  // Turn on motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // Set speed to 100 out of possible range 0~255
  analogWrite(enB, 255);
}

void reverse(){
  // Now change motor directions
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH); 
  analogWrite(enA, 255); 
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH); 
  analogWrite(enB, 255);
}
 
void cease(){
  // Now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void straight() {
  servo.writeMicroseconds(1305);
}

void turnRight(double degree, double rate) {
  servo.writeMicroseconds(1305);
  for (angle = 1305; angle <= (1330/180)*(90+degree)+640; angle +=1){
    servo.writeMicroseconds((int)angle);
    delay(rate);
  }
}

void turnLeft(double degree, double rate) {
  servo.writeMicroseconds(1305);
  for (angle = 1305; angle >= (1330/180)*(90-degree)+640; angle -=1){
    servo.writeMicroseconds((int)angle);
    delay(rate);
  }
}

void rightCure(double degree, double rate) {
  for (angle = (1330/180)*(90+degree)+640;  angle >= 1305; angle -=1){
    servo.writeMicroseconds((int)angle);
    delay(rate);
  }
  servo.writeMicroseconds(1305);
}
  
void leftCure(double degree, double rate) {
  for (angle = (1330/180)*(90-degree)+640;  angle <= 1305; angle +=1){
    servo.writeMicroseconds((int)angle);
    delay(rate);
  }
  servo.writeMicroseconds(1305);
}

void setWheels(){
  if (path == 1){
    turnLeft(25, 4);
  }
  else if (path == 2) {
    turnRight(25, 4);
  }    
}

void fixWheels(){
  if (path == 1){
    leftCure(25, 4);
  }
  else if (path == 2) {
    rightCure(25, 4);
  }    
}

void repositionLeft() {
  fixWheels();
  turnLeft(45,4);
  delay(10000);
  leftCure(45,4);
  setWheels();
}

void repositionRight() {
  fixWheels();
  turnRight(50,4);
  delay(10000);
  rightCure(50,4);
  setWheels();
}

void rotateLeft() {
  drive();
  turnLeft(45, 4);
  delay(15000);
  leftCure(45, 4);
  cease();
  turnRight(50,4);
  reverse();
  delay(10000);
  cease();
  rightCure(50, 4);
  drive();
  turnLeft(45, 4);
  delay(15000);
  leftCure(45, 4);
  cease();
  setWheels();
}

void rotateRight() {
  drive();
  turnRight(50, 4);
  delay(15000);
  rightCure(50, 4);
  cease;
  turnLeft(45,4);
  reverse();
  delay(10000);
  cease();
  leftCure(45, 4);
  drive();
  turnRight(50, 4);
  delay(15000);
  rightCure(50, 4);
  cease();
  setWheels();
}

void park() {
  fixWheels();
  turnLeft(45, 4);
  while(getAoA() < 35);
  leftCure(45, 4);
  while(getAoA() < 50);
  turnRight(50, 4);
  while(getAoA() > 25);
  rightCure(50, 4);
  turnRight(25, 4);
  while(getAoA() > 10);
  rightCure(25,4);
  while(!isFMdetected());
  cease(); 
}

boolean isFLdetected(){ //Front Left sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durFL = pulseIn(echoPinFL, HIGH);
  distFL = (durFL / 2) * 0.0343;
  delay(200);
  return (distFL <= 40);
}

boolean isFMdetected(){ //Front Middle sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durFM = pulseIn(echoPinFM, HIGH);
  distFM = (durFM / 2) * 0.0343;
  delay(200);
  return (distFM <= 10);
}

boolean isFRdetected(){ //Front Right sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durFR = pulseIn(echoPinFR, HIGH);
  distFR = (durFR / 2) * 0.0343;
  delay(200);
  return (distFR <= 40);
}

boolean isRLdetected(){ //Rear Left sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durRL = pulseIn(echoPinRL, HIGH);
  distRL = (durRL / 2) * 0.0343;
  delay(200);
  return (distRL <= 40);
}

boolean isRMdetected(){ //Rear Middle sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durRM = pulseIn(echoPinRM, HIGH);
  distRM = (durRM / 2) * 0.0343;
  delay(200);
  return (distRM <= 10);
}

boolean isRRdetected(){ //Rear Right sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durRR = pulseIn(echoPinRR, HIGH);
  distRR = (durRR / 2) * 0.0343;
  delay(200);
  return (distRR <= 10);
}

int getAoA(){//front pair
  int rightRead = analogRead(rightPin);
  rightVolt = rightRead*0.0048828125;
  double rightAng = map(rightRead, rightMinVal, rightMaxVal, 180.0, 0.0);
  double rightAngRad = rightAng/57.29578;
  rightAoA = (57.29578)*asin(rightAngRad/3.14159265);
  return rightAoA; 
}


boolean leftLKAstatus(){ 
  if (digitalRead(leftLKA) == LOW){
    //Serial.println("Left lane marking detected, move to the right");
    return true;
  }
  else {
    //Serial.println("Car within the left lane marking");
    return false;
  }
}

boolean rightLKAstatus(){ 
  if (digitalRead(rightLKA) == LOW){
    //Serial.println("Right lane marking detected, move to the left");
    return true;
  }
  else {
    //Serial.println("Car within the right lane marking");
    return false;
  }
}

void checkIR() {
  if (rightLKAstatus()) {
    fixWheels();
    turnLeft(45,4);
    while (rightLKAstatus()){
      drive();
    }
    delay(7000);
    leftCure(45,4);
    setWheels();
    Serial.println("True");
  } 
  else if (leftLKAstatus()){
    fixWheels();
    turnRight(50,4);
    while (leftLKAstatus()){
      drive();
    }
    delay(7000);
    rightCure(50,4);
    setWheels();
    Serial.println("True");
  }
  else {
    Serial.println("False");
  }
}

void start()
  {if (isFLdetected() && isFRdetected()){
    cease();
    Serial.println("Both sensors detected");
  }
  else if (isFRdetected()){
    path = 2;
    drive();
    turnLeft(45, 4);
    delay(25000);
    leftCure(45, 4);
    Serial.println("Left path chosen");
    setWheels();
  }
  else{
    path = 1;
    drive();
    turnRight(45, 4);
    delay(25000);
    rightCure(45, 4);
    Serial.println("Right path chosen");
    setWheels();
  }
}
void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;
  

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parseData() {      // split the data into its parts
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars,",");      // get the first part - the string
  strcpy(command, strtokIndx); // copy it to messageFromPC
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  commandValue = atoi(strtokIndx);     // convert this part to an integer
}

void doCommand() {
  if (strcmp(command,"reposition") == 0){
    if (commandValue == 1){
      repositionLeft();
      Serial.println("done reposition left");
    }
    else if (commandValue == 2){
      repositionRight();
      Serial.println("done reposition right");
    }
  }
  if (strcmp(command,"ultrasonic") == 0){
    if (isFMdetected()){
      cease();
      Serial.println("True");
    }
    else {
      Serial.println("False");
    }
  }
  if (strcmp(command,"rotate") == 0) {
    if (commandValue > 180){
      //commandValue = 360 - commandValue;
      rotateLeft();
      Serial.println("done rotate left");
    }
    else {
      rotateRight();
      Serial.println("done rotate right");
    }
  }
  if (strcmp(command,"start") == 0) {
    start();
  }
  if (strcmp(command,"drive") == 0) {
    drive();
    Serial.println("driving");
  }
  if (strcmp(command,"infrared") == 0){
    checkIR();
  }
  if (strcmp(command,"stop") == 0){
    cease();
    Serial.println("stopped");
  }
  if (strcmp(command,"park") == 0){
    park();
  }
  if (strcmp(command,"aoa") == 0){ 
    int aoa = getAoA();
    Serial.println(aoa);
  }
}



void loop(){
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    doCommand();
    newData = false;
  }
}
