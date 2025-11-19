#include "ScaleWebServer.h"

void ScaleWebServer::getWeight(AsyncWebServerRequest *request)
{
    float currentWeight=scale.getStableWeight();
    
    JsonDocument doc; 
    doc["weight"]=currentWeight;
    doc["unit"]="g";

    String jsonResponse;
    serializeJson(doc,jsonResponse);

    //send to Kotlin
    request->send(200,"application/json",jsonResponse);
}

ScaleWebServer::ScaleWebServer(Scale &scale) : server(8080),scale(scale)
{
}

ScaleWebServer::~ScaleWebServer()
{
}

void ScaleWebServer::begin()
{
    //endpoint GET weight
    server.on("/weight",HTTP_GET,[this](AsyncWebServerRequest*request){
        this->getWeight(request);
    }); //pointer to a specific object

    server.begin();
    Serial.println("Server works");
}
