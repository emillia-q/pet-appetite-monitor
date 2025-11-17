#pragma once
#include<HX711.h>

class Scale{
public:
    static constexpr float EMA_ALPHA = 0.30f; //EMA smoothing
    static constexpr float DISPLAY_STEP = 1.0f; //rounding step to units
    static constexpr float DISPLAY_HYST = 0.5f; //prevents flashing between two adjacent numbers
    static constexpr int MED_N = 5;

    //constructors & destructors
    Scale(int doutPin, int sckPin);
    ~Scale();

    void begin(float calibrationMass); //probably to delete it when calibrated scale will be in teh flash
    long getStableWeight();
    void tare();
private:
    HX711 scale;
    int _doutPin;
    int _sckPin;
    float medBuf[MED_N];
    int medIndex;
    int medCount;
    bool emaInit;
    float gEMA; //current value after EMA filter
    long lastShown;

    //helper methods
    float medianFromBuffer(const float* buf,int n);
    void applyStabilization(float gRaw);
};