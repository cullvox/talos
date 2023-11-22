#pragma once

/* Make a copy of this file and call it "Secrets.h" to enable.
    This API is only temporary and will be removed in
    beta-0.7.0 or later opting in for a configuration system. */


#define TS_SECRET_WIFI_USE_ENTERPRISE   0
#define TS_SECRET_WIFI_WAP2_AUTH        WPA2_AUTH_PEAP 
#define TS_SECRET_WIFI_SSID             "MyNetwork"
#define TS_SECRET_WIFI_PASSWORD         "password1234"
#define TS_SECRET_WIFI_USERNAME         "myusername@enterprise.com"
#define TS_SECRET_WIFI_IDENTITY         "myusername@enterprise.com"

#define TS_SECRET_LASTFM_USER           "yourusername"
#define TS_SECRET_LASTFM_KEY            "somelargestringofnumbersandletters"