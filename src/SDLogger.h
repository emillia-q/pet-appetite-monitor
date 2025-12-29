#pragma once
#include<SD.h>
#include<FS.h>
#include"FirebaseLogger.h"

class SDLogger{
    int _csPin,_sckPin,_mosiPin,_misoPin;
    const char* LOG_FILE_NAME;
    const char* BACKUP_LOG_FILE_NAME;
    bool sync;

public:
    SDLogger(int csPin,int sckPin,int mosiPin,int misoPin,const char* fileName,const char*blFileName);
    ~SDLogger();
    void log(const String &date,const String &time,const String &weight);
    void backupLog(const String &date,const String &time,const String &weight);
    bool begin();
    void syncBackupWithFirebase(FirebaseLogger&firebase);
    bool getSync();
};