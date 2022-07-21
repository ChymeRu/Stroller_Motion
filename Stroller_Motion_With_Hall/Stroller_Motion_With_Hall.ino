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

//const int convert[] = {
//  [6] = 3;
//  [5] = 5;
//  [4] = 4;
//  [3] = 1;
//  [2] = 2;
//  [1] = 0;
//}

int count = 0;

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



//  xTaskCreatePinnedToCore(
//                    Task1code,   /* Task function. */
//                    "Task1",     /* name of task. */
//                    10000,       /* Stack size of task */
//                    NULL,        /* parameter of the task */
//                    1,           /* priority of the task */
//                    &Task1,      /* Task handle to keep track of created task */
//                    0);          /* pin task to core 0 */                  
//  delay(500);
}

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    Serial.print("RotPos: ");
    Serial.print(-200*prevRotPos);
    Serial.print(",");
    Serial.print("Direction: ");
    Serial.print(1000*currentDirection);
//  Serial.print(",");
//  Serial.print("YellowActive:");
//  Serial.print(1500*activeY);
//  Serial.print(",");
//  Serial.print("GreenActive:");
//  Serial.print(1500*activeG);
//  Serial.print(",");
//  Serial.print("WhiteActive:");
//  Serial.print(1500*activeW);
    Serial.println();
  } 
}

void loop() {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
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

  bool activeY = digitalRead(yPin);
  bool activeG = digitalRead(gPin);
  bool activeW = digitalRead(wPin);
  

  int rotPos = getRotPos(activeY, activeG, activeW);

  currentDirection = checkDirection(rotPos);
  
  Serial.print("RotPos: ");
  Serial.print(-200*prevRotPos);
  Serial.print(",");
  Serial.print("Direction: ");
  Serial.print(1000*currentDirection);
//  Serial.print(",");
//  Serial.print("YellowActive:");
//  Serial.print(1500*activeY);
//  Serial.print(",");
//  Serial.print("GreenActive:");
//  Serial.print(1500*activeG);
//  Serial.print(",");
//  Serial.print("WhiteActive:");
//  Serial.print(1500*activeW);
  Serial.println();
  
  prevY = activeY;
  prevG = activeG;
  prevW = activeW;
  prevRotPos = rotPos;
//  delay(5);
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
  if(num == 0 || num == 3){
    return currentDirection;
  }else if(num<3){
    return true;
  }else {
    return false;
  }
}
