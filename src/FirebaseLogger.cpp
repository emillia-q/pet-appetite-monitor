#include "FirebaseLogger.h"

FirebaseLogger::FirebaseLogger(const char *url, const char *secret)
{
    databaseUrl=url;
    databaseSecret=secret;
}

void FirebaseLogger::begin()
{
    config.database_url=databaseUrl;
    config.signer.tokens.legacy_token=databaseSecret.c_str();

    Firebase.reconnectWiFi(true); //if the connection breaks
    Firebase.begin(&config,&auth);
    Serial.println("Firebase initialized.");
}

bool FirebaseLogger::logMeal(String date, String time, String weight)
{
    //change format
    FirebaseJson json;
    json.set("time",time);
    json.set("weight",weight);

    if(Firebase.RTDB.pushJSON(&firebaseData,"/meals/"+date,&json)){
        Serial.println("Data sent to firebase.");
        return true;
    }else{
        Serial.print("Firebase error: ");
        Serial.println(firebaseData.errorReason());
        return false;
    }
}
