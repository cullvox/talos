#include <Arduino.h>
#include <Preferences.h>
#include <WiFiAP.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include <esp_ota_ops.h>

#include "Config.h"
#include "Version.h"
#include "Print.h"
#include "Secrets.h"
#include "Neuton_Regular.h"
#include "App.h"


namespace ts {

App::App()
    : _buffer(ts::Display::extent)
{
}

bool App::init()
{
        
    /* Print out a nice little logo. */
    const esp_app_desc_t* app = esp_ota_get_app_description();

    printf("\n");
    printf("ooooooooooooo       .o.       ooooo          .oooooo.    .oooooooo " "\tCommit %s\n", TALOS_VERSION_GIT_HASH_SHORT);
    printf("8'   888   `8      .888.      `888'         d8P'  `Y8b  d8P'    `Y8" "\tTag %s\n", TALOS_VERSION_GIT_TAG);
    printf("     888          .8 888.      888         888      888 Y88bo.     " "\tVersion %s\n", TALOS_VERSION_STRING);
    printf("     888         .8  `888.     888         888      888  ` Y8888o. " "\tDate %s\n", app->date);
    printf("     888        .88ooo8888.    888         888      888      ` Y88b" "\tIDF Version %s\n", app->idf_ver);
    printf("     888       .8'     `888.   888       o `88b    d88' oo     .d8P" "\tArduino %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
    printf("    o888o     o88o     o8888o o888ooooood8  `Y8bood8P'  88888888P' " "\tby Caden Miller (https://cadenmiller.dev)\n");
    printf("\n");



    /* Initialize the display and graphics operations. */
    _render.setBitmap(_buffer);
    if (!_render.loadFont(Neuton_Regular, sizeof(Neuton_Regular)))
    {
        TS_ERROR("Could not load default font!\n");
        return false;
    }

    SPI.begin(ts::Pin::SpiClk, ts::Pin::SpiCipo, ts::Pin::SpiCopi, ts::Pin::PaperSpiCs);
    if (!_display.begin(ts::Pin::PaperSpiCs, ts::Pin::PaperRst, ts::Pin::PaperDc, ts::Pin::PaperBusy, ts::Pin::PaperPwr)) 
    {
        TS_INFO("e-Paper Initialization Failed!\n");
        return;
    }


    _prefs.begin("talos");

    /* In the case of a first time setup, don't show the initial screen. */
    if (_prefs.getBool("fts", true))
    {
        if (!preformFirstTimeSetup()) return false;
    }

    _prefs.end();

    MDNS.begin(ts::hostname);
    

    

}

/** @brief First time setup for the device.
 *  
 *  Creates a wireless access point for the user to connect to and update their configuration.
 *  The user sets their WiFi/Password for their network in a form and it's submitted to
 *  the web server running here. This also sets up spotify authentication and other
 *  authorizations. 
 * 
*/
bool App::preformFirstTimeSetup()
{
    TS_INFO("\n");
    TS_INFO("=======================================\n");
    TS_INFO("BEGIN: First Time Setup\n");
    TS_INFO("=======================================\n");
    TS_INFO("\n");

    /* Start the access point. */
    String ssid = F("TALOS_");
    ssid.concat(WiFi.macAddress().substring(4, 7));
    
    String pass = TS_SECRET_FIRST_TIME_SETUP_PASSWORD;

    if (!WiFi.softAP(ssid, pass))
    {
        TS_ERROR("Could not begin first-time-setup soft access point!\n");
        return false;
    }

    /* Create a static url for spotify and logging in. */
    MDNS.begin("talos"); /* talos.local */


    WiFi.softAPdisconnect();

    TS_INFO("\n");
    TS_INFO("=======================================\n");
    TS_INFO("END: First Time Setup\n");
    TS_INFO("=======================================\n");
    TS_INFO("\n");
}

} /* namespace ts */