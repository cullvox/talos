#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>

#include "Version.h"
#include "Print.h"
#include "NotoSans_Bold.h"
#include "Neuton_Regular.h"
#include "Pins.h"
#include "Bitmap.h"
#include "Display.h"
#include "slides/SlideDigitalClock.h"
#include "slides/SlideSpotify.h"
#include "slides/SlideError.h"
#include "slides/SlideTalos.h"
#include "FlashStrings.h"
#include "Secrets.h"
#include "slides/SlideLastFM.h"
#include "App.h"

ts::App app;

void setup()
{
    app.init();
}

void loop() 
{
}
