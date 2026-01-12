#include"Scale.h"
#include"Display.h"
#include"Button.h"
#include"Diode.h"
#include"ScaleWebServer.h"
#include"secrets.h"
#include"SDLogger.h"
#include"RTCManager.h"
#include"FirebaseLogger.h"

//wifi config
const char*SSID=SECRET_SSID;
const char*PASSWORD=SECRET_PASSWORD;

//firebase
const char*FB_URL=FBASE_URL;
const char*FB_SECRET=FBASE_SECRET;

//pin declarations
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
#define SD_CS 5
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
const char* BACKUP_LOG_FILE_NAME = "/backup_log.txt";
SDLogger sd(SD_CS,SD_SCK,SD_MOSI,SD_MISO,LOG_FILE_NAME,BACKUP_LOG_FILE_NAME);
RTCManager rtc;
FirebaseLogger firebase(FB_URL,FB_SECRET);

//variables and flags
bool isButtonHeld;
bool isTared;
bool isRunning;
bool isSyncWithFirebase;
bool isWiFiConnecting;
int lastDayTimeSync;
unsigned long monitoringStartTime;
unsigned long lastWiFiConnectionAttempt;
unsigned long lastActivity;

//constants
const int WEIGHT_MARGIN=2; //2g
const unsigned long WEIGHT_STABILIZE=3000; //3s
const unsigned long WIFI_PRE_CONNECT=30000; //30s
const unsigned long WIFI_RETRY_INTERVAL=10000; //10s
const unsigned long INACTIVITY_SLEEP_TIMEOUT=300000; //5min

//functions declarations
void reconnectWithWiFi();
void dataLog(long weightDrop);
void firebaseSyncCheck();
void dailyRTCSync();
void goToDeepSleep();

void setup() {
  Serial.begin(115200);

  //display
  while(!display.begin());

  //sd card
  int i=0;
  while(!sd.begin()){
    if(i>5)
      display.displayMsg("SD card error.");
    i++;
  }
  display.displayClr();

  //hx711
  scale.begin();

  //connect with wifi -> at start it is a must have
  WiFi.persistent(false); //Dont't store WiFi confing in flash
  WiFi.mode(WIFI_STA); //Set to station mode
  WiFi.disconnect(); //Disconnect from  Access Point if it was previously connected
  delay(100);
  WiFi.begin(SSID,PASSWORD);

  unsigned long startAttemptTime=millis();
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(1000);
    if(millis()-startAttemptTime>WIFI_RETRY_INTERVAL){ //Reconnection
      Serial.println("Failed to connect. Restart.");
      WiFi.disconnect();
      delay(100);
      WiFi.begin(SSID,PASSWORD);
      startAttemptTime=millis();
    }
  }
  Serial.print("Connected with Wi-Fi. IP: ");
  Serial.println(WiFi.localIP()); //NOT USED: needed for web server 

  //rtc configured right only when having wi-fi access
  rtc.config();

  //firebase
  firebase.begin();

  //disconnect with WiFi to reduce power consumption
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  isButtonHeld=false;
  isTared=false;
  isRunning=false;
  isSyncWithFirebase=false; //in case of device reset, it will check the file
  isWiFiConnecting=false;
  lastDayTimeSync=-1;
  monitoringStartTime=0;
  lastWiFiConnectionAttempt=0;
  lastActivity=millis();
}

void loop() {
  //button logic
  if(!isRunning){
    //display weight
    long weightToDisplay=scale.getStableWeight();
    display.displayWeight(weightToDisplay);

    //read button pin
    button.buttonStateRead();
    button.measurePressTime();
    
    if(button.buttonHold() && weightToDisplay>WEIGHT_MARGIN){
      diode.startMonitoringMsg();
      scale.setStartWeight();
      isRunning=true;
      monitoringStartTime=millis();
      display.displayClr();
    }

    if(button.buttonClick()){
      diode.tareMsg();
      scale.tare();
      lastActivity=millis();
      //isTared=true;
    }

    button.changeLastState(); 
  }

  if(isRunning && millis()-monitoringStartTime+WIFI_PRE_CONNECT>=Scale::MEASURE_TIME){
    reconnectWithWiFi();
    //get weight 3s before SD log so it will stabilize
    if(isRunning && millis()-monitoringStartTime+WEIGHT_STABILIZE>=Scale::MEASURE_TIME){
      long measuredWeight=scale.getStableWeight();  
      Serial.println(measuredWeight);
      if(millis()-monitoringStartTime>=Scale::MEASURE_TIME){ //if the time has passed, we check if the weight drop occured
        monitoringStartTime=millis(); //to start measuring time to the next log
        Serial.println("the time has passed");
        scale.checkWeightDrop();
        
        if(scale.getDidDrop()){ //if the weight had dropped, we log the data
          scale.setDidDrop(false);
          dataLog(scale.getWeightDrop());
          firebaseSyncCheck(); //chceck if the data are synchronized with firebase, always checks it for the first time after running a program
          scale.setStartWeight();
        }
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        if(measuredWeight<=WEIGHT_MARGIN){ //if the bowl is empty, the program stops running and is waiting for another bowl fill and start of the program
          isRunning=false;
          goToDeepSleep();
        }
      }
    }
  }

  //daily time sync at 3AM
  dailyRTCSync();

  //deep sleep when there is no activity for 5 minutes
  if(!isRunning && millis()-lastActivity>=INACTIVITY_SLEEP_TIMEOUT)
    goToDeepSleep();
  
  delay(5);
}

void reconnectWithWiFi()
{
  //check if the device has WiFi connection
  if(WiFi.status()==WL_CONNECTED){
    isWiFiConnecting=false;
    return;
  }
 
  if(!isWiFiConnecting || (millis()-lastWiFiConnectionAttempt>WIFI_RETRY_INTERVAL)){ //Reconnection
    Serial.println("Failed to connect. Restart.");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.begin(SSID,PASSWORD);
    lastWiFiConnectionAttempt=millis();
    isWiFiConnecting=true;
  }
  
}

void dataLog(long weightDrop)
{
  String weight=String(weightDrop);
  String currentDate=rtc.getDate();
  String currentTime=rtc.getTime();
  //sd log
  sd.log(currentDate,currentTime,weight);
  //firebase
  if(!firebase.logMeal(currentDate,currentTime,weight)){
    isSyncWithFirebase=false;
    sd.backupLog(currentDate,currentTime,weight);
  }
}

void firebaseSyncCheck()
{
  if(!isSyncWithFirebase && WiFi.status()==WL_CONNECTED){
    sd.syncBackupWithFirebase(firebase);
    if(sd.getSync())
      isSyncWithFirebase=true;
  }
}

void dailyRTCSync()
{
  if(rtc.getHour()==3 && rtc.getDay()!=lastDayTimeSync && WiFi.status()==WL_CONNECTED){
    Serial.println("Routine NTP sync.");
    rtc.config();
    lastDayTimeSync=rtc.getDay();
  }
}

void goToDeepSleep()
{
  Serial.println("Preparing for deep sleep.");
  //display
  display.displayOff();

  //hx711
  digitalWrite(LOADCELL_SCK_PIN,HIGH);

  //WiFi
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  //wakeup pin configuration
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,0); 

  //sleep
  esp_deep_sleep_start();
}
