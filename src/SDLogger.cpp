#include "SDLogger.h"

void SDLogger::log(const String &date,const String &time,const String &weight)
{
    File file=SD.open(LOG_FILE_NAME,FILE_APPEND);
    if(!file){
        Serial.print("ERROR: Cannot open file ");
        Serial.print(LOG_FILE_NAME);
        Serial.println(".");
        return;
    }
    file.println(date+";"+time+";"+weight);
    file.close();
    
    Serial.println("log success");
}

void SDLogger::backupLog(const String &date, const String &time, const String &weight)
{
    File file=SD.open(BACKUP_LOG_FILE_NAME,FILE_APPEND);
    if(!file){
        Serial.print("ERROR: Cannot open file ");
        Serial.print(BACKUP_LOG_FILE_NAME);
        Serial.println(".");
        return;
    }
    file.println(date+";"+time+";"+weight);
    file.close();
    
    Serial.println("backup log success");
}

SDLogger::SDLogger(int csPin,int sckPin,int mosiPin,int misoPin, const char *fileName,const char*blFileName)
{
    _csPin=csPin;
    _sckPin=sckPin;
    _mosiPin=mosiPin;
    _misoPin=misoPin;
    LOG_FILE_NAME=fileName;
    BACKUP_LOG_FILE_NAME=blFileName;
}

SDLogger::~SDLogger()
{
}

bool SDLogger::begin()
{
    SPI.begin(_sckPin,_misoPin,_mosiPin,_csPin);
    SPI.setFrequency(2000000);//2MHz
    //initialization
    if(!SD.begin(_csPin)){
        Serial.println("ERROR: SD card initialization error.");
        return false;
    }
    Serial.println("SD card initialized successfully.");

    //if file is new add the header
    if(!SD.exists(LOG_FILE_NAME)){
        File file=SD.open(LOG_FILE_NAME,FILE_WRITE);
        if(!file){
            Serial.print("ERROR: Cannot open file ");
            Serial.print(LOG_FILE_NAME);
            Serial.println(".");
            return false;
        }
        file.println("DATE(YYYY-MM-DD);TIME(HH:MM);WEIGHT(g)");
        file.close();
    }

    //if file is new add the header
    if(!SD.exists(BACKUP_LOG_FILE_NAME)){
        File file=SD.open(BACKUP_LOG_FILE_NAME,FILE_WRITE);
        if(!file){
            Serial.print("ERROR: Cannot open file ");
            Serial.print(BACKUP_LOG_FILE_NAME);
            Serial.println(".");
            return false;
        }
        file.println("DATE(YYYY-MM-DD);TIME(HH:MM);WEIGHT(g)");
        file.close();
    }

    return true;
}
