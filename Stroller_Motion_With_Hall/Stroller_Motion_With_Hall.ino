// the number of the LED pin
const int ledPin = 16;

TaskHandle_t Task1;

//hall effect pins
const int yPin = 25;
const int gPin = 26;
const int wPin = 27;

// Setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

int buff = 0;

static const int convert[] = {12, 0, 2, 1, 4, 5, 3, 12};

int count = 0;
double rotations = 0.0;

bool activeY = digitalRead(yPin);
bool activeG = digitalRead(gPin);
bool activeW = digitalRead(wPin);

float pulseTimeY;
float pulseTimeG;
float pulseTimeW; 

//true is forward false is reverse
int currentDirection = 1;
int prevRotPos = 0;
float startTime;
float prevTime = millis(); 
float AvPulseTime; 
float PPM;
float RPM;

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
  if ((millis() - prevTime) > 100) RPM = 0;
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
  Serial.print("count:");
  Serial.print(count);
  Serial.print(",");
  Serial.print("rotations:");
  Serial.print((float)count/90);
  
//
//  bool activeY = digitalRead(yPin);
//  bool activeG = digitalRead(gPin);
//  bool activeW = digitalRead(wPin);
//  Serial.print("YellowActive:");
//  Serial.print(1000*activeY);
//  Serial.print(",");
//  Serial.print("GreenActive:");
//  Serial.print(1000*activeG);
//  Serial.print(",");
//  Serial.print("WhiteActive:");
//  Serial.print(1000*activeW);
  Serial.println();
}

void updateY() {
  startTime = millis();  
  activeY = digitalRead(yPin);
  activeW = digitalRead(yPin);
  currentDirection = (activeY == activeW) ? 1 : -1;
  count = count + (1*currentDirection);
  pulseTimeY = startTime - prevTime;
  AvPulseTime = ((pulseTimeY + pulseTimeG + pulseTimeW)/3);
  PPM = (1000 / AvPulseTime) * 60;
  RPM = PPM / 90;
  prevTime = startTime;  
}

void updateG() {
  startTime = millis();  
  activeY = digitalRead(yPin);
  activeG = digitalRead(gPin);
  currentDirection = (activeY == activeG) ? 1 : -1;
  count = count + (1*currentDirection);
  pulseTimeG = startTime - prevTime;
  AvPulseTime = ((pulseTimeY + pulseTimeG + pulseTimeW)/3);
  PPM = (1000 / AvPulseTime) * 60;
  RPM = PPM / 90;
  prevTime = startTime;  
}

void updateW() {
  startTime = millis();  
  activeG = digitalRead(gPin);
  activeW = digitalRead(wPin);
  currentDirection = (activeG == activeW) ? 1 : -1;
  count = count + (1*currentDirection);
  pulseTimeW = startTime - prevTime;
  AvPulseTime = ((pulseTimeY + pulseTimeG + pulseTimeW)/3);
  PPM = (1000 / AvPulseTime) * 60;
  RPM = PPM / 90;
  prevTime = startTime;  
}
