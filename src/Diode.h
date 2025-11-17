#pragma once
#include<Arduino.h>

class Diode{
    int _ledPin;
public:
    //constructors & destructors
    Diode(int ledPin);
    ~Diode();

    void tareMsg();
    void startMonitoringMsg();
};