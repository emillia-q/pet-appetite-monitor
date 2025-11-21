#include "SDLogger.h"

void SDLogger::log(const String &dataLine)
{
    File file=SD.open(FILE_NAME,FILE_APPEND);
    if(!file){
        Serial.print("ERROR: Cannot open file ");
        Serial.print(FILE_NAME);
        Serial.println(".");
        return;
    }
    file.println(dataLine);
    file.close();
}

SDLogger::SDLogger(int csPin,int sckPin,int mosiPin,int misoPin, const char *fileName)
{
    _csPin=csPin;
    _sckPin=sckPin;
    _mosiPin=mosiPin;
    _misoPin=misoPin;
    FILE_NAME=fileName;
}

SDLogger::~SDLogger()
{
}

bool SDLogger::begin()
{
    SPI.begin(_sckPin,_misoPin,_mosiPin,_csPin);
    SPI.setFrequency(2000000);//4MHz
    //initialization
    if(!SD.begin(_csPin)){
        Serial.println("ERROR: SD card initialization error.");
        return false;
    }
    Serial.println("SD card initialized successfully.");

    //if file is new add the header
    File file=SD.open(FILE_NAME,FILE_READ);
    if(!file)
        log("DATE;TIME;WEIGHT_G");

    file.close();

    return true;
}
