#include <math.h>

//const int gainPin = 1;
const int leftPin = 2;
const int rightPin = 1;
double leftAoA = 0.0;
double rightAoA = 0.0;
//Finding minimum and maximum digital values according to min & max voltages that come from phase detector

int leftMinVal = 15; //(1024/5)*p Volt, theretical minimum is 0.0 Volt
int leftMaxVal = 428; //(1024/5)*q Volt, theoretical maximum is 1.8 Volt

int rightMinVal = 15; //(1024/5)*0.260 Volt, theretical minimum is 0.0 Volt
int rightMaxVal = 428; //(1024/5)*2.148 Volt, theoretical maximum is 1.8 Volt

double left;
double leftVolt;
double right;
double rightVolt;
double x = 0.0;

void setup(){
 Serial.begin(9600);
}

void printL(){//rear
  //read the analog values from phase detector
  int leftRead = analogRead(leftPin);
  leftVolt = leftRead*0.0048828125; //multiplying by quantized voltage unit to obtain analog value
  double leftAng = map(leftRead, leftMinVal, leftMaxVal, 180.0, 0.0);
  double leftAngRad = leftAng/57.29578;
  //leftAoA = 135 - (57.29578)*asin(leftAngRad/3.14159265);
  leftAoA = (57.29578)*asin(leftAngRad/(2.414*3.14159265));
  //Serial.print("Digital value Left:");
  //Serial.println(leftRead);
  //Serial.print("Voltage Left: ");
  //Serial.println(leftVolt);
  //Serial.print("Phase Difference Left: ");
  //Serial.println(leftAng);
  Serial.print("Angle of Arrival Left: ");
  Serial.println(leftAoA);
  Serial.println();
 
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
  Serial.print("Angle of Arrival Right: ");
  Serial.println(rightAoA);
  //Serial.println();
}

void loop(){
  printR();
  //printL(); 
  delay (1000);
}
