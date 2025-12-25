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
    int getDay();
    int getHour();
    int getMinute();
public:
    RTCManager();
    ~RTCManager();

    void config();
    String getDate();
    String getTime();
};