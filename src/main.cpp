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



bool connect()
{

    const char* SSID = TS_SECRET_WIFI_SSID;
    const char* Password = TS_SECRET_WIFI_PASSWORD;
    const char* Username = TS_SECRET_WIFI_USERNAME;
    const char* Identity = TS_SECRET_WIFI_IDENTITY;
    wpa2_auth_method_t Auth = TS_SECRET_WIFI_WAP2_AUTH;

    if (TS_SECRET_WIFI_USE_ENTERPRISE) {
        WiFi.begin(SSID, Auth, Identity, Username, Password);
    } else {
        WiFi.begin(SSID, Password);
    }

    TS_INFO("Connecting to WiFi\n");
    bool connected = false;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        printf(".\n");

        switch(WiFi.status())
        {
        case WL_CONNECTED:
            connected = true;
        case WL_IDLE_STATUS:
            continue;
        case WL_CONNECT_FAILED: 
            TS_INFO("WiFi Connection failed!\n");
            //slideError.setPrimary(ts::Strings::eErr_Wifi_ConFailed);
            goto Err_ConnectFailed;
        case WL_NO_SSID_AVAIL: 
            TS_INFO("No WiFi SSID: %s Found");
            // slideError.setPrimary(ts::Strings::eErr_Wifi_SsidNotFound);
            goto Err_ConnectFailed;
        default: break;
        }
    }

    TS_INFOF("Connected to WiFi network with IP Address: %s\n", WiFi.localIP().toString());
    return true;

Err_ConnectFailed:
    // slideError.setSecondary(ts::Strings::eSol_Reboot);
    // error();
    return false;
}

ts::App app;

void setup()
{
    app.init();
}

void loop() 
{
}
