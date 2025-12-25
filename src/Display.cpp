#include"Display.h"

Display::Display(int sdaPin, int sclPin, int width, int height, int resetPin, uint8_t address) : display(width,height,&Wire,resetPin)
{
    _address=address;
    lastOled=0;
    Wire.begin(sdaPin,sclPin);
}

Display::~Display()
{
}

bool Display::begin()
{
    if(!display.begin(SSD1306_SWITCHCAPVCC,_address)){
        Serial.println("SSD1306 allocation failed.");
        return false;
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2.5);
    display.setCursor(0,0);
    return true;
}

void Display::displayWeight(long weight)
{
    unsigned long now=millis();
    if(now-lastOled>=REFRESH_INTERVAL){
        lastOled=now;

        Serial.print("weight: ");
        Serial.print(weight);
        Serial.println("g");

        display.clearDisplay();
        display.setCursor(0,0);
        display.print(weight);
        display.print("g");
        display.display();
    }
}

void Display::displayMsg(String msg)
{
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(msg);
    display.display();
}

void Display::displayClr()
{
    display.clearDisplay();
}
