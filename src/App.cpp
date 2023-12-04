#include <atomic>

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <mbedtls/sha256.h>
#include <base64.h>
#include <ESPNtpClient.h>

#include <esp_ota_ops.h>

#include "Config.h"
#include "Version.h"
#include "Print.h"
#include "Secrets.h"
#include "Neuton_Regular.h"
#include "App.h"


namespace ts {

App::App()
    : _display()
    , _buffer(_display.extent())
    , _server(80)
{
    memset(_config.wifiSSID, 0, sizeof(_config.wifiSSID));
    memset(_config.wifiPassword, 0, sizeof(_config.wifiPassword));
    memset(_config.wifiIdentity, 0, sizeof(_config.wifiIdentity));
    memset(_config.wifiUsername, 0, sizeof(_config.wifiUsername));
    _config.isWifiEnterprise = false;
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
    ffsupport_setffs(SPIFFS);
    if (!_render.loadFont("/fonts/Neuton-Regular.ttf"))
    {
        TS_ERROR("Could not load default font!\n");
        return false;
    }

    SPI.begin(TS_PIN_SPI_CLK, TS_PIN_SPI_CIPO, TS_PIN_SPI_COPI, TS_PIN_PAPER_SPI_CS);
    if (!_display.begin(TS_PIN_PAPER_SPI_CS, TS_PIN_PAPER_RST, TS_PIN_PAPER_DC, TS_PIN_PAPER_BUSY, TS_PIN_PAPER_PWR)) 
    {
        TS_INFO("e-Paper Initialization Failed!\n");
        return false;
    }

    _prefs.begin("talos");

    _config.isFirstTimeSetup = _prefs.getBool("fts", true);
    _config.spotifyEnabled = _prefs.getBool("spotenable", false);
    _config.spotifyAuthorized = _prefs.getBool("spotauthed", false);

    _prefs.end();

    /* Preform the first time setup. */
    if (_config.isFirstTimeSetup)
    {

        if (!preformFirstTimeSetup()) return false;
        // _prefs.putBool("fts", false);
    }


    /* Preform authentication for Spotify. */
    if (_config.spotifyEnabled && !_config.spotifyAuthorized)
    {
        if (!preformSpotifyAuthorization()) return false;
    }

    /* Display the TALOS splash screen. */


    return true;
}

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
        log_i("Sending first time setup page to browser.");

        if (request->requestedConnType() == HTTP_GET &&
            request->url() == "/css/pico.min.css") {
            request->send(SPIFFS, "/css/pico.min.css", "text/css");
            return;
        } else if (request->requestedConnType() == HTTP_GET &&
            request->url() == "/css/font-awesome.min.css") {
            request->send(SPIFFS, "/css/font-awesome.min.css", "text/css");
            return;
        } else if (request->requestedConnType() == HTTP_GET &&
            request->url() == "/css/fontawesome-webfont.ttf") {
            request->send(SPIFFS, "/css/fontawesome-webfont.ttf", "text/css");
            return;
        } else if (request->requestedConnType() == HTTP_GET &&
            request->url() == "/") {
            request->send(SPIFFS, "/first_time_setup.html");
            return;
        }

        request->send(SPIFFS, "/first_time_setup.html", "text/html");
    }
};

bool App::connectToWiFi()
{
    if (_config.isWifiEnterprise) {
        log_i("Using Enterprise, SSID: %s, Identitiy: %s, Username %s, Password: %s", _config.wifiSSID, _config.wifiIdentity, _config.wifiUsername, _config.wifiPassword);
        WiFi.begin(_config.wifiSSID, WPA2_AUTH_PEAP, _config.wifiIdentity, _config.wifiUsername, _config.wifiPassword);
    } else {
        WiFi.begin(_config.wifiSSID, _config.wifiPassword);
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
            TS_INFO("No WiFi SSID: %s Found\n");
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

    log_i("\n");
    log_i("=======================================\n");
    log_i("BEGIN: First Time Setup\n");
    log_i("=======================================\n");
    log_i("\n");

    /* Start the access point. */
    String pass = "ABC1234567";
    String ssid = F("TALOS_");
    ssid.concat(WiFi.macAddress().substring(3, 5));

    WiFi.softAP(ssid, pass);

    /* Start the DNS server to catch all for the captive portal. */
    DNSServer dnsServer;
    dnsServer.start(53, "*", WiFi.softAPIP());


    /* TODO: Display the actual SSID in displayGeneral some how. */
    displayGeneral(Strings::eSeverityInfo, Strings::eSetupBeginning, Strings::eSetupJoinWifi);
    
    /* Setup the server callbacks and begin. */
    std::atomic<bool> finished(false);

    _server.on("/done", HTTP_POST, [&](AsyncWebServerRequest* request){
        
        /* Retrieve the WiFi SSID, password, username and identity. */
        int params = request->params();
        for (int i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            TS_INFOF("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());

            if (p->name() == "wifi_ssid")
                p->value().getBytes((unsigned char*)_config.wifiSSID, sizeof(_config.wifiSSID)-1);
            
            if (p->name() == "wifi_password")
                p->value().getBytes((unsigned char*)_config.wifiPassword, sizeof(_config.wifiPassword)-1);
 
            if (p->name() == "wifi_enterprise_username") {
                _config.isWifiEnterprise = true;
                p->value().getBytes((unsigned char*)_config.wifiUsername, sizeof(_config.wifiUsername)-1);
            }

            if (p->name() == "wifi_enterprise_identity") {
                _config.isWifiEnterprise = true;
                p->value().getBytes((unsigned char*)_config.wifiIdentity, sizeof(_config.wifiIdentity)-1);
            }

            if (p->name() == "enable_spotify")
                _config.spotifyEnabled = true; /* data race shouldn't occur here... hopefully.  */
        }
 
        TS_INFOF("Recieved Wi-Fi SSID: %s, Password: %s\n", _config.wifiSSID, _config.wifiPassword);

        finished = true;
        request->send(200, "text/http", "<html><p>Work is just about done!</p></html>");
    });
 
 
     _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
 
    _server.begin();
 
     while (!finished) { dnsServer.processNextRequest(); }
 
     WiFi.softAPdisconnect(true);

    dnsServer.stop();
    _server.reset();

    /* Attempt to connect to the persons wifi. */
    if (!connectToWiFi()) {
        TS_INFO("Could not connect to WiFi during first time setup, configuring again.\n");
        return false; /* connectToWiFi will display an error. */
    }

    /* WiFi is guaranteed connected, connect the NTP client. */
    if (!NTP.begin(nullptr, false))
    {
        /* WiFi might not be properly configured, we're not connected to the web. */
        log_e("NTP connection failed, WiFi may not be connected to internet.");

        // displayGeneral()
    }

    while (true)
    {
        sleep(1);
        log_i("Time: %s", NTP.getTimeDateString());
    }


    /* Establish authorization with spotify. */
    if (_config.spotifyEnabled)
    {
        if (!preformSpotifyAuthorization())
        {
            /* Display to the user that first time setup is restarting. */
            displayGeneral(Strings::eSeverityError, Strings::eErrSpotAuthFailed, Strings::eSolRestartFTS);    
            return false;
        }
    }

    /* Display the setup complete slide. */
    

    log_i("Setup complete!");

    log_i("");
    log_i("=======================================");
    log_i("END: First Time Setup");
    log_i("=======================================");
    log_i("");

    return true;
}

bool App::preformSpotifyAuthorization()
{
    log_d("Beginning Spotify authentication process");

    /* Add spotify callbacks for the authentication process. */
    std::atomic<bool> finished(false);

    /* Start MDNS for to use talos.local for Spotify config. */
    log_d("Beginning MDNS");
    MDNS.begin("talos");

    _spotify.addAuthCallbacks(_server); 

    /* Wait until authentication is complete. */
    while (!finished) { }

    if (_spotify.isRefreshRequired())
        _spotify.blockingUpdateRefreshToken();

    /* Cleanup server stuffs. */
    _server.end();
    MDNS.end();

    return true;
}

void App::displayGeneral(Strings::Select severity, Strings::Select primary, Strings::Select secondary)
{
    _slideGeneral.setSeverity(severity);
    _slideGeneral.setPrimary(primary);
    _slideGeneral.setSecondary(secondary);
    _buffer.clear();
    _slideGeneral.render(_render);
    _display.present(_buffer.data());
}

} /* namespace ts */