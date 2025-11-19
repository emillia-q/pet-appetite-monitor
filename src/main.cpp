#include"Scale.h"
#include"Display.h"
#include"Button.h"
#include"Diode.h"
#include"ScaleWebServer.h"
#include"secrets.h"

//wifi config
const char*SSID=SECRET_SSID;
const char*PASSWORD=SECRET_PASSWORD;

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

//objects
Scale scale(LOADCELL_DOUT_PIN,LOADCELL_SCK_PIN);
ScaleWebServer webServer(scale);
Display display(OLED_SDA,OLED_SCL,SCREEN_WIDTH,SCREEN_HEIGHT,OLED_RESET,SCREEN_ADDRESS);
Button button(BUTTON_PIN);
Diode diode(LED_PIN);

void setup() {
  Serial.begin(115200);

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
}

void loop() {
  //button logic
  button.buttonStateRead();
  button.measurePressTime();

  if(button.buttonHold()){
    diode.startMonitoringMsg();
  }
  if(button.buttonClick()){
    diode.tareMsg();
    scale.tare();
  }

  button.changeLastState();

  //display weight
  long weightToDisplay=scale.getStableWeight();
  display.displayWeight(weightToDisplay);

  delay(5);
}