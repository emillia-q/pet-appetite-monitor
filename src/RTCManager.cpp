#include "RTCManager.h"

int RTCManager::getYear()
{
    return timeinfo.tm_year+1900;
}

int RTCManager::getMonth()
{
    return timeinfo.tm_mon+1;
}

int RTCManager::getDay()
{
    return timeinfo.tm_mday;
}

int RTCManager::getHour()
{
    return timeinfo.tm_hour;
}

int RTCManager::getMinute()
{
    return timeinfo.tm_min;
}

RTCManager::RTCManager()
{
    ntpServer="pool.ntp.org";
    gmtOffset_sec=3600; //1h
    daylightOffset_sec=3600;
}

RTCManager::~RTCManager()
{
}

String RTCManager::getDate()
{
    configTime(gmtOffset_sec,daylightOffset_sec,ntpServer);
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time.");
        //return;
    }

    char buff[11];
    sprintf(buff,"%d.%02d.%02d",getYear(),getMonth(),getDay());
    return buff;
}

String RTCManager::getTime()
{
    configTime(gmtOffset_sec,daylightOffset_sec,ntpServer);
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time.");
        //return;
    }

    char buff[6];
    sprintf(buff,"%02d:%02d",getHour(),getMinute());
    Serial.println(buff);
    return buff;
}
