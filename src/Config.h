#pragma once

#include <Arduino.h>

#include "Numeric.h"

#define TS_STRINGIFY(a) TS_XSTRINGIFY(a)
#define TS_XSTRINGIFY(a) #a


// ======================================
// TALOS VERSION
// ======================================

#define TALOS_VERSION_TAG "beta"
#define TALOS_VERSION_MAJOR 0
#define TALOS_VERSION_MINOR 7
#define TALOS_VERSION_PATCH 0

#define TALOS_VERSION_STRING TALOS_VERSION_TAG "-v" TS_STRINGIFY(TALOS_VERSION_MAJOR) "." TS_STRINGIFY(TALOS_VERSION_MINOR) "." TS_STRINGIFY(TALOS_VERSION_PATCH)
#define TALOS_VERSION_CODE (uint32_t)(TALOS_VERSION_TAG[0]) << 24 | \
                            (uint32_t)(TALOS_VERSION_MAJOR) << 16 | \
                            (uint32_t)(TALOS_VERSION_MINOR) << 8 | \
                            (uint32_t)(TALOS_VERSION_PATCH);

/* Any git info is gathered at pre-build time. */
#ifndef TALOS_VERSION_GIT_TAG
#define TALOS_VERSION_GIT_TAG ""
#endif

#ifndef TALOS_VERSION_GIT_HASH
#define TALOS_VERSION_GIT_HASH ""
#endif

#ifndef TALOS_VERSION_GIT_HASH_SHORT
#define TALOS_VERSION_GIT_HASH_SHORT ""
#endif

#define TALOS_VERSION_STRING_GIT TALOS_VERSION_GIT_TAG " " TALOS_VERSION_GIT_HASH_SHORT

// ======================================
// TALOS CONFIG
// ======================================

#define TS_HOSTNAME "talos"
#define TS_MAX_WIFI_CONNECTION_TIMEOUT 30
#define TS_MAX_NTP_CONNECTION_TIMEOUT 15
#define TS_SECRET_FIRST_TIME_SETUP_PASSWORD "123456789"

/* The TALOS Spotify client ID */
#define TS_SPOTIFY_CLIENT_ID "bb15029eaeea4bd086008584ae1fbaf7" 

/* General SPI Pins */
#define TS_PIN_SPI_CLK 5
#define TS_PIN_SPI_CIPO 21
#define TS_PIN_SPI_COPI 19

/* SPI/Pins ePaper Display */
#define TS_PIN_PAPER_SPI_CS 12
#define TS_PIN_PAPER_DC 27
#define TS_PIN_PAPER_RST 33
#define TS_PIN_PAPER_BUSY 15
#define TS_PIN_PAPER_PWR 13

#define TS_PIN_FIRST_TIME_SETUP 38

namespace ts {

class Config {
    bool _isFirstTimeSetup;
    String _wifiSsid;
    String _wifiPassword;
    bool _isSpotifyEnabled;
    bool _isSpotifyAuthorized;
    String _spotifyRefreshToken;
    s32 _timeOffset;
    bool _use24HourClock;

public:
    Config()
        : _isFirstTimeSetup(true) {}
    
    ~Config() = default;

    void load();        /* loads values from preferences into memory.*/
    void save();        /* saves values in memory to the preferences. */
    void clear();       /* clears all config values from preferences and in memory. */

    void setFirstTimeSetup(bool isFirstTimeSetup) { _isFirstTimeSetup = isFirstTimeSetup; } 
    void setWifiSsid(const String& wifiSsid) { _wifiSsid = wifiSsid; }
    void setWifiPassword(const String& wifiPassword) { _wifiPassword = wifiPassword; }
    void setSpotifyEnabled(bool isSpotifyEnabled) { _isSpotifyEnabled; }
    void setSpotifyAuthorized(bool isSpotifyAuthorized) { _isSpotifyAuthorized; }
    void setSpotifyRefreshToken(const String& spotifyRefreshToken) { _spotifyRefreshToken; }
    void setTimeOffset(s32 timeOffset) { _timeOffset = timeOffset; }
    void setUse24HourClock(bool use24HourClock) { _use24HourClock = use24HourClock; }

    bool getFirstTimeSetup() const { return _isFirstTimeSetup; };
    bool getSpotifyEnabled() const { return _isSpotifyEnabled; }
    bool getSpotifyAuthorized() const { return _isSpotifyAuthorized; }
    const String& getWifiSsid() const { return _wifiSsid; }
    const String& getWifiPassword() const { return _wifiPassword; }
    const String& getSpotifyRefreshToken() const { return _spotifyRefreshToken; }
    s32 getTimeOffset() const { return _timeOffset; }
    bool getUse24HourClock() const { return _use24HourClock; }
};

} /* namespace ts */