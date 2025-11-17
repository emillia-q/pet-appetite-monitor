#include "Scale.h"

Scale::Scale(int doutPin, int sckPin)
{
    _doutPin=doutPin;
    _sckPin=sckPin;

    medBuf[MED_N] = {0};
    medIndex = 0;
    medCount = 0;
    emaInit = false;
    gEMA = 0.0f;
    lastShown = 0;
}

Scale::~Scale()
{
}

void Scale::begin(float calibrationMass)
{
    //scale
    scale.begin(_doutPin,_sckPin);
    scale.tare(10); //to remove the weight of the stand
    Serial.println("place a weight:");
    delay(5000);

    float reading=scale.get_units(10);
    float scale_factor=reading/calibrationMass;
    Serial.print("calculated: ");
    Serial.println(scale_factor);
    scale.set_scale(scale_factor);

    //reset filters after calibration
    emaInit=false;
    medIndex=0;
    medCount=0;
    lastShown=0;
}

long Scale::getStableWeight()
{
    //measurement and stabilization
    if(scale.is_ready()){
        float gRaw=scale.get_units(1); //quick sample
        applyStabilization(gRaw);
    }

    return (lastShown<0) ? 0 : lastShown;
}

void Scale::tare()
{
    //reset
    emaInit=false;
    medIndex=0;
    medCount=0;
    lastShown=0;
    scale.tare();
}

float Scale::medianFromBuffer(const float* buf,int n)
{
  float tmp[Scale::MED_N];
  for(int i=0;i<n;i++)
    tmp[i]=buf[i];

  //insertion sort
  for(int i=1;i<n;i++){
    float element=tmp[i];
    int j=i-1;
    while(j>=0 && tmp[j]>element){
      tmp[j+1]=tmp[j];
      j--;
    }
    tmp[j+1]=element;
  }

  if(n==0)
    return 0.0f;

  return (n%2==0) ? (tmp[n/2-1]+tmp[n/2])/2 : tmp[n/2];
}

void Scale::applyStabilization(float gRaw)
{
    if(!isfinite(gRaw))
      gRaw==0.0f;

    //median of 5 samples
    medBuf[medIndex++]=gRaw;
    if(medIndex>=MED_N)
      medIndex=0;
    if(medCount<MED_N)
      medCount++;
    float gMed=medianFromBuffer(medBuf,medCount);

    //EMA smoothing
    if(!emaInit){
      gEMA=gMed;
      emaInit=true;
    }else
      gEMA=EMA_ALPHA*gMed+(1.0f-EMA_ALPHA)*gEMA;
    
    //rounding
    float gQuant=roundf(gEMA/DISPLAY_STEP)*DISPLAY_STEP;
    long weight=round(gQuant);

    //display hysteresis
    if(abs(weight-lastShown)>DISPLAY_HYST)
      lastShown=weight;
}
