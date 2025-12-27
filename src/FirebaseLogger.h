#pragma once
#include<Firebase_ESP_Client.h>

class FirebaseLogger{
    FirebaseData firebaseData; //to know if we send the data successfully
    FirebaseAuth auth; //to log, lib needs this
    FirebaseConfig config;
    String databaseUrl;
    String databaseSecret;

public:
    FirebaseLogger(const char*url, const char* secret);
    void begin();
    bool logMeal(String date,String time,String weight);
};