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

    log_printf("");
    log_printf("ooooooooooooo       .o.       ooooo          .oooooo.    .oooooooo " "\tCommit %s\n", TALOS_VERSION_GIT_HASH_SHORT);
    log_printf("8'   888   `8      .888.      `888'         d8P'  `Y8b  d8P'    `Y8" "\tTag %s\n", TALOS_VERSION_GIT_TAG);
    log_printf("     888          .8 888.      888         888      888 Y88bo.     " "\tVersion %s\n", TALOS_VERSION_STRING);
    log_printf("     888         .8  `888.     888         888      888  ` Y8888o. " "\tDate %s\n", app->date);
    log_printf("     888        .88ooo8888.    888         888      888      ` Y88b" "\tIDF Version %s\n", app->idf_ver);
    log_printf("     888       .8'     `888.   888       o `88b    d88' oo     .d8P" "\tArduino %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
    log_printf("    o888o     o88o     o8888o o888ooooood8  `Y8bood8P'  88888888P' " "\tby Caden Miller (https://cadenmiller.dev)\n");
    log_printf("");

    if (!SPIFFS.begin(true))
    {
        TS_ERROR("Could not mount SPIFFS!");
        return false;
    }

    TS_INFOF("Does file exist: %d", SPIFFS.exists("/first_time_setup.html"));

    /* Initialize the display and graphics operations. */
    _render.setBitmap(_buffer);
    ffsupport_setffs(SPIFFS);

    
    if (!_render.loadFont("/fonts/faxnrxwi.ttf"))
    {
        log_e("Could not load the icons font!");
        return false;
    }

    // if (!_render.loadFont("/fonts/Neuton-Regular.ttf", 1))
    // {
    //     log_e("Could not load default font!\n");
    //     return false;
    // }


    SPI.begin(TS_PIN_SPI_CLK, TS_PIN_SPI_CIPO, TS_PIN_SPI_COPI, TS_PIN_PAPER_SPI_CS);
    if (!_display.begin(TS_PIN_PAPER_SPI_CS, TS_PIN_PAPER_RST, TS_PIN_PAPER_DC, TS_PIN_PAPER_BUSY, TS_PIN_PAPER_PWR)) 
    {
        TS_INFO("e-Paper Initialization Failed!\n");
        return false;
    }

    _prefs.begin("talos");

    _config.isFirstTimeSetup = _prefs.getBool("fts", true);
    _prefs.getBytes("wifissid", _config.wifiSSID, sizeof(_config.wifiSSID)-1);
    _prefs.getBytes("wifipass", _config.wifiPassword, sizeof(_config.wifiPassword)-1);
    _config.spotifyEnabled = _prefs.getBool("spotenable", false);
    _config.spotifyAuthorized = _prefs.getBool("spotauthed", false);

    log_i("config: fts = %s", _config.isFirstTimeSetup ? "true" : "false");
    log_i("config: wifissid = %s", _config.wifiSSID);
    log_i("config: wifipass = %s", _config.wifiPassword);
    log_i("config: spotenable = %s", _config.spotifyEnabled ? "true" : "false");
    log_i("config: spotauthed = %s", _config.spotifyAuthorized ? "true" : "false");

    _prefs.end();

    /* Preform the first time setup. */
    if (_config.isFirstTimeSetup)
    {
        if (!preformFirstTimeSetup()) return false;
        _prefs.putBool("fts", false);
    }

    if (!connectToWiFi())
    {
        preformFirstTimeSetup();
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

        if (request->method() == HTTP_GET &&
            request->url() == "/css/pico.min.css") {
            request->send(SPIFFS, "/css/pico.min.css", "text/css");
            return;
        } else if (request->method() == HTTP_GET &&
            request->url() == "/css/font-awesome.min.css") {
            request->send(SPIFFS, "/css/font-awesome.min.css", "text/css");
            return;
        } else if (request->method() == HTTP_GET &&
            request->url() == "/fonts/fontawesome-webfont.ttf") {
            request->send(SPIFFS, "/fonts/fontawesome-webfont.ttf", "text/css");
            return;
        } else if (request->method() == HTTP_GET &&
            request->url() == "/") {
            request->send(SPIFFS, "/first_time_setup.html");
            return;
        }

        request->send(SPIFFS, "/first_time_setup.html", "text/html");
    }
};

bool App::connectToWiFi()
{
    log_i("Initiating connection to WiFi: SSID: %s", _config.wifiSSID);

    /* Clear any previous WiFi settings. */
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);

    /* Disable automatic reconnect so we can display an 
        error message to the user properly. */
    WiFi.setAutoReconnect(false); 
    
    /* Begin the WiFi! */
    WiFi.begin(_config.wifiSSID, _config.wifiPassword);

    bool connected = false;
    time_t timeStarted = time(nullptr);

    while (!connected) {

        /* Stop attempting to connect after time runs out. */
        const time_t timeSinceStarted = time(nullptr) - timeStarted;
        log_i("Connection time remaining: %ds", TS_MAX_WIFI_CONNECTION_TIMEOUT - timeSinceStarted);

        if (timeSinceStarted > TS_MAX_WIFI_CONNECTION_TIMEOUT) {
            log_e("Timeout, could not connect to WiFi in " TS_STRINGIFY(TS_MAX_WIFI_CONNECTION_TIMEOUT) " seconds.");
            
            /* Timeout error has occurred. */

            goto Err_ConnectFailed;
        }

        delay(500);

        switch(WiFi.status()) {
        case WL_DISCONNECTED:
            log_e("WiFi Disconnected.");
        case WL_IDLE_STATUS:
            /* Continue until something interesting happens. */ 
            continue; 
        case WL_NO_SSID_AVAIL: 
            log_e("Wifi SSID: %s was not found.", _config.wifiSSID);
            goto Err_ConnectFailed;
        case WL_CONNECTED:
            connected = true;
            break;
        case WL_CONNECT_FAILED:  
            log_e("WiFi connection failed.");
            goto Err_ConnectFailed;
        case WL_CONNECTION_LOST: 
            log_e("WiFi connection lost.");
            goto Err_ConnectFailed;

        case WL_SCAN_COMPLETED: /* We aren't scanning right now... */
        case WL_NO_SHIELD:
        default:
            log_i("Unexpected WiFi connection error: %d", WiFi.status());
        }

    }

    log_i("Connected to WiFi network with IP Address: %s", WiFi.localIP().toString());


    /* WiFi is guaranteed connected, connect the NTP client to a server. */
    log_i("Connecting to NTP time server: %s", DEFAULT_NTP_SERVER);

    if (!NTP.begin(DEFAULT_NTP_SERVER, false))
    {
        /* WiFi might not be properly configured, we're not connected to the web. */
        log_e("NTP connection failed, WiFi may not be connected to internet.");
        return false;
        // displayGeneral()
    }

    timeStarted = time(nullptr);
    while (NTP.getFirstSync() == 0) {
        /* Stop attempting to connect after time runs out. */
        const time_t timeSinceStarted = time(nullptr) - timeStarted;
        log_i("NTP client connection time remaining: %ds", TS_MAX_NTP_CONNECTION_TIMEOUT - timeSinceStarted);

        if (timeSinceStarted > TS_MAX_NTP_CONNECTION_TIMEOUT)
        {
            log_e("Timeout, could not connect to an NTP server in " TS_STRINGIFY(TS_MAX_WIFI_CONNECTION_TIMEOUT) " seconds.");
            /* Timeout error has occurred. */

            goto Err_ConnectFailed;
        }

        delay(500);
        log_i("Waiting for an NTP connection.");
    }

    log_i("Current network time: %s", NTP.getTimeDateString());

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
    log_printf("\n");
    log_printf("=======================================\n");
    log_printf("BEGIN: First Time Setup\n");
    log_printf("=======================================\n");
    log_printf("\n");

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

    _server.on("/done", HTTP_GET, [&](AsyncWebServerRequest* request){
        
        /* Retrieve the WiFi SSID, password, username and identity. */
        int params = request->params();
        for (int i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            TS_INFOF("GET [%s]: %s\n", p->name().c_str(), p->value().c_str());

            if (p->name() == "wifi_ssid")
                p->value().getBytes((unsigned char*)_config.wifiSSID, sizeof(_config.wifiSSID)-1);
            
            if (p->name() == "wifi_password")
                p->value().getBytes((unsigned char*)_config.wifiPassword, sizeof(_config.wifiPassword)-1);

            if (p->name() == "enable_spotify")
                _config.spotifyEnabled = true; /* data race shouldn't occur here... hopefully.  */
        }
 
        TS_INFOF("Recieved Wi-Fi SSID: %s, Password: %s\n", _config.wifiSSID, _config.wifiPassword);

        
        
        request->send(SPIFFS, "text/http", "/setup_complete.html");
        finished = true;
    });
 
 
    _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    _server.begin();
 
    while (!finished) { dnsServer.processNextRequest(); }

    WiFi.softAPdisconnect(true);

    dnsServer.stop();
    _server.reset();

    /* Save the WiFi credentials. */
    Preferences prefs;
    prefs.begin("talos");
    prefs.putBool("fts", false);
    prefs.putBytes("wifissid", _config.wifiSSID, sizeof(_config.wifiSSID)-1);
    prefs.putBytes("wifipass", _config.wifiPassword, sizeof(_config.wifiPassword)-1);
    
    /* Reset spotify authorizations. */
    prefs.putBool("spotenable", _config.spotifyEnabled);
    prefs.putBool("spotauthed", false);
    prefs.end();

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
    log_i("Beginning Spotify authentication process.");

    /* Add spotify callbacks for the authentication process. */
    std::atomic<bool> finished(false);

    /* Start MDNS for to use talos.local for Spotify config. */
    log_i("Beginning MDNS");
    MDNS.begin("talos");

    _spotify.addAuthCallbacks(_server); 

    /* Wait until the user authenticates the device from the web. */
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