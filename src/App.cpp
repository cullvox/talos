#include <atomic>

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <SPIFFS.h>

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
    , _server(80)
{
    memset(_wifiSSID, 0, sizeof(_wifiSSID));
    memset(_wifiPassword, 0, sizeof(_wifiPassword));
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

    if (!SPIFFS.begin(true))
    {
        TS_ERROR("Could not mount SPIFFS!");
        return false;
    }

    TS_INFOF("Does file exist: %d", SPIFFS.exists("/first_time_setup.html"));

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
        return false;
    }


    _prefs.begin("talos");

    /* In the case of a first time setup, don't show the initial screen. */
    if (_prefs.getBool("fts", true))
    {
        if (!preformFirstTimeSetup()) return false;
    }

    _prefs.end();

    return true;
    // MDNS.begin(ts::hostname);

}

const char* responseHTML PROGMEM = ""
        "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
        "<h1>TALOS</h1><p>This is a captive portal example. All requests will "
        "be redirected here.</p></body></html>";

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}
    bool canHandle(AsyncWebServerRequest *request)
    {
        request->addInterestingHeader("ANY");
        return true;
    }
    void handleRequest(AsyncWebServerRequest *request)
    {
        TS_INFO("Sending first time setup page to browser.\n");
        request->send(SPIFFS, "/first_time_setup.html", "text/html");
        // request->send_P(200, "text/html", responseHTML); 
    }
};

bool App::connectToWiFi()
{

    const char* SSID = TS_SECRET_WIFI_SSID;
    const char* Password = TS_SECRET_WIFI_PASSWORD;
    const char* Username = TS_SECRET_WIFI_USERNAME;
    const char* Identity = TS_SECRET_WIFI_IDENTITY;
    wpa2_auth_method_t Auth = TS_SECRET_WIFI_WAP2_AUTH;

    if (TS_SECRET_WIFI_USE_ENTERPRISE) {
        WiFi.begin(SSID, Auth, Identity, Username, Password);
    } else {
        WiFi.begin(_wifiSSID, _wifiPassword);
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

startFirstTimeSetup:
    TS_INFO("\n");
    TS_INFO("=======================================\n");
    TS_INFO("BEGIN: First Time Setup\n");
    TS_INFO("=======================================\n");
    TS_INFO("\n");

    /* Start the access point. */
    String pass = "ABC1234567";
    String ssid = F("TALOS_");
    ssid.concat(WiFi.macAddress().substring(3, 5));

    IPAddress apip{192, 168, 1, 1};
    WiFi.softAPConfig(apip, apip, IPAddress{255, 255, 255, 0});
    if (!WiFi.softAP(ssid, pass))
    {
        TS_ERROR("Could not begin first-time-setup access point!\n");
        return false;
    }

    TS_INFOF("Access point started with name: %s, IP: %s, MAC: %s\n", ssid, apip.toString().c_str(), WiFi.macAddress().c_str());

    /* Start a DNS server to create a captive portal. */
    DNSServer dnsServer;
    dnsServer.start(53, "*", apip);
    
    /* Setup the server callbacks and begin. */
    std::atomic<bool> finished = false;

    _server.on("/done", HTTP_POST, [&](AsyncWebServerRequest* request){
        
        /* Retrieve the WiFi SSID, password, username and identity. */
        int params = request->params();
        for (int i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            TS_INFOF("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }

        /* Process the parameters received. */
        AsyncWebParameter* param = request->getParam("wifi_ssid");
        if (!param)
        {
            TS_ERROR("Expected parameter wifi_ssid not found in form!");
            request->send(200, "text/http", "<html><p>Work is just about done!</p></html>");
            return;
        }

        param->value().getBytes((unsigned char*)_wifiSSID, sizeof(_wifiSSID)-1);

        param = request->getParam("wifi_password");
        if (!param)
        {
            TS_ERROR("Expected parameter wifi_password not found in form!");
            request->send(200, "text/http", "<html><p>Work is just about done!</p></html>");
            return;
        }
        
        param->value().getBytes((unsigned char*)_wifiPassword, sizeof(_wifiPassword)-1);

        param = request->getParam("enable_spotify");
        if (param) _enableSpotify = true; /* data race shouldn't occur here... hopefully.  */

        TS_INFOF("Recieved Wi-Fi SSID: %s, Password: %s\n", _wifiSSID, _wifiPassword);

        finished = true;
        request->send(200, "text/http", "<html><p>Work is just about done!</p></html>");
        
    });

    _server.on("/css/pico.min.css", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/css/pico.min.css", "text/css");
    });
    
    _server.on("/css/font-awesome.min.css", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/css/font-awesome.min.css", "text/css");
    });

    _server.on("/fonts/fontawesome-webfont.ttf", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/fonts/fontawesome-webfont.ttf");
    });

    _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    _server.begin();

    /* Process DNS requests for captive request until setup is consitered complete. */
    while (!finished) {
        dnsServer.processNextRequest();
    };
    
    _server.end();
    _server.reset();
    dnsServer.stop();
    WiFi.softAPdisconnect();

    /* Attempt to connect to the persons wifi. */
    if (!connectToWiFi()) {
        TS_INFO("Could not connect to WiFi during first time setup, configuring again.\n");
        goto startFirstTimeSetup; /* connectToWiFi will display an error. */
    }

    /* Establish authorization with spotify. */
    if (_enableSpotify)
    {
        
        if (!_spotify.blockingRequestUserAuthorization())
        {
            /* Display to the user that first time setup is restarting. */
            displayError(Strings::eSpotAuthFailed, Strings::eSolRestartFTS);    
            goto startFirstTimeSetup;
        }
    }

    /* Display the setup complete slide. */
    

    TS_INFO("Setup complete!\n");

    TS_INFO("\n");
    TS_INFO("=======================================\n");
    TS_INFO("END: First Time Setup\n");
    TS_INFO("=======================================\n");
    TS_INFO("\n");

    return true;

displayError:

    goto startFirstTimeSetup;    

}

void App::displayError(Strings::Select primary, Strings::Select secondary)
{
    _slideError.setPrimary(primary);
    _slideError.setSecondary(secondary);
    _buffer.clear();
    _slideError.render(_render);
    _display.present(_buffer.data());
}

} /* namespace ts */