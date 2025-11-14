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
#define OLED_RESET -1 //we dont't use it on i2c so the domain value is -1
#define SCREEN_ADDRESS 0X3C
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

//function declarations hx711


void setup() {
  Serial.begin(115200);

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
  Serial.print("przekalkulowane: ");
  Serial.println(scale_factor);
  loadcell.set_scale(scale_factor);
}

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);

  float raw_weight=loadcell.get_units(50);
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
  delay(500);
}

//function definitions hx711
