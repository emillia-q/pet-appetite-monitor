#pragma once
#include<SD.h>
#include<FS.h>

class SDLogger{
    int _csPin,_sckPin,_mosiPin,_misoPin;
    const char* FILE_NAME;

    
public:
    SDLogger(int csPin,int sckPin,int mosiPin,int misoPin,const char* fileName);
    ~SDLogger();
    void log(const String &date,const String &time,const String &weight); //TODO: change it to private, for now its for test
    bool begin();
    //TODO: void recordWeigthDrop(); 
};