#include"Scale.h"
#include"Display.h"

//pin declarations and new objects
//hx711
#define LOADCELL_SCK_PIN 18
#define LOADCELL_DOUT_PIN 19

//oled
#define OLED_SDA 21
#define OLED_SCL 22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1 //we dont't do reset on i2c, so the domain value is -1
#define SCREEN_ADDRESS 0X3C

//button and diode
#define LED_PIN 5
#define BUTTON_PIN 23
#define LONG_PRESS_TIME 3000

//new objects
Scale scale(LOADCELL_DOUT_PIN,LOADCELL_SCK_PIN);
Display display(OLED_SDA,OLED_SCL,SCREEN_WIDTH,SCREEN_HEIGHT,OLED_RESET,SCREEN_ADDRESS);

bool buttonState=HIGH;
bool lastButtonState=HIGH;
unsigned long pressStartMonitoringTime=0;
bool actionExecuted=false;

//function declarations hx711
void userTaring();
void startMonitoring();

void setup() {
  Serial.begin(115200);

  //button
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  //diode
  pinMode(LED_PIN,OUTPUT);

  //display
  if(!display.begin())
    for(;;);

  //hx711
  scale.begin(200.f);
}

void loop() {
  //button
  buttonState=digitalRead(BUTTON_PIN);
  
  if(lastButtonState==HIGH&&buttonState==LOW){
    pressStartMonitoringTime=millis();
    actionExecuted=false;
  }

  if(buttonState==LOW && !actionExecuted){
    if(millis()-pressStartMonitoringTime>=LONG_PRESS_TIME){
      startMonitoring();
      actionExecuted=true;
    }
  }
  
  if(lastButtonState==LOW && buttonState==HIGH && !actionExecuted){
    userTaring();
    scale.tare();
    actionExecuted=true;
  }

  lastButtonState=buttonState;

  //display weight
  long weightToDisplay=scale.getStableWeight();
  display.displayWeight(weightToDisplay);

  delay(5);
}

//function definitions

void userTaring(){
  digitalWrite(LED_PIN,HIGH);
  delay(1000);
  digitalWrite(LED_PIN,LOW);
}

void startMonitoring(){
    for(int i=0;i<3;i++){
      digitalWrite(LED_PIN,HIGH);
      delay(500); 
      digitalWrite(LED_PIN,LOW);
      delay(500);
    }
}