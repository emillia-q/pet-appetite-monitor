#pragma once
#include"time.h"
#include<Arduino.h>

class RTCManager{
    //NTP server and time zone
    const char* ntpServer;
    long gmtOffset_sec;
    int daylightOffset_sec;
    struct tm timeinfo;

    int getYear();
    int getMonth();
    int getMinute();
public:
    RTCManager();
    ~RTCManager();

    int getHour(); //not private, it is needed to sync time in loop
    int getDay(); 
    void config();
    String getDate();
    String getTime();
};