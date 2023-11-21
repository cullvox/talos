#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <OpenFontRender.h>

#include "Version.h"
#include "Print.h"
#include "NotoSans_Bold.h"
#include "Neuton_Regular.h"
#include "Pins.h"
#include "Bitmap.h"
#include "Display.h"
#include "SlideDigitalClock.h"
#include "SlideLastFM.h"
#include "SlideError.h"
#include "SlideTalos.h"
#include "FlashStrings.h"
#include "Secrets.h"


ts::BitmapAlloc buffer{ts::Display::extent};
OpenFontRender ofr;
// ts::Render render;
ts::SlideError slideError;
ts::Display display;

void error()
{
    buffer.clear();
    slideError.fetch(&buffer);
    slideError.render(ofr);
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
    auto ofrDrawPixel = [](int32_t x, int32_t y, uint16_t c){ buffer.set(ts::Vector2i{(int16_t)x, (int16_t)y}, c); };
    auto ofrPrint = [](const char* str){ printf("%s", str); };

    ofr.set_printFunc(ofrPrint);
    ofr.set_drawPixel(ofrDrawPixel);
    ofr.showCredit();
    ofr.showFreeTypeVersion();
    ofr.setDebugLevel(OFR_DEBUG | OFR_ERROR | OFR_INFO);
    
    
    //SPI.begin(ts::Pin::SpiClk, ts::Pin::SpiCipo, ts::Pin::SpiCopi, ts::Pin::SdSpiCs);

    TS_INFOF("This is a %s!\n", "TEST");

    //if (!SD.begin(ts::Pin::SdSpiCs))
    //{
    //    printf("Could not initialize SD!\r\n");
    //    return;
    //}
    //
    //printf("Total Bytes: %lld, Used Bytes: %lld\r\n", SD.totalBytes(), SD.usedBytes());

    ofr.loadFont(Neuton_Regular, sizeof(Neuton_Regular), 0);

    
    SPI.begin(ts::Pin::SpiClk, ts::Pin::SpiCipo, ts::Pin::SpiCopi, ts::Pin::PaperSpiCs);
    if (!display.begin(ts::Pin::PaperSpiCs, ts::Pin::PaperRst, ts::Pin::PaperDc, ts::Pin::PaperBusy, ts::Pin::PaperPwr)) 
    {
        TS_INFO("e-Paper Initialization Failed!\n");
        return;
    }

    buffer.clear(0xFF);
    // display.clear();

    ts::SlideTalos talosSlide;
    talosSlide.fetch(nullptr);
    talosSlide.render(ofr);

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
    lastfmSlide.fetch(&buffer);
    lastfmSlide.render(ofr);

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
