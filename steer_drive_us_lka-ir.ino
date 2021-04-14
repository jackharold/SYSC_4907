#include <Servo.h>
#include <math.h>

Servo servo;
int angle = 0;
boolean leftDone=0;
boolean rightDone=0;
boolean doneL=0;
boolean doneR=0;
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
//Trig to Arduino Pin 11, Echo to Arduino pins 2, 12, 13, A0, A1 & A2
 
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
//=================================
const int rightPin = 5;
double leftAoA = 0.0;
double rightAoA = 0.0;
//Finding minimum and maximum digital values according to min & max voltages that come from phase detector
int rightMinVal = 15; //(1024/5)*0.260 Volt, theretical minimum is 0.0 Volt
int rightMaxVal = 428; //(1024/5)*2.148 Volt, theoretical maximum is 1.8 Volt
//double rightA;
double rightVolt;
//=================================
void setup() {
  // setup code here, to run once:
  servo.writeMicroseconds(1305); //640-1970 range, middle at 1305 for 90deg
  servo.attach(10);
  Serial.begin(9600);
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
void straight() {
  servo.writeMicroseconds(1305);
}
void rapidR(double deg) {
  if (doneR == 0){
  servo.writeMicroseconds((1330/180)*(90+deg)+640);
  doneR=1;
}
}
void rapidL(double deg) {
  if (doneL == 0){
  servo.writeMicroseconds((1350/180)*(90-deg)+640);
  doneL=1;
}
}

void right(double degree, double rate) { //rate determines how rapidly the turn needed in ms degree/ms: 3<= range <=10
  //servo.writeMicroseconds(1305);
  rightDone = 0;
  for(angle = 1305; angle <=(1330/180)*(90+degree)+640;angle +=1){ // Turn some degrees to the right
  servo.writeMicroseconds(angle);
  delay(rate);
  }
  rightDone = 1;
}
void rightCure(double degree, double rate){//rate determines how rapidly the recovery needed in ms degree/ms: 3<= range <=10
  if (rightDone == 1){
    for(angle =  (1330/180)*(90+degree)+640; angle >=1305;angle -=1){ 
    servo.writeMicroseconds(angle);
    delay(rate);
    }
  //servo.writeMicroseconds(1305); 
  rightDone = 0;
}
}
void left(double degree, double rate) {//rate determines how rapidly the turn needed in ms degree/ms: 3<= range <=10
  servo.writeMicroseconds(1305);
  for(angle = 1305; angle >=(1350/180)*(90-degree)+640;angle -=1){ 
  servo.writeMicroseconds(angle);
  delay(rate);
  }
}

void leftCure(double degree, double rate){ //rate determines how rapidly the recovery needed in ms degree/ms: 3<= range <=10
  for(angle = (1350/180)*(90-degree)+640; angle<= 1305; angle +=1){ //return servo to straight (90 degrees)
    servo.writeMicroseconds(angle); 
  delay(rate);
  }
  //servo.writeMicroseconds(1305); // rapid straighten, to be safe
}

//======================
boolean isFLdetected(){ //Front Left sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  durFL = pulseIn(echoPinFL, HIGH);
  distFL = (durFL / 2) * 0.0343;
  Serial.print("Distance FL = ");
  if (distFL <= 18) {
     Serial.print(distFL);
     Serial.println(" cm");
     Serial.println("FL - Obstacle detected within 18cm");
     return true;
     }
     else {
    Serial.println("FL - No Obstacle detected within 18cm");
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
  Serial.print("Distance FM = ");
  if (distFM <= 18) {
     Serial.print(distFM);
     Serial.println(" cm");
     Serial.println("FM - Obstacle detected within 18cm");
     return true;
     }
     else {
    Serial.println("FM - No Obstacle detected within 18cm");
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
  Serial.print("Distance FR = ");
  if (distFR <= 18) {
     Serial.print(distFR);
     Serial.println(" cm");
     Serial.println("FR - Obstacle detected within 18cm");
     return true;
     }
     else {
    Serial.println("FR - No Obstacle detected within 18cm");
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
  Serial.print("Distance RL = ");
  if (distRL <= 18) {
     Serial.print(distRL);
     Serial.println(" cm");
     Serial.println("RL - Obstacle detected within 18cm");
     return true;
     }
     else {
    Serial.println("RL - No Obstacle detected within 18cm");
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
  Serial.print("Distance RM = ");
  if (distRM <= 18) {
     Serial.print(distRM);
     Serial.println(" cm");
     Serial.println("RM - Obstacle detected within 18cm");
     return true;
     }
     else {
    Serial.println("RM - No Obstacle detected within 18cm");
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
  Serial.print("Distance RR = ");
  if (distRR <= 18) {
     Serial.print(distRR);
     Serial.println(" cm");
     Serial.println("RR - Obstacle detected within 18cm");
     return true;
     }
     else {
    Serial.println("RR - No Obstacle detected within 18cm");
    return false;
    delay(5);
    }
  delay(5);
}

boolean leftLKAstatus(){ 
  if (digitalRead(leftLKA) == LOW){
    Serial.println("Left lane marking detected, move to the right");
    return true;
  }
  else {
    Serial.println("Car within the left lane marking");
    return false;
    }
}

boolean rightLKAstatus(){ 
  if (digitalRead(rightLKA) == LOW){
    Serial.println("Right lane marking detected, move to the left");
    return true;
  }
  else {
    Serial.println("Car within the right lane marking");
    return false;
    }
}

//======================

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
  Serial.print("Angle of Arrival Right: ");
  Serial.println(rightAoA);
  //Serial.println();
}
//======================

void loop() {
    //drive();
    //straight();
    if (leftLKAstatus()){
       //right(30,4);
       rapidR(30);
      //delay(2000);
      }
      else{
      rightCure(30,4);
      }
    
  /*  if (rightLKAstatus()){
       left(30,4);
        delay(2000);
        }
        else{
        straight();
        }
*/    
//    if (isFMdetected()||(isFLdetected()&&isFMdetected())||(isFRdetected()&&isFMdetected())){
//      cease();
//        delay(2000);
//    }
//      if (isFLdetected()){
//        right(40,4);
//      delay(2000);
//        
//      }
//      rightCure(40,4);
//    
//     if (isFRdetected()){
//        left(40,4);
//        delay(2000);        
//      }
//      leftCure(40,4);

}
