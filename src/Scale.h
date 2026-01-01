#pragma once
#include<HX711.h>
#include<Preferences.h>

class Scale{
public:
    static constexpr float EMA_ALPHA = 0.30f; //EMA smoothing
    static constexpr float DISPLAY_STEP = 1.0f; //rounding step to units
    static constexpr float DISPLAY_HYST = 0.5f; //prevents flashing between two adjacent numbers
    static constexpr int MED_N = 5;
    static constexpr int  MEASURE_TIME=60000; //60s

    //constructors & destructors
    Scale(int doutPin, int sckPin);
    ~Scale();

    void begin(); //probably to delete it when calibrated scale will be in teh flash
    long getStableWeight();
    long getWeightDrop();
    void setStartWeight();
    bool getDidDrop();
    void setDidDrop(bool state);
    void checkWeightDrop();
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
    long lastShown,startWeight,weightDrop;
    bool didDrop;

    //flash for calibration factor
    Preferences pref;
    const char* PREF_NS="scale";
    const char* PREF_CAL_KEY="cal";
    float scale_factor;

    //helper methods
    float medianFromBuffer(const float* buf,int n);
    void applyStabilization(float gRaw);
    //calculates and saves the calibration factor (one-time use)
    void calibration(float calibrationMass); 
};