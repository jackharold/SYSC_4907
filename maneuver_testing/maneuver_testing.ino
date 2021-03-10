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

void setup() {
  // setup code here, to run once:
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
}

//=================
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
//=================

void left30() {
  for(angle = 1175; angle >=(1350/180)*60+500;angle -=1){ // Turn 30 degrees to the right
  servo.writeMicroseconds(angle);
  delay(5);
  }
  delay(2000);
  for(angle = (1350/180)*60+500; angle<= 1175; angle +=1){ //return servo to straight (90 degrees)
    servo.writeMicroseconds(angle); 
  delay(5);
  }
}

void right30() {
  for(angle = 1175; angle <=(1350/180)*120+500;angle +=1){ // Turn 30 degrees to the left
  servo.writeMicroseconds(angle);
  delay(5);
  }
  delay(2000);
  for(angle = (1350/180)*120+500; angle>= 1175; angle -=1){ //return servo to straight (90 degrees)
    servo.writeMicroseconds(angle); 
  delay(5);
  }
}

void steady90() {
  servo.writeMicroseconds(1175);
}

void right(double degree) {
  for(angle = 1175; angle <=(1350/180)*(90+degree)+500;angle +=1){ // Turn some degrees to the right
  servo.writeMicroseconds(angle);
  delay(5);
  }
  delay(2000);
  for(angle = (1350/180)*(90+degree)+500; angle>=  1175; angle -=1){ //return servo to straight (90 degrees)
    servo.writeMicroseconds(angle); 
  delay(5);
  }
}

void left(double degree) {
  for(angle = 1175; angle >=(1350/180)*(90-degree)+500;angle -=1){ // Turn some degrees to the right
  servo.writeMicroseconds(angle);
  delay(5);
  }
  delay(2000);
  for(angle = (1350/180)*(90-degree)+500; angle<= 1175; angle +=1){ //return servo to straight (90 degrees)
    servo.writeMicroseconds(angle); 
  delay(5);
  }
}

void repositionLeft() {
  left30();
  delay(1000);
  drive();
  delay(5000);
  cease();
  steady90();
  delay(1000);
}

void repositionRight() {
  right30();
  delay(1000);
  drive();
  delay(5000);
  cease();
  steady90();
  delay(1000);
}

void rotateLeft() {
  left(45);
  drive();
  delay(400);
  cease();
  steady90();
}

void rotateRight() {
  left(45);
  drive();
  delay(400);
  cease();
  steady90();
}


void loop() {
  if (Serial.available() > 0){
    String value = Serial.readStringUntil('\n');
    value.trim();
    Serial.print("Arduino recieved: ");
    Serial.println(value);
    if (value == "go"){
      steady90();
      drive();
    }
    else if (value == "reposition right"){
      repositionRight();
      delay(1000);
      drive();
    }
    else if (value == "reposition left"){
      repositionLeft();
      delay(1000);
      drive();
    }
    else if (value == "stop"){
      cease();
    }
  }
}
