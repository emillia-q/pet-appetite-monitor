#pragma once
#include<Arduino.h>

class Button{
    int _buttonPin;
    bool buttonState;
    bool lastButtonState;
    bool actionExecuted;
    unsigned long pressStartMonitoringTime;
public:
    static constexpr int  LONG_PRESS_TIME=3000;

    //constructors & destructors
    Button(int buttonPin);
    ~Button();

    void measurePressTime();
    void buttonStateRead();
    void changeLastState();
    bool buttonClick();
    bool buttonHold();
};