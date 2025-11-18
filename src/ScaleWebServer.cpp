#include "ScaleWebServer.h"

void ScaleWebServer::getWeight(AsyncWebServerRequest *request)
{
    float currentWeight=scale.getStableWeight();
    
    StaticJsonDocument<200>doc; //probably 200bytes is more than needed
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
    server.on("/weight",HTTP_GET,std::bind(&ScaleWebServer::getWeight,this,std::placeholders::_1)); //pointer to a specific object
    //TODO: fix using lambda??
    server.begin();
    Serial.println("Server works");
}
