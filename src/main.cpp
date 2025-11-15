#include <Arduino.h>
#include<HX711.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include <Wire.h>

//pin declarations and new objects
//hx711
#define LOADCELL_SCK_PIN 18
#define LOADCELL_DOUT_PIN 19
HX711 loadcell;

//oled
#define OLED_SDA 21
#define OLED_SCL 22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1 //we dont't do reset on i2c, so the domain value is -1
#define SCREEN_ADDRESS 0X3C
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

//button and diode
#define LED_PIN 5
#define BUTTON_PIN 23
#define LONG_PRESS_TIME 3000

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
  if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)){
    Serial.println("SSD1306 allocation failed.");
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2.5);
  display.setCursor(0,0);
  //loadcell
  loadcell.begin(LOADCELL_DOUT_PIN,LOADCELL_SCK_PIN);
  loadcell.set_scale();
  loadcell.tare(); //to remove the weight of the stand
  Serial.println("poloz ciezar:");
  delay(5000);

  float reading=loadcell.get_units(10);
  float scale_factor=reading/200.0;
  Serial.print("calculated: ");
  Serial.println(scale_factor);
  loadcell.set_scale(scale_factor);
}

void loop() {
  if(loadcell.is_ready()){
    display.clearDisplay();
    display.setCursor(0,0);

    float raw_weight=loadcell.get_units();
    // use 'long' for the rounded weight. Although 'int' is 32-bit on ESP32, 
    // 'long' is used for safety and compatibility with the HX711 library's large
    // raw values (up to 16 million) from its internal 24-bit ADC.
    long weight=round(raw_weight);
    if(weight<0)
      weight=0;

    Serial.print("weight: ");
    Serial.print(weight);
    Serial.println(" g");

    display.print(weight);
    display.print(" g");
    display.display();
  }

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
    actionExecuted=true;
    loadcell.tare();
  }

  lastButtonState=buttonState;
}

//function definitions hx711
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