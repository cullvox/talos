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
        TS_INFO("Sending first time setup page to browser.\n");
        request->send(SPIFFS, "/first_time_setup.html", "text/html");
        // request->send_P(200, "text/html", responseHTML); 
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

startFirstTimeSetup:
    TS_INFO("\n");
    TS_INFO("=======================================\n");
    TS_INFO("BEGIN: First Time Setup\n");
    TS_INFO("=======================================\n");
    TS_INFO("\n");

    /* Display to the user that FTS is about to happen. */
    displayGeneral(Strings::eSeverityInfo, Strings::eSetupBeginning, Strings::eSetupJoinWifi);

    /* Start the access point. */
    String pass = "ABC1234567";
    String ssid = F("TALOS_");
    ssid.concat(WiFi.macAddress().substring(3, 5));

    WiFi.softAP(ssid, pass);

    /* Start the DNS server to catch all for the captive portal. */
    DNSServer dnsServer;
    dnsServer.start(53, "*", WiFi.softAPIP());


    /* TODO: Display the actual SSID in displayGeneral some how. */
    
  

    
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
 
     while (!finished) { dnsServer.processNextRequest(); }
 
     WiFi.softAPdisconnect(true);

    dnsServer.stop();
    _server.reset();

    /* Attempt to connect to the persons wifi. */
    if (!connectToWiFi()) {
        TS_INFO("Could not connect to WiFi during first time setup, configuring again.\n");
        goto startFirstTimeSetup; /* connectToWiFi will display an error. */
    }

    /* Establish authorization with spotify. */
    if (_config.spotifyEnabled)
    {
        if (!preformSpotifyAuthorization())
        {
            /* Display to the user that first time setup is restarting. */
            displayGeneral(Strings::eSeverityError, Strings::eErrSpotAuthFailed, Strings::eSolRestartFTS);    
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

bool App::preformSpotifyAuthorization()
{
    log_d("Beginning Spotify authentication process");

    /* Add spotify callbacks for the authentication process. */
    std::atomic<bool> finished(false);

    /* Start MDNS for to use talos.local for Spotify config. */
    log_d("Beginning MDNS");
    MDNS.begin("talos");

    _server.on("/spotify", HTTP_GET, [](AsyncWebServerRequest* request){
        
        /* Generate a random state value. */
        const int codeVerifierLength = 64;
        char codeVerifier[codeVerifierLength+1];
        
        generateSpotifyCodeVerifier(codeVerifier, codeVerifierLength);

        char codeVerifierSha[32];

        mbedtls_sha256_context ctx;
        mbedtls_sha256_init(&ctx);
        mbedtls_sha256_starts(&ctx, false);
        mbedtls_sha256_update(&ctx, (unsigned char*)codeVerifier, codeVerifierLength);
        mbedtls_sha256_finish(&ctx, (unsigned char*)codeVerifierSha);
        mbedtls_sha256_free(&ctx);

        String codeVerifierShaBase64 = base64::encode((uint8_t*)codeVerifierSha, sizeof(codeVerifierSha));
        codeVerifierShaBase64.replace('+', '-');
        codeVerifierShaBase64.replace('/', '_');

        char spotifyCodeChallenge[44] = "";
        strncpy(spotifyCodeChallenge, codeVerifierShaBase64.c_str(), 43);

        /* Build the Spotify authentication URL and redirect the user there. */
        String url;
        url.reserve(500);
        url.concat(
            F("https://accounts.spotify.com/authorize/?"
            "response_type=code"
            "&client_id=" TS_SPOTIFY_CLIENT_ID
            "&scope="
                "user-read-private+"
                "user-read-currently-playing+"
                "user-read-playback-state"
            "&redirect_uri=http%3A%2F%2Ftalos.local/spotify_callback"
            "&code_challenge_method=S256"
            "&code_challenge="));
        url.concat(spotifyCodeChallenge);

        log_i("Spotify authorization redirect generated: %s", url.c_str());

        request->redirect(url);
    });

    _server.on("/spotify_callback", HTTP_GET, [&finished](AsyncWebServerRequest* request){

        TS_INFO("Received callback from Spotify\n");

        /* Retrieve the spotify access code. */
        AsyncWebParameter* code = request->getParam("code");
        if (!code)
        {
            TS_ERROR("Could not find Spotify access code in callback!");
            return;
        }
        
        String spotifyCode = code->value();
        TS_INFOF("Received authorization code from Spotify: %s\n", spotifyCode.c_str());

        finished = true;
    });

    _server.on("/", [](AsyncWebServerRequest* req){
        return;
    });

    /* Wait until authentication is complete. */
    while (!finished) { }

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

void App::generateSpotifyCodeVerifier(char* codeVerifier, uint32_t codeLength)
{
    const char* possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (int i = 0; i < codeLength; i++)
    {
        uint32_t index = esp_random() % (sizeof(possible)-1);
        codeVerifier[i] = possible[i];
    }
}

} /* namespace ts */