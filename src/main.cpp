#include <Arduino.h>
#include<HX711.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include <Wire.h>

//pin declarations and new objects
//hx711
#define LOADCELL_SCK_PIN 18
#define LOADCELL_DOUT_PIN 19
HX711 scale;

//oled
#define OLED_SDA 21
#define OLED_SCL 22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1 //we dont't do reset on i2c, so the domain value is -1
#define SCREEN_ADDRESS 0X3C
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

//stabilization
const float EMA_ALPHA=0.30f; //EMA smoothing

//display stabilization
const float DISPLAY_STEP=1.0f; //rounding step to units
const float DISPLAY_HYST=0.5f; //prevents flashing between two adjacent numbers
const int MED_N=5;
float medBuf[MED_N]={0};
int medIndex=0;
int medCount=0;
bool emaInit=false;
float gEMA=0.0f; //current value after EMA filter
long lastShown=0;

//OLED refresh
unsigned long lastOled=0;
const unsigned long OLED_REFRESH=100; //in ms 

//button and diode
#define LED_PIN 5
#define BUTTON_PIN 23
#define LONG_PRESS_TIME 3000

bool buttonState=HIGH;
bool lastButtonState=HIGH;
unsigned long pressStartMonitoringTime=0;
bool actionExecuted=false;

//function declarations hx711
float medianFromBuffer(const float*buf,int n);
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
  //scale
  scale.begin(LOADCELL_DOUT_PIN,LOADCELL_SCK_PIN);
  scale.tare(10); //to remove the weight of the stand
  Serial.println("poloz ciezar:");
  delay(5000);

  float reading=scale.get_units(10);
  float scale_factor=reading/200.0;
  Serial.print("calculated: ");
  Serial.println(scale_factor);
  scale.set_scale(scale_factor);

  //reset
  emaInit=false;
  medIndex=0;
  medCount=0;
  lastShown=0;
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
    actionExecuted=true;
    scale.tare(); //just to check
    //reset
    emaInit=false;
    medIndex=0;
    medCount=0;
    lastShown=0;
  }

  lastButtonState=buttonState;

  //measurement and stabilization
  if(scale.is_ready()){
    float gRaw=scale.get_units(1); //quick sample

    if(!isfinite(gRaw))
      gRaw==0.0f;

    //median of 5 samples
    medBuf[medIndex++]=gRaw;
    if(medIndex>=MED_N)
      medIndex=0;
    if(medCount<MED_N)
      medCount++;
    float gMed=medianFromBuffer(medBuf,medCount);

    //EMA smoothing
    if(!emaInit){
      gEMA=gMed;
      emaInit=true;
    }else
      gEMA=EMA_ALPHA*gMed+(1.0f-EMA_ALPHA)*gEMA;
    
    //rounding
    float gQuant=roundf(gEMA/DISPLAY_STEP)*DISPLAY_STEP;
    long weight=round(gQuant);

    //display hysteresis
    if(abs(weight-lastShown)>DISPLAY_HYST)
      lastShown=weight;
  }

  //display
  unsigned long now=millis();
  if(now-lastOled>=OLED_REFRESH){
    lastOled=now;

    display.clearDisplay();
    display.setCursor(0,0);

    long weigthToShow=(lastShown<0) ? 0 : lastShown;

    Serial.print("weight: ");
    Serial.print(weigthToShow);
    Serial.println("g");

    display.print(weigthToShow);
    display.print("g");
    display.display();
  }

  delay(5);
}

//function definitions hx711
float medianFromBuffer(const float* buf,int n){
  float tmp[MED_N];
  for(int i=0;i<n;i++)
    tmp[i]=buf[i];

  //insertion sort
  for(int i=1;i<n;i++){
    float element=tmp[i];
    int j=i-1;
    while(j>=0 && tmp[j]>element){
      tmp[j+1]=tmp[j];
      j--;
    }
    tmp[j+1]=element;
  }

  if(n==0)
    return 0.0f;

  return (n%2==0) ? (tmp[n/2-1]+tmp[n/2])/2 : tmp[n/2];
}

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