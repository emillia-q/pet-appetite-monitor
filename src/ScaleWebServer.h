#pragma once
#include<ESPAsyncWebServer.h>
#include<WiFi.h>
#include<ArduinoJson.h>
#include"Scale.h"

class ScaleWebServer{
    AsyncWebServer server;
    Scale&scale;

    //helper method for endpoint operation
    void getWeight(AsyncWebServerRequest *request);

public:
    ScaleWebServer(Scale&scale);
    ~ScaleWebServer();

    void begin();
};