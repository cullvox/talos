#include <atomic>

#include <esp_ota_ops.h>

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <ESPNtpClient.h>

#include "Config.h"
#include "Pages/Sandbox.h"
#include "Widgets/WidgetSpotify.h"
#include "App.h"

namespace ts {

Talos::Talos()
    : _display()
    , _spotify(_wifiClient, _httpClient, TS_SPOTIFY_CLIENT_ID)
    , _server(80)
{
    memset(_config.wifiSSID, 0, sizeof(_config.wifiSSID));
    memset(_config.wifiPassword, 0, sizeof(_config.wifiPassword));
    memset(_config.wifiIdentity, 0, sizeof(_config.wifiIdentity));
    memset(_config.wifiUsername, 0, sizeof(_config.wifiUsername));
    _config.isWifiEnterprise = false;
}

bool Talos::init()
{
    printBegin("Initializing Devices");
    printStartup();

    /* Setup the first-time-setup interrupt pin. */
    pinMode(TS_PIN_FIRST_TIME_SETUP, INPUT_PULLUP);
    attachInterrupt(TS_PIN_FIRST_TIME_SETUP, Talos::interruptClear, FALLING);

    /* PSRAM is very useful for the Spotify image API and other web things.
        Without it some things have to be cut out... (Spotify album art) */
    log_v("Checking for PSRAM...");

    if (psramInit()) {
        _bHasPsram = true;
        log_v("PSRAM has been initialized");
    } else {
        _bHasPsram = false;
        log_i("PSRAM is not initialized");
    }

    /* SPIFFS stores the setup webpage and fonts. */
    log_v("Mounting SPIFFS...");
    if (!SPIFFS.begin(true)) {
        log_e("Could not mount SPIFFS!");
        return false; /* SPIFFS is required for a functional TALOS device. */
    } else {
        log_v("Mounted.");
    }

    log_i("Is font file found: %d", SPIFFS.exists("/fonts/talos.ttf"));

    _buffer = BitmapAlloc(_display.extent(), _bHasPsram);

    /* Initialize the display and graphics operations. */
    _render.setBitmap(_buffer);
    ffsupport_setffs(SPIFFS);

    log_v("Loading TALOS font...");
    if (!_render.loadFont("/fonts/talos.ttf"))
    {
        log_e("Could not load the icons font!");
        return false; /* The font must be loaded in order to operate TALOS. */
    } else {
        log_v("Loaded font.");
    }

    /* Initialize the ePaper's SPI. */
    SPI.begin(TS_PIN_SPI_CLK, TS_PIN_SPI_CIPO, TS_PIN_SPI_COPI, TS_PIN_PAPER_SPI_CS);
    if (!_display.begin(TS_PIN_PAPER_SPI_CS, TS_PIN_PAPER_RST, TS_PIN_PAPER_DC, TS_PIN_PAPER_BUSY, TS_PIN_PAPER_PWR)) 
    {
        log_i("Could not initialize the ePaper device!\n");
        return false; /* The display is required to be initialized properly. */
    }


    File siegeFile = SPIFFS.open("/siege.bin");

    BitmapAlloc siegeBits{{419, 460}, true};
    siegeFile.readBytes((char*)siegeBits.data(), siegeFile.size());

    _buffer.clear();
    _buffer.blit(siegeBits, {190, 10});
    _display.present(_buffer.data());

    /* Read in the users configuration, if any. */
doFTS:
    readConfig();

    if (_config.spotifyRefreshToken.isEmpty())
        _config.isFirstTimeSetup = true; /* The configuration isn't completely finished. */

    /* Preform the first time setup. */
    if (_config.isFirstTimeSetup) {
        if (!preformFirstTimeSetup()) return false;
        _prefs.begin("talos");
        _prefs.putBool("fts", false);
        _prefs.end();
    }

    /* Attempt to connect to the users Wi-Fi, on fail wait a little and try again. */
    if (!connectToWiFi()) {
        sleep(30);
        ESP.restart();
        return false;
    }

    /* Check if Spotify was correctly and fully authenticated. */
    if (_config.spotifyEnabled && _config.spotifyRefreshToken.isEmpty())
    {
        /* Authenticate again. */
        if (!preformSpotifyAuthorization()) return false;
    } else if (_config.spotifyEnabled) {
        _spotify.setRefreshToken(_config.spotifyRefreshToken.c_str());

        if (!refreshSpotify())
            log_e("Could not refresh Spotify's refresh token!");
    }

    /* Display the splash page. */
    WidgetSpotify spotifyWidget{_spotify};

    spotifyWidget.fetch(_wifiClient);

    _buffer.clear();
    spotifyWidget.render(_render);
    _display.present(_buffer.data());

    printEnd("Initializing Devices");

    return true;
}

bool Talos::run()
{

    /* If the user pressed the reset button we will completely reset the device. */
    if (_bReset) {
        clearConfig();
        ESP.restart();
    }

    return true;
}

void Talos::printBegin(const char* name)
{
    log_printf("\n");
    log_printf("=======================================\n");
    log_printf("BEGIN: %s\n", name);
    log_printf("=======================================\n");
    log_printf("\n");
}

void Talos::printEnd(const char* name)
{
    log_printf("\n");
    log_printf("=======================================\n");
    log_printf("END: %s\n", name);
    log_printf("=======================================\n");
    log_printf("\n");   
}

void Talos::printStartup()
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
}

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    bool canHandle(AsyncWebServerRequest *request) {
        request->addInterestingHeader("ANY");
        return true;
    }
    void handleRequest(AsyncWebServerRequest *request) {
        if (request->method() == HTTP_GET &&
            request->url() == "/css/pico.min.css") {
            log_i("Sending /css/pico.min.css");
            request->send(SPIFFS, "/css/pico.min.css", "text/css");
            return;
        } else if (request->method() == HTTP_GET &&
            request->url() == "/css/font-awesome.min.css") {
                log_i("Sending /css/font-awesome.min.css");
            request->send(SPIFFS, "/css/font-awesome.min.css", "text/css");
            return;
        } else if (request->method() == HTTP_GET &&
            request->url() == "/fonts/fontawesome-webfont.ttf?v=4.7.0") {
            request->send(SPIFFS, "/fonts/fontawesome-webfont.ttf", "text/font");
            return;
        } else if (request->method() == HTTP_GET &&
            request->url() == "/") {
            log_i("Sending /first_time_setup.html");
            request->send(SPIFFS, "/first_time_setup.html");
            return;
        }

        request->send(SPIFFS, "/first_time_setup.html", "text/html");
    }
};

bool Talos::connectToWiFi()
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
    log_i("Connecting to NTP time server: %s", "time.cloudflare.com");

    if (!NTP.begin("time.cloudflare.com", false))
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
bool Talos::preformFirstTimeSetup()
{
    printBegin("First Time Setup");

    /* Clear any previous configuration before first time setup.*/
    clearConfig();

    /* Start the access point. */
    String pass = TS_SECRET_FIRST_TIME_SETUP_PASSWORD;
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
            log_i("GET [%s]: %s\n", p->name().c_str(), p->value().c_str());

            if (p->name() == "wifi_ssid")
                p->value().getBytes((unsigned char*)_config.wifiSSID, sizeof(_config.wifiSSID)-1);
            
            if (p->name() == "wifi_password")
                p->value().getBytes((unsigned char*)_config.wifiPassword, sizeof(_config.wifiPassword)-1);

            if (p->name() == "enable_spotify")
                _config.spotifyEnabled = true; /* data race shouldn't occur here... hopefully.  */
        }
 
        log_i("Recieved Wi-Fi SSID: %s, Password: %s\n", _config.wifiSSID, _config.wifiPassword);

        request->send(SPIFFS, "/setup_complete.html", "text/http");
        finished = true;
    });
 
 
    _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    _server.begin();
 
    while (!finished) { dnsServer.processNextRequest(); yield(); }

    // WiFi.softAPdisconnect(true);

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
    prefs.putString("spotrefresh", "");
    prefs.end();

    log_i("Setup complete!");

    printEnd("First Time Setup");

    return true;
}

bool Talos::preformSpotifyAuthorization()
{
    printBegin("Spotify Authentication Process");

    /* Add spotify callbacks for the authentication process. */
    std::atomic<bool> finished(false);

    /* Start MDNS for to use talos.local for Spotify config. */
    log_v("Beginning MDNS");
    MDNS.begin("talos");

    _server.on("/spotify", [&](AsyncWebServerRequest* request){
        char url[500];
        const char* scopes = "user-read-private+"
                             "user-read-currently-playing+"
                             "user-read-playback-state";

        _wifiClient.setCACert(SpotifyCert::server);
        _spotify.generateRedirectForPKCE(scopes, "http%3A%2F%2Ftalos.local%2Fspotify_callback", url, sizeof(url));

        log_v("Redirecting to Spotify authentication URL: %s", url);
        request->redirect(url);
    });

    const char *refreshToken = NULL;
    _server.on("/spotify_callback", [&](AsyncWebServerRequest* request){                
        AsyncWebParameter* param = request->getParam("code");
        if (!param)
        {
            log_e("User denied Spotify authetication request!");
            request->send(200, "text/plain", "Please reauthenticate, user denied authentication!");
            return;
        }

        String code = param->value();

        log_v("Recieved Spotify code: %s", code.c_str());

        if (!_spotify.requestAccessTokens(code.c_str(), "http%3A%2F%2Ftalos.local%2Fspotify_callback"))
            return;

        finished = true;
    });

    _server.begin();

    /* Wait until the user authenticates the device from the web. */
    log_i("Waiting for Spotify authentication to finish...");
    while (!finished) yield();

    _config.spotifyRefreshToken = _spotify.getRefreshToken();

    _prefs.begin("talos");
    _prefs.putString("spotrefresh", _config.spotifyRefreshToken);
    _prefs.end();

    /* Cleanup server stuffs. */
    _server.end();
    MDNS.end();

    printEnd("Spotify Authentication Process");

    return true;
}

bool Talos::refreshSpotify()
{
    if (!_spotify.checkAndRefreshAccessToken()) return false;

    _config.spotifyRefreshToken = _spotify.getRefreshToken();

    _prefs.begin("talos");
    _prefs.putString("spotrefresh", _config.spotifyRefreshToken);
    _prefs.end();

    return true;
}

void Talos::displayGeneral(Strings::Select severity, Strings::Select primary, Strings::Select secondary)
{
    _slideGeneral.setSeverity(severity);
    _slideGeneral.setPrimary(primary);
    _slideGeneral.setSecondary(secondary);
    _buffer.clear();
    _slideGeneral.render(_render);
    _display.present(_buffer.data());
}

void Talos::interruptClear()
{
    clearConfig();
    ESP.restart();
}

} /* namespace ts */