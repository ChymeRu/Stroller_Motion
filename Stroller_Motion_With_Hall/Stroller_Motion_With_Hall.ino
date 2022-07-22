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

static const int convert[] = {12, 0, 2, 1, 4, 5, 3, 12};
//  [6] = 3;
//  [5] = 5;
//  [4] = 4;
//  [3] = 1;
//  [2] = 2;
//  [1] = 0;
//}

int count = 0;
double rotations = 0.0;

bool prevY = false;
bool prevG = false;
bool prevW = false;

//true is forward false is reverse
bool currentDirection = true;

int prevRotPos = 0;

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
  
  Serial.print("RotPos: ");
  Serial.print(-20*prevRotPos);
  Serial.print(",");
  Serial.print("Direction: ");
  Serial.print(200*currentDirection);
  Serial.print(",");
  Serial.print("YellowP:");
  Serial.print(150*prevY);
  Serial.print(",");
  Serial.print("GreenP:");
  Serial.print(150*prevG);
  Serial.print(",");
  Serial.print("WhiteP:");
  Serial.print(150*prevW);
  Serial.print(",");
  Serial.print("rotations:");
  Serial.print(rotations);
  Serial.print(",");
  Serial.print("count:");
  Serial.print(count);
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
//  noInterrupts();
  bool activeY = digitalRead(yPin);;
  upSens(activeY, prevG, prevW);
  prevY = activeY;
//  interrupts();
}

void updateG() {
//  noInterrupts();
  bool activeG = digitalRead(gPin);;
  upSens(prevY, activeG, prevW);
  prevG = activeG;
//  interrupts();
}

void updateW() {
//  noInterrupts();
  bool activeW = digitalRead(wPin);;
  upSens(prevY, prevG, activeW);
  prevW = activeW;
//  interrupts();
}

void upSens(bool activeY, bool activeG, bool activeW) {
  int num = (4*((int)activeY)) + (2*((int)activeG)) + ((int)activeW);
  int rotPos = convert[num];
//  int rotPos = getRotPos(activeY, activeG, activeW);
  currentDirection = checkDirection(rotPos);
  rotations = (double)count/90;
  prevRotPos = rotPos;
}
//int rot = (-2*activeY)

int getRotPos(bool activeY, bool activeG, bool activeW){
  if(activeY){
    if(activeG){
      return 3;
    }else{
      if(activeW){
        return 5;
      }else{
        return 4;
      }
    }
  }else{
    if(activeG){
      if(activeW){
        return 1;
      }else{
        return 2;
      }
    }else{
      if(activeW){
        return 0;
      }else{
        return 12;
      }
    }
  }
}

bool checkDirection(int rotPos){
  int num = rotPos-prevRotPos;
  num = (num < 0) ? num + 6 : num;
//  Serial.print("Num: ");
//  Serial.print(-50*num);
//  Serial.print(",");
  
  if((num == 0) || (num == 3) || (rotPos + prevRotPos >= 12)){
    return currentDirection;
  }else if(num<3){
    count += num;
    return true;
  }else {
    count -= num-4;
    return false;
  }
}
