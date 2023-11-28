#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <OpenFontRender.h>
#include <esp_adc_cal.h>
#include <esp_ota_ops.h>

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


ts::BitmapAlloc buffer{ts::Display::extent};
ts::Render render;
// ts::Render render;
ts::SlideError slideError;
ts::Display display;
Preferences prefs;

void error()
{
    buffer.clear();
    slideError.fetch(render);
    slideError.render(render);
    display.present(buffer.data());
}

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
            slideError.setPrimary(ts::Strings::eErr_Wifi_ConFailed);
            goto Err_ConnectFailed;
        case WL_NO_SSID_AVAIL: 
            TS_INFO("No WiFi SSID: %s Found");
            slideError.setPrimary(ts::Strings::eErr_Wifi_SsidNotFound);
            goto Err_ConnectFailed;
        default: break;
        }
    }

    TS_INFOF("Connected to WiFi network with IP Address: %s\n", WiFi.localIP().toString());
    return true;

Err_ConnectFailed:
    slideError.setSecondary(ts::Strings::eSol_Reboot);
    error();
    return false;
}

void setup()
{
    const esp_app_desc_t* app = esp_ota_get_app_description();

    printf("\n");
    printf("ooooooooooooo       .o.       ooooo          .oooooo.    .oooooooo " "\tCommit %s\n", TALOS_VERSION_GIT_HASH_SHORT);
    printf("8'   888   `8      .888.      `888'         d8P'  `Y8b  d8P'    `Y8" "\tTag %s\n", TALOS_VERSION_GIT_TAG);
    printf("     888          .8 888.      888         888      888 Y88bo.     " "\tVersion %s\n", TALOS_VERSION_STRING);
    printf("     888         .8  `888.     888         888      888  ` Y8888o. " "\tDate %s\n", app->date);
    printf("     888        .88ooo8888.    888         888      888      ` Y88b" "\tIDF Version %s\n", app->idf_ver);
    printf("     888       .8'     `888.   888       o `88b    d88' oo     .d8P" "\tArduino %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
    printf("    o888o     o88o     o8888o o888ooooood8  `Y8bood8P'  8888888P'  " "\tby Caden Miller (https://cadenmiller.dev)\n");
    printf("\n");


    //SPI.begin(ts::Pin::SpiClk, ts::Pin::SpiCipo, ts::Pin::SpiCopi, ts::Pin::SdSpiCs);

    // if (!SD.begin(ts::Pin::SdSpiCs))
    // {
    //     printf("Could not initialize SD!\r\n");
    //     return;
    // }
    // 
    // TS_INFOF("Total Bytes: %lld, Used Bytes: %lld\r\n", SD.totalBytes(), SD.usedBytes());

    TS_INFO("TALOSv1 " TALOS_VERSION_STRING"\n");
    render.setBitmap(buffer);
    render.loadFont(Neuton_Regular, sizeof(Neuton_Regular));

    
    SPI.begin(ts::Pin::SpiClk, ts::Pin::SpiCipo, ts::Pin::SpiCopi, ts::Pin::PaperSpiCs);
    if (!display.begin(ts::Pin::PaperSpiCs, ts::Pin::PaperRst, ts::Pin::PaperDc, ts::Pin::PaperBusy, ts::Pin::PaperPwr)) 
    {
        TS_INFO("e-Paper Initialization Failed!\n");
        return;
    }

    buffer.clear(0xFF);
    // display.clear();

    TS_INFO("Creating Slide!\n");
    ts::SlideTalos talosSlide;


    talosSlide.fetch(render);
    
    TS_INFO("Rendering Slide\n");
    talosSlide.render(render);

    TS_INFO("Presenting Slide\n");
    display.present(buffer.data());

    sleep(5);

    //SD.end();
    // SPI.end();

    if (!connect())
    {
        sleep(30);
        ESP.restart();
    }

    buffer.clear();

    ts::SlideLastFM lastfmSlide;
    // ts::SlideTalos talosSlide;
// 
    TS_INFO("Beginning Slide\n");
    lastfmSlide.fetch(render);
    lastfmSlide.render(render);

    // talosSlide.fetch(nullptr);
    // talosSlide.render(ofr);

    display.present(buffer.data());

    TS_INFO("Setup Success!\n");

}

void loop() 
{

    //ts::Slide* slides[] = 
    //{
    //    new ts::SlideDigitalClock(),
    //    new ts::SlideLastFM(),
    //};

   //bool running = true;
   //ts::Slide* pCurrent = nullptr;
   //int index = 0;
   //while (running)
   //{
   //    buffer.clear(0xFF);

   //    pCurrent = slides[index];
   //    pCurrent->fetch(&buffer);
   //    pCurrent->render(render);

   //    index = ++index % std::size(slides);

   //    display.clear();
   //    display.present(buffer.data());

   //    sleep(30);
   //}

   TS_INFO("Beginning Loop!\n");

    SPI.begin(ts::Pin::SpiClk, ts::Pin::SpiCipo, ts::Pin::SpiCopi, ts::Pin::PaperSpiCs);

    TS_INFO("SPI Began\n");

    if (!display.begin(ts::Pin::PaperSpiCs, ts::Pin::PaperRst, ts::Pin::PaperDc, ts::Pin::PaperBusy, ts::Pin::PaperPwr)) 
    {
        TS_INFO("e-Paper Initialization Failed!\n");
        return;
    }

    TS_INFO("Clearing Buffer\n");

    buffer.clear(); 

   

    SPI.end();

}
