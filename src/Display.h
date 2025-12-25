#pragma once
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include <Wire.h>

class Display{
public:
    static constexpr unsigned long REFRESH_INTERVAL = 100; // ms

    //constructors & destructors
    Display(int sdaPin,int sclPin,int width,int height,int resetPin,uint8_t address);
    ~Display();

    bool begin();
    void displayWeight(long weight);
    void displayMsg(String msg);
    void displayClr();
private:
    Adafruit_SSD1306 display;
    unsigned long lastOled;
    uint8_t _address;
};