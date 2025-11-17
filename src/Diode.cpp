#include"Diode.h"

Diode::Diode(int ledPin)
{
    _ledPin=ledPin;
    pinMode(_ledPin,OUTPUT);
}

Diode::~Diode()
{
}

void Diode::tareMsg()
{
    digitalWrite(_ledPin,HIGH);
    delay(1000);
    digitalWrite(_ledPin,LOW);
}

void Diode::startMonitoringMsg()
{
    for(int i=0;i<3;i++){
        digitalWrite(_ledPin,HIGH);
        delay(500); 
        digitalWrite(_ledPin,LOW);
        delay(500);
    }
}
