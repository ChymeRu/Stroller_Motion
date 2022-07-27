#include <PID_v1.h>

// the number of the LED pin
const int motPin = 16;
const int revPin = 18;

//hall effect pins
const int yPin = 25;
const int gPin = 26;
const int wPin = 27;

// Setting PWM properties
const int freq = 1000;
const int ledChannel = 0;
const int resolution = 8;


static const int convert[] = {12, 0, 2, 1, 4, 5, 3, 12};

bool activeY = digitalRead(yPin);
bool activeG = digitalRead(gPin);
bool activeW = digitalRead(wPin);

long pulseTimeY;
long pulseTimeG;
long pulseTimeW; 

//true is forward false is reverse
int currentDirection = 1;
int prevRotPos = 0;
long startTime;
long prevTime = micros(); 
double AvPulseTime; 
double PPM;
double RPM;
int count = 0;
double rotations = 0.0;
double distPerRotation = 0.492; //meters
double distance = 0; //meters

//PID
double Setpoint, Output;
double kP = 150;
double kI = 0;
double kD = 0;

PID myPID(&distance, &Output, &Setpoint, kP, kI, kD, P_ON_E, DIRECT);

void setup() {
  Serial.begin(115200);
  Serial.println("PWM Control");
  ledcSetup(ledChannel, freq, resolution);

  ledcAttachPin(motPin, ledChannel);

  pinMode(revPin, OUTPUT);
  pinMode(yPin, INPUT);
  pinMode(gPin, INPUT);
  pinMode(wPin, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(yPin), updateY, CHANGE);
  attachInterrupt(digitalPinToInterrupt(gPin), updateG, CHANGE);
  attachInterrupt(digitalPinToInterrupt(wPin), updateW, CHANGE);

  Setpoint = 0;
  myPID.SetOutputLimits(-255, 255);
  myPID.SetMode(AUTOMATIC);
//  if(abs(distance) > abs(Setpoint)){
//    Setpoint = -1 * Setpoint;
//  }
}

void loop() {
  int buff = 0;
  bool negative = false;
  if ((micros() - prevTime) > 100000) RPM = 0;
  while (Serial.available() > 0){
    int x = Serial.read() - 48;
    if(x == -3){
      negative = true;
    }else if(x == -35 || x == -38){
      Serial.print("You send: ");
      if(negative) Serial.print("-");
      Serial.print(buff);
      Serial.println();
      Setpoint = (negative)? buff * -1: buff;
      buff = 0;
      x = 0;
    }else{
      buff = buff*10 + x;
    }
    delay(5);
  }
  myPID.Compute();
  controlMotor(Output);
//  Serial.print("YellowActive:");
//  Serial.print(1000*activeY);
//  Serial.print(",");
  Serial.print("SetPoint:");
  Serial.print(Setpoint);
  Serial.print(",");
  Serial.print("Output:");
  Serial.print(Output);
  Serial.print(",");
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println();
}

void controlMotor(int speed){
  bool negative = speed < 0;
  ledcWrite(ledChannel, abs(speed));
  digitalWrite(revPin, negative);
}

void updateY() {
  startTime = micros();  
  activeY = digitalRead(yPin);
  activeW = digitalRead(wPin);
  currentDirection = (activeY == activeW) ? -1 : 1;
  count = count + (1*currentDirection);
  pulseTimeY = startTime - prevTime;
  AvPulseTime = ((double)(pulseTimeY + pulseTimeG + pulseTimeW))/3.0f;
  PPM = (1000000.0f / AvPulseTime) * 60.0f;
  RPM = PPM / 90.0f;
  rotations = (double)count/90.0f;
  distance = rotations * distPerRotation;
  prevTime = startTime;  
}

void updateG() {
  startTime = micros();  
  activeY = digitalRead(yPin);
  activeG = digitalRead(gPin);
  currentDirection = (activeY == activeG) ? -1 : 1;
  count = count + (1*currentDirection);
  pulseTimeG = startTime - prevTime;
  AvPulseTime = ((double)(pulseTimeY + pulseTimeG + pulseTimeW))/3.0f;
  PPM = (1000000.0f / AvPulseTime) * 60.0f;
  RPM = PPM / 90.0f;
  rotations = (double)count/90.0f;
  distance = rotations * distPerRotation;
  prevTime = startTime;   
}

void updateW() {
  startTime = micros();  
  activeG = digitalRead(gPin);
  activeW = digitalRead(wPin);
  currentDirection = (activeG == activeW) ? -1 : 1;
  count = count + (1*currentDirection);
  pulseTimeW = startTime - prevTime;
  AvPulseTime = ((double)(pulseTimeY + pulseTimeG + pulseTimeW))/3.0f;
  PPM = (1000000.0f / AvPulseTime) * 60.0f;
  RPM = PPM / 90.0f;
  rotations = (double)count/90.0f;
  distance = rotations * distPerRotation;
  prevTime = startTime;  
}
