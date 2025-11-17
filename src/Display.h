#pragma once
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include <Wire.h>

class Display{
public:
    static constexpr unsigned long REFRESH_INTERVAL = 100; // ms

    Display(int sdaPin,int sclPin,int width,int height,int resetPin,uint8_t address);
    ~Display();

    bool begin();
    void displayWeight(long weight);
private:
    Adafruit_SSD1306 display;
    unsigned long lastOled=0;
    uint8_t _address;
};