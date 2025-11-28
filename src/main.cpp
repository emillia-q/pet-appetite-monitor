#include"Scale.h"
#include"Display.h"
#include"Button.h"
#include"Diode.h"
#include"ScaleWebServer.h"
#include"secrets.h"
#include"SDLogger.h"
#include"RTCManager.h"

//wifi config
const char*SSID=SECRET_SSID;
const char*PASSWORD=SECRET_PASSWORD;

//pin declarations and new objects
//hx711
#define LOADCELL_DOUT_PIN 26
#define LOADCELL_SCK_PIN 27

//oled
#define OLED_SDA 21
#define OLED_SCL 22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1 //we dont't do reset on i2c, so the domain value is -1
#define SCREEN_ADDRESS 0X3C

//button and diode
#define LED_PIN 13
#define BUTTON_PIN 14

//SD card reader
#define SD_CS 15
#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23

//objects
Scale scale(LOADCELL_DOUT_PIN,LOADCELL_SCK_PIN);
ScaleWebServer webServer(scale);
Display display(OLED_SDA,OLED_SCL,SCREEN_WIDTH,SCREEN_HEIGHT,OLED_RESET,SCREEN_ADDRESS);
Button button(BUTTON_PIN);
Diode diode(LED_PIN);
const char* LOG_FILE_NAME = "/data_log.txt";
SDLogger sd(SD_CS,SD_SCK,SD_MOSI,SD_MISO,LOG_FILE_NAME);
RTCManager rtc;

//test
bool hold;
unsigned long pressStartMonitoringTime;

void setup() {
  Serial.begin(115200);

  sd.begin();
  //display
  if(!display.begin())
    for(;;);

  //hx711
  scale.begin();

  //connect with wifi
  WiFi.begin(SSID,PASSWORD);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected with Wi-Fi. IP: ");
  Serial.println(WiFi.localIP()); //to save!

  //start the server
  webServer.begin();

  hold=false;
  pressStartMonitoringTime=0;
}

void loop() {
  //button logic
  button.buttonStateRead();
  button.measurePressTime();

  if(button.buttonHold()){
    diode.startMonitoringMsg();
    scale.setStartWeight();
    hold=true;
    pressStartMonitoringTime=millis();
  }
  if(button.buttonClick()){
    diode.tareMsg();
    scale.tare();
  }

  button.changeLastState();


  //display weight
  long weightToDisplay=scale.getStableWeight();
  display.displayWeight(weightToDisplay);
  
    
  if(millis()-pressStartMonitoringTime>=Scale::MEASURE_TIME && hold==true){
    hold=false;
    pressStartMonitoringTime=0;
    scale.checkWeightDrop();
    Serial.println("the time has passed");
    if(scale.getDidDrop()){
      scale.setDidDrop(false);
      String weightToString=String(scale.getWeightDrop());
      sd.log(rtc.getDate(),rtc.getTime(),weightToString);
      Serial.println("log success");
    }
  }
  

  //test
  //sd.log("test pliku");
  delay(5);
}