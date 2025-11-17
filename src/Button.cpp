#include "Button.h"

Button::Button(int buttonPin)
{
    _buttonPin=buttonPin;
    buttonState=HIGH;
    lastButtonState=HIGH;
    actionExecuted=false;
    pressStartMonitoringTime=0;
    pinMode(_buttonPin,INPUT_PULLUP);
}

Button::~Button()
{
}

void Button::measurePressTime()
{
    if(lastButtonState==HIGH&&buttonState==LOW){
        pressStartMonitoringTime=millis();
        actionExecuted=false;
    }
}

void Button::buttonStateRead()
{
    buttonState=digitalRead(_buttonPin);
}

void Button::changeLastState()
{
    lastButtonState=buttonState;
}

bool Button::buttonClick()
{
    if(lastButtonState==LOW && buttonState==HIGH && !actionExecuted){
        actionExecuted=true;
        return true;
    }else
        return false;
}

bool Button::buttonHold()
{
    if(buttonState==LOW && !actionExecuted){
        if(millis()-pressStartMonitoringTime>=LONG_PRESS_TIME){
            actionExecuted=true;
            return true;
        }
    }
    return false;
}