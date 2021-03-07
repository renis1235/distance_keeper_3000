
#include <UTFT.h>

extern uint8_t SmallFont[];
extern uint8_t BigFont[];

UTFT myGLCD(ILI9325D_16,PD11,PD5,PD7,PC1);

//Pins
const byte LED = PA2;
const byte trigPin = PD12;
const byte echoPin = PD10;
const byte motion = PD8;
const byte buzzer = PA4;

long duration;
int distance;
bool changed = true;
bool warning = false;
bool oldWarning = false;
bool danger = false;
bool oldDanger = false;
int motionState = 0;


void setup() {
  randomSeed(analogRead(0));

  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(VGA_BLACK);

  pinMode(LED,OUTPUT);
  pinMode(buzzer,OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  
  pinMode(echoPin, INPUT); 
  pinMode(motion,INPUT);
  
  loadingScreen(30);
  
  Serial.begin(9600); // Starts the serial communication
  digitalWrite(LED, HIGH);
  
  myGLCD.setFont(SmallFont);
}

void loop() {

  displayUpdate();
  measureDistance();
  checkPIR();
delay(100);
  CheckForWarningOrDanger();
}

void displayUpdate(){

 if(changed)
 {
    changed = false;
    myGLCD.fillScr(VGA_BLACK);
    
   if(warning == true)
    {
      myGLCD.setColor(255,100,0);
      myGLCD.fillCircle(160,120,120);
      myGLCD.setColor(VGA_BLACK);
      myGLCD.fillCircle(160,120,80);
    }
    else if(danger == true)
    { myGLCD.setColor(VGA_RED);
      myGLCD.fillCircle(160,120,120);
      myGLCD.setColor(VGA_BLACK);
      myGLCD.fillCircle(160,120,80);
    } 
    else 
    { myGLCD.setColor(VGA_GREEN);
      myGLCD.fillCircle(160,120,120);
      myGLCD.setColor(VGA_BLACK);
      myGLCD.fillCircle(160,120,80);
    }
  }
}
void measureDistance(){
  int val=0;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  val= duration*0.034/2;

 if(val > 300 && distance <= 300){
    changed = true;
  } else 
  if(val <= 150 && distance >150){
    changed = true;
  }else 
  if(val > 150 && val <= 300 && distance <= 150){
    changed = true; 
  }else
  if(val > 150 && val <= 300 && distance > 300){
    changed = true;
  }
  
 distance = val;
  // Prints the distance on the Serial Monitor
  myGLCD.setColor(VGA_WHITE);
  myGLCD.print("Distance: ", LEFT, 220);
  myGLCD.print("     ", LEFT, 230);
  myGLCD.printNumI(distance, LEFT, 230);
}

  void checkPIR(){
  int val = digitalRead(motion);
  if(val != motionState){
    motionState = val;
    changed = true;
  }
}

void CheckForWarningOrDanger(){
  if(changed)
  {
    changed = false;
    if(motionState == HIGH)
    {
      if(distance <= 150 && danger == false)
      {
        danger = true;
        warning = false;
        changed = true;
        digitalWrite(buzzer,HIGH);
      } 
      else if(distance > 150 && distance <= 300 && warning == false)
      {  warning = true;
        danger = false;
        changed = true;
        digitalWrite(buzzer,LOW);
      }else if(distance > 300 && (warning == true || danger == true)){
        danger = false;
        warning = false;
        changed = true;
        digitalWrite(buzzer,LOW);
      }
    }
    else
     {
      danger = false;
      warning = false;
      changed = true;
      digitalWrite(buzzer,LOW);
     }
  }   
}

void loadingScreen(int seconds){
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.fillScr(VGA_BLACK);
  for(int i=0; i< 2*seconds ;i++){
    switch (i%3){
      case 0:
        myGLCD.print("Loading.  ", CENTER, 110);
        break;
      case 1:
        myGLCD.print("Loading.. ", CENTER, 110);
        break;
      case 2:
        myGLCD.print("Loading...", CENTER, 110);
        break;
    }
    delay(500);
  }
}
