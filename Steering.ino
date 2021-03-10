#include <Servo.h>

Servo servo;
int angle = 0;

void setup() {
  // setup code here, to run once:
  servo.attach(9);
  Serial.begin(9600);
 servo.writeMicroseconds(1175);//0 to 180 map to 500 to 1850; hence 90 map to 1175
}

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

void loop() {
  //steady90();
  delay(2000);
  //left30();
  //right30();
  left(40);
  delay(2000);
  right(40);
}
