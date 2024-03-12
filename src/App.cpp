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
#include "UI/Layout.h"
#include "UI/VerticalBox.h"
#include "UI/HorizontalBox.h"
#include "UI/Label.h"
#include "App.h"

namespace ts {

Talos::Talos()
    : _display()
    , _spotify(_wifiClient, _httpClient, TS_SPOTIFY_CLIENT_ID)
    , _server(80)
    , pageSandbox(_spotify)
{
}

bool Talos::init()
{
    printBegin("Initializing Talos");
    printStartup();


    /* Setup the first-time-setup interrupt pin. */
    pinMode(TS_PIN_FIRST_TIME_SETUP, INPUT_PULLUP);
    attachInterruptArg(TS_PIN_FIRST_TIME_SETUP, Talos::interruptClear, this, FALLING);

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

    /* Read in the users configuration, if there is any the defaults will be overridden. 
        FTS will be preformed if the default FTS value is still present.*/
    log_v("Loading config...");
    _config.load();
    log_v("Loaded config.");

    /* Allocate memory for the display bitmap early so that we always
        have enough memory for it. Some instances especially with larger 
        fonts or lots of spotify data we don't want to allocate this any later.*/
    _buffer = BitmapAlloc(_display.extent(), _bHasPsram);

    log_v("Loading SPIFFS Talos font...");

    /* The font renderer needs to know the device to load fonts from.*/
    ffsupport_setffs(SPIFFS);

    if (!_render.loadFont("/fonts/talos.ttf"))
    {
        log_e("Could not load the talos font!");
        return false; /* The font must be loaded in order to operate TALOS. */
    } else {
        log_v("Loaded font.");
    }

    /* Give our large display bitmap to the renderer so it can draw to it. */
    _render.setBitmap(_buffer);

    /* Setup the electronic paper's SPI output settings. */
    SPI.begin(TS_PIN_SPI_CLK, TS_PIN_SPI_CIPO, TS_PIN_SPI_COPI, TS_PIN_PAPER_SPI_CS);
    if (!_display.begin(TS_PIN_PAPER_SPI_CS, TS_PIN_PAPER_RST, TS_PIN_PAPER_DC, TS_PIN_PAPER_BUSY, TS_PIN_PAPER_PWR)) 
    {
        /* Something really went wrong! Check your SPI pins and ensure they are correctly placed. */
        log_i("Could not initialize the ePaper device!\n");
        return false; 
    }

    /* Draw a nice little logo on every boot. This will ensure the screen is clean too. */
    File siegeFile = SPIFFS.open("/siege.bin");

    BitmapAlloc siegeBits{{419, 460}, true};
    siegeFile.readBytes((char*)siegeBits.data(), siegeFile.size());

    _buffer.clear();
    _buffer.blit(siegeBits, {190, 10});
    
    _render
        .setCursor({550, 330})
        .setFontSize(60)
        .setFillColor(Color::eBlack)
        .setOutlineColor(Color::eWhite)
        .drawText("for siege")
        .seekCursor({0, 50})
        .setFontSize(35)
        .drawText("by cullvox");

    _display.present(_buffer.data());

    /* Do a first time setup if it has not occurred yet. */
    if (_config.isFirstTimeSetup()) {
        
        if (!preformFirstTimeSetup()) return false;
        
        _prefs.begin("talos");
        _prefs.putBool("fts", false);
        _prefs.end();
    }

    /* Sometimes Wi-Fi can be a little spotty so we always check for Wi-Fi when running too. 
        That leads this to not being an error when we would usually think of it as one.
        However Talos will only reset the users Wi-Fi secrets when they request it (from the button). */
    connectToWiFi();

    /* And we're done! */
    printEnd("Initializing");
    return true;
}

void Talos::run()
{

    /* Check if the user pressed the reset button while the device was asleep. */
    if (_bReset) {
        /* This will bring the device back to a factory reset state 
            and initialize into FTS. */
        _config.clear();
        ESP.restart();
    }

    /* The display is powered down before after every run, so we reset
        it before every run. Otherwise the display would could get some
        nasty burn in. */
    _display.reset();

    /* Ensure we are connected to the internet for this run. */
    if (!WiFi.isConnected()) {
        if (!connectToWiFi()) {
            log_e("Could not connect to WiFi, skipping page for now.");
            return; /* wait until next run?... */
        }
    }

    /* Check if the user was authenticated with spotify yet. */
    if (_config.getSpotifyEnabled() && _config.getSpotifyRefreshToken().isEmpty()) {

        if (!preformSpotifyAuthorization()) {
         
            /* Not being able to authenticate is an error that the 
                user should probably reset their config for. */
            // displayGeneral();
            return; 
        }
    } else if (_config.getSpotifyEnabled()) {

        /* Use the saved refresh token. */
        _spotify.setRefreshToken(_config.getSpotifyRefreshToken().c_str());

        /* Try to refresh the token now. */
        if (!refreshSpotify()) {
            log_e("Could not refresh Spotify's refresh token!");
            return;
        }
    }

    /* Display the splash page. */
    _wifiClient.setCACert(SpotifyCert::server);
    pageSandbox.fetch(_wifiClient);

    _buffer.clear();
    pageSandbox.render(_render);
    _display.present(_buffer.data());

    _display.sleep();

    sleep(180);
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


/* This captive request handler hooks into the AsyncWebServer and 
    pulls all requests quickly and easily that creates a Captive Portal! */
class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    bool canHandle(AsyncWebServerRequest *request) {
        request->addInterestingHeader("ANY"); /* This makes it CAPTIVE! */
        return true;
    }

    /* Handles and sends all requests required for the setup site. 
        This could probably be cleaned up to be more generalized but for now
        hardcoding all the files ensures that there are no slip ups. */
    void handleRequest(AsyncWebServerRequest *request) {
        if (request->method() == HTTP_GET && request->url() == "/css/pico.min.css") {
            request->send(SPIFFS, "/css/pico.min.css", "text/css");
            return;
        } else if (request->method() == HTTP_GET && request->url() == "/css/font-awesome.min.css") {
            request->send(SPIFFS, "/css/font-awesome.min.css", "text/css");
            return;
        } else if (request->method() == HTTP_GET && request->url() == "/fonts/fontawesome-webfont.ttf?v=4.7.0") {
            request->send(SPIFFS, "/fonts/fontawesome-webfont.ttf", "text/font");
            return;
        } else if (request->method() == HTTP_GET && request->url() == "/") {
            request->send(SPIFFS, "/first_time_setup.html");
            return;
        }

        request->send(SPIFFS, "/first_time_setup.html", "text/html");
    }
};

bool Talos::connectToWiFi()
{
    log_i("Initiating connection to WiFi: SSID: %s", _config.getWifiSsid());

    /* Clear any previous WiFi settings. */
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);

    /* Disable automatic reconnect so we can display an 
        error message to the user properly. */
    WiFi.setAutoReconnect(false); 
    
    /* Begin the WiFi! */
    WiFi.begin(_config.getWifiSsid(), _config.getWifiPassword());

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
            log_e("Wifi SSID: %s was not found.", _config.getWifiSsid());
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

        case WL_SCAN_COMPLETED: /* We aren't scanning right now... why? */
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
        /* TODO: Display an error message on screen for connection failure. */
        return false;
    }

    /* Time sync to the current UTC time on the internet.
        This also acts a test of the connection. */
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

    log_i("Current internet time: %s", NTP.getTimeDateString());
    return true;

Err_ConnectFailed:
    // TODO: Display an internet connection error.
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

    /* Clear any previous configuration before starting the first time setup.*/
    _config.clear();

    /* Generate a random password for the WiFi AP. */
    char dict[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*_";
    char password[12] = {0};
    
    for (int i = 0; i < 8; i++)
        password[i] = dict[random(sizeof(dict)-1)];

    log_i("WiFi AP password: %s", password);

    /* Start the access point. */
    String ssid = F("TALOS_");
    ssid.concat(WiFi.macAddress().substring(6, 8));

    WiFi.softAP(ssid, password);

    /* Start the DNS server to catch all for the captive portal. */
    DNSServer dnsServer;
    dnsServer.start(53, "*", WiFi.softAPIP());

    /* TODO: Display the actual SSID in displayGeneral some how. */
    displayGeneral(Strings::eSeverityInfo, Strings::eSetupBeginning, Strings::eSetupJoinWifi);
    
    std::atomic<bool> finished(false);

    /* Lambda called when the web site sends the GET request to 
        submit all the values from setup. */
    _server.on("/done", HTTP_GET, [&](AsyncWebServerRequest* request){
        
        /* Retrieve the WiFi SSID, password, username and identity. */
        int params = request->params();
        for (int i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            log_i("GET [%s]: %s\n", p->name().c_str(), p->value().c_str());

            if (p->name() == "wifi_ssid")           _config.setWifiSsid(p->value());
            if (p->name() == "wifi_password")       _config.setWifiPassword(p->value());
            if (p->name() == "enable_spotify")      _config.setSpotifyEnabled(true);
            if (p->name() == "utc_offset")          _config.setTimeOffset(p->value().toInt());
            if (p->name() == "enable_24_hour_time") _config.setUse24HourClock(true);
        }
 
        log_i("Recieved Wi-Fi SSID: %s, Password: %s\n", _config.getWifiSsid(), _config.getWifiPassword());

        request->send(SPIFFS, "/setup_complete.html", "text/http");
        finished = true;
    });
 
    
    /* Create a captive request handler and finally begin the web server for FTS. */
    _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    _server.begin();
 
    /* Wait until the user completes FTS. The web server uses other threads, 
        while the DNS server does its thing in this thread so we manually update it.  */
    while (!finished) { dnsServer.processNextRequest(); yield(); }

    // WiFi.softAPdisconnect(true);

    dnsServer.stop();
    _server.reset();

    /* Finally save the Wi-Fi credentials into the config. */
    _config.save();

    log_i("Setup complete!");
    printEnd("First Time Setup");

    return true;
}

bool Talos::preformSpotifyAuthorization()
{
    printBegin("Spotify Authentication");

    /* Add spotify callbacks for the authentication process. */
    std::atomic<bool> finished(false);

    /* Start MDNS for to use talos.local for Spotify config. */
    log_v("Beginning MDNS");
    MDNS.begin("talos");

    /* We are doing Spotify's PKCE Authentication method. This means we don't require
        a client secret in order to access Spotify's APIs. It's much safer than storing
        the client secret on this device that any user or developer could easily see by
        reverse engineering. */
    const char* TALOS_SPOTIFY_CALLBACK_URL = "http%3A%2F%2Ftalos.local%2Fspotify_callback";
    _server.on("/spotify", [&](AsyncWebServerRequest* request){
        char url[500];
        const char* scopes = "user-read-private+"
                             "user-read-currently-playing+"
                             "user-read-playback-state";

        _wifiClient.setCACert(SpotifyCert::server);
        _spotify.generateRedirectForPKCE(scopes, TALOS_SPOTIFY_CALLBACK_URL, url, sizeof(url));

        log_v("Redirecting to Spotify authentication URL: %s", url);
        request->redirect(url);
    });

    _server.on("/spotify_callback", [&](AsyncWebServerRequest* request){                
        AsyncWebParameter* param = request->getParam("code");
        if (!param) {
            log_e("User denied Spotify authetication request!");
            request->send(200, "text/plain", "Please reauthenticate, user denied authentication!");
            return;
        }

        String code = param->value();
        log_v("Received Spotify code: %s", code.c_str());

        if (!_spotify.requestAccessTokens(code.c_str(), TALOS_SPOTIFY_CALLBACK_URL))
            return; /* The access token request failed! */

        /* Authentication has been completed! */
        finished = true;
    });

    /* Begin the web server to receive the Spotify tokens. */
    _server.begin();

    /* Wait until the user authenticates the device from the web. */
    log_i("Waiting for Spotify authentication to finish...");
    while (!finished) yield();

    /* Save the refresh token to the config and cleanup. */
    _config.setSpotifyRefreshToken(_spotify.getRefreshToken());
    _config.save();

    _server.end();
    MDNS.end();

    printEnd("Spotify Authentication");
    return true;
}

bool Talos::refreshSpotify()
{
    /* Set the WiFi Client to the Spotify Cert and then see if 
        we should request another access token. */
    _wifiClient.setCACert(SpotifyCert::server);
    if (!_spotify.checkAndRefreshAccessToken()) return false;

    /* Save the refresh token to the config. */
    _config.setSpotifyRefreshToken(_spotify.getRefreshToken());
    _config.save();

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

void Talos::interruptClear(void* arg)
{
    /* This function is called whenever the button is pushed, sometimes it takes a 
        little while to actually call. (usually occurs after the device wakes back up)*/
    Talos* talos = (Talos*)arg;
    talos->_bReset = true;
}

} /* namespace ts */