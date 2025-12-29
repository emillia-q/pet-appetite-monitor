#pragma once
#include<SD.h>
#include<FS.h>

class SDLogger{
    int _csPin,_sckPin,_mosiPin,_misoPin;
    const char* LOG_FILE_NAME;
    const char* BACKUP_LOG_FILE_NAME;

    
public:
    SDLogger(int csPin,int sckPin,int mosiPin,int misoPin,const char* fileName,const char*blFileName);
    ~SDLogger();
    void log(const String &date,const String &time,const String &weight); //TODO: change it to private, for now its for test
    void backupLog(const String &date,const String &time,const String &weight);
    bool begin();
};