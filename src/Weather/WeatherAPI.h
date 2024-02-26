#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

class WeatherAPI {
public:
    WeatherAPI(WiFiClientSecure& wifi, HTTPClient& http);
    ~WeatherAPI();
    

private:
    
};