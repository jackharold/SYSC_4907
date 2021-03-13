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
const int rightPin = 1;
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

double right;
double rightVolt;
double x = 0.0;

boolean newData = false;

void setup() {
  servo.attach(10);
  Serial.begin(9600);
  
  servo.writeMicroseconds(1175);//0 to 180 map to 500 to 1850; hence 90 map to 1175
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
}

void printR(){//front pair
  int rightRead = analogRead(rightPin);
  rightVolt = rightRead*0.0048828125;
  double rightAng = map(rightRead, rightMinVal, rightMaxVal, 180.0, 0.0);
  double rightAngRad = rightAng/57.29578;
  rightAoA = (57.29578)*asin(rightAngRad/3.14159265);
  //Serial.print("Digital value Right:");
  //Serial.println(rightRead);
  //Serial.print("Voltage Right: ");
  //Serial.println(rightVolt);
  //Serial.print("Phase Difference Right: ");
  //Serial.println(rightAng);
  //Serial.print("Angle of Arrival Right: ");
  //Serial.println(rightAoA);
  //Serial.println();
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

void steady90() {
  servo.writeMicroseconds(1175);
}

void turnRight(double degree, int time) {
  for(angle = 1175; angle <=(1350/180)*(90+degree)+500;angle +=1){ // Turn some degrees to the right
  servo.writeMicroseconds(angle);
  delay(5);
  time = time - 10;
  }
  delay(time);
  for(angle = (1350/180)*(90+degree)+500; angle>=  1175; angle -=1){ //return servo to straight (90 degrees)
    servo.writeMicroseconds(angle); 
  delay(5);
  }
}

void turnLeft(double degree, int time) {
  for(angle = 1175; angle >=(1350/180)*(90-degree)+500;angle -=1){ // Turn some degrees to the right
  servo.writeMicroseconds(angle);
  delay(5);
  time = time - 10;
  }
  delay(time);
  for(angle = (1350/180)*(90-degree)+500; angle<= 1175; angle +=1){ //return servo to straight (90 degrees)
    servo.writeMicroseconds(angle); 
  delay(5);
  }
}

void repositionLeft() {
  drive();
  turnLeft(40, 5000);
  cease();
  steady90();
}

void repositionRight() {
  drive();
  turnRight(40, 5000);
  cease();
  steady90();
}

void rotateLeft(int degree) {
  drive();
  turnLeft(40, 25*degree);
  cease();
  steady90();
}

void rotateRight(int degree) {
  drive();
  turnRight(40, 25*degree);
  cease();
  steady90();
}
/*
void park() {
  dleft = measureDistance(TrigPin4, EchoPin5);
  dright = measureDistance(TrigPin6, EchoPin7);
  distance = measureDistance(TrigPin8, EchoPin9);
  drive();
  right(90); //Angle of arrival (AOA)
  
  delay(1000);
  while (distance > 6) {
    reverse();
  }
  if (dleft < dright){
    //cease
    //move forward
    //turn left at an angle
    //cease
    //turn right at the same angle and reverse
    
    cease();
    delay(1000);
    drive();
    left(30); //left30();
    delay(1000);
    cease();
    delay(1000);
    reverse();
    right(30); //right30();
  }
  if (dleft > dright){
    //cease
    //move forward
    //turn left at an angle and reverse
    //cease
    //turn right at the same angle
    //reverse
    
    cease();
    delay(1000);
    right(30); //right30();
    drive();
    
    delay(1000);
    cease();
    delay(1000);
    left(30); //left30();
    reverse();
    
  }
  if (distance == dleft == dright || distance < 6){
     cease();
  }
} */

boolean isFLdetected(){ //Front Left sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durFL = pulseIn(echoPinFL, HIGH);
  distFL = (durFL / 2) * 0.0343;
  //Serial.print("Distance FL = ");
  if (distFL <= 18) {
     //Serial.print(distFL);
     //Serial.println(" cm");
     //Serial.println("FL - Obstacle detected within 18cm");
     return true;
     }
     else {
    //Serial.println("FL - No Obstacle detected within 18cm");
    return false;
    delay(5);
    }
  delay(5);
}

boolean isFMdetected(){ //Front Middle sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durFM = pulseIn(echoPinFM, HIGH);
  distFM = (durFM / 2) * 0.0343;
  //Serial.print("Distance FM = ");
  if (distFM <= 18) {
     //Serial.print(distFM);
     //Serial.println(" cm");
     //Serial.println("FM - Obstacle detected within 18cm");
     return true;
     }
     else {
    //Serial.println("FM - No Obstacle detected within 18cm");
    return false;
    delay(5);
    }
  delay(5);
}

boolean isFRdetected(){ //Front Right sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durFR = pulseIn(echoPinFR, HIGH);
  distFR = (durFR / 2) * 0.0343;
  //Serial.print("Distance FR = ");
  if (distFR <= 18) {
     //Serial.print(distFR);
     //Serial.println(" cm");
     //Serial.println("FR - Obstacle detected within 18cm");
     return true;
     }
     else {
    //Serial.println("FR - No Obstacle detected within 18cm");
    return false;
    delay(5);
    }
  delay(5);
}

boolean isRLdetected(){ //Rear Left sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durRL = pulseIn(echoPinRL, HIGH);
  distRL = (durRL / 2) * 0.0343;
  //Serial.print("Distance RL = ");
  if (distRL <= 18) {
     //Serial.print(distRL);
     //Serial.println(" cm");
     //Serial.println("RL - Obstacle detected within 18cm");
     return true;
     }
     else {
    //Serial.println("RL - No Obstacle detected within 18cm");
    return false;
    delay(5);
    }
  delay(5);
}

boolean isRMdetected(){ //Rear Middle sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durRM = pulseIn(echoPinRM, HIGH);
  distRM = (durRM / 2) * 0.0343;
  //Serial.print("Distance RM = ");
  if (distRM <= 18) {
     //Serial.print(distRM);
     //Serial.println(" cm");
     //Serial.println("RM - Obstacle detected within 18cm");
     return true;
     }
     else {
    //Serial.println("RM - No Obstacle detected within 18cm");
    return false;
    delay(5);
    }
  delay(5);
}

boolean isRRdetected(){ //Rear Right sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durRR = pulseIn(echoPinRR, HIGH);
  distRR = (durRR / 2) * 0.0343;
  //Serial.print("Distance RR = ");
  if (distRR <= 18) {
     //Serial.print(distRR);
     //Serial.println(" cm");
     //Serial.println("RR - Obstacle detected within 18cm");
     return true;
     }
     else {
    //Serial.println("RR - No Obstacle detected within 18cm");
    return false;
    delay(5);
    }
  delay(5);
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
  if (command == "reposition"){
    if (commandValue == 1){
      repositionLeft();
      Serial.println("done");
    }
    else if (commandValue == 2){
      repositionRight();
      Serial.println("done");
    }
  }
  if (command == "ultrasonic"){
    if (commandValue == 1){
      if (isFLdetected() || isFMdetected() || isFRdetected()) {
        Serial.println("False");
      }
      else {
        Serial.println("True");
      }
    }
    if (commandValue == 2){
      if (isRLdetected() || isRMdetected() || isRRdetected()) {
        Serial.println("False");
      }
      else {
        Serial.println("True");
      }
    }
  }
  if (command == "rotate") {
    if (commandValue > 180){
      commandValue = 360 - commandValue;
      rotateLeft(commandValue);
      Serial.println("done");
    }
    else {
      rotateRight(commandValue);
      Serial.println("done");
    }
  }
  if (command == "drive") {
    drive();
    delay(commandValue * 500);
  }
  if (command == "infrared"){
    if (leftLKAstatus() || rightLKAstatus()){
      Serial.println("False");
    }
    else {
      Serial.println("True");
    }
  }
  if (command == "stop"){
    cease();
    Serial.println("done");
  }
  /*
  if (command == "park"){
    park();
    Serial.println("done");
  }
  */
}


void loop() {
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
