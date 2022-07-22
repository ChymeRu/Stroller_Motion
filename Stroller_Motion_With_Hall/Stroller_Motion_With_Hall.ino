// the number of the LED pin
const int ledPin = 16;

//hall effect pins
const int yPin = 25;
const int gPin = 26;
const int wPin = 27;

// Setting PWM properties
const int freq = 1000;
const int ledChannel = 0;
const int resolution = 8;


static const int convert[] = {12, 0, 2, 1, 4, 5, 3, 12};

int count = 0;
double rotations = 0.0;
double distPerRotation = 0.492; //meters
double distance = 0; //meters

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

void setup() {
  Serial.begin(115200);
  Serial.println("PWM Control");
  ledcSetup(ledChannel, freq, resolution);

  ledcAttachPin(ledPin, ledChannel);

  pinMode(yPin, INPUT);
  pinMode(gPin, INPUT);
  pinMode(wPin, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(yPin), updateY, CHANGE);
  attachInterrupt(digitalPinToInterrupt(gPin), updateG, CHANGE);
  attachInterrupt(digitalPinToInterrupt(wPin), updateW, CHANGE);
}

void loop() {
  int buff = 0;

  if ((micros() - prevTime) > 100000) RPM = 0;
  while (Serial.available() > 0){
    int x = Serial.read() - 48;

    if(x == -35 || x == -38){
//      Serial.print("You send: ");
//      Serial.print(buff);
//      Serial.println();
      ledcWrite(ledChannel, buff);
      buff = 0;
      x = 0;
    }else{
      buff = buff*10 + x;
    }
    delay(5);
  }
  
//  Serial.print("RotPos: ");
//  Serial.print(-20*prevRotPos);
//  Serial.print(",");
//  Serial.print("Direction: ");
//  Serial.print(200*currentDirection);
//  Serial.print(",");
//  Serial.print("YellowP:");
//  Serial.print(150*prevY);
//  Serial.print(",");
//  Serial.print("GreenP:");
//  Serial.print(150*prevG);
//  Serial.print(",");
//  Serial.print("WhiteP:");
//  Serial.print(150*prevW);
//  Serial.print(",");
  Serial.print("RPM:");
  Serial.print(RPM);
  Serial.print(",");
  Serial.print("PPM:");
  Serial.print(PPM);
  Serial.print(",");
  Serial.print("count:");
  Serial.print(count);
  Serial.print(",");
  Serial.print("rotations:");
  Serial.print(rotations);
  Serial.print(",");
  Serial.print("distance:");
  Serial.print(distance);
  Serial.print(",");
  Serial.print("AvTime:");
  Serial.print(AvPulseTime);
  Serial.print(",");
  Serial.print("YellowActive:");
  Serial.print(1000*activeY);
  Serial.print(",");
  Serial.print("GreenActive:");
  Serial.print(1000*activeG);
  Serial.print(",");
  Serial.print("WhiteActive:");
  Serial.print(1000*activeW);
  Serial.println();
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
