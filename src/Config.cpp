
#include <Preferences.h>
  
#include "Config.h"

namespace ts {

void Config::load()
{
    Preferences prefs;
    prefs.begin("talos");
    _isFirstTimeSetup = prefs.getBool("fts", true);
    _wifiSsid = prefs.getString("wifissid", "");
    _wifiPassword = prefs.getString("wifipass", "");
    _isSpotifyEnabled = prefs.getBool("spotenable", false);
    _spotifyRefreshToken = prefs.getString("spotrefresh", "");
    _timeOffset = prefs.getLong("timeoffset", 0);
    _use24HourClock = prefs.getBool("24hrclock", false);

    log_v("config: fts =config. %s", _isFirstTimeSetup ? "true" : "false");
    log_v("config: wifissid = %s", _wifiSsid.c_str());
    log_v("config: wifipass = %s", _wifiPassword.c_str());
    log_v("config: spotenabled = %d", _isSpotifyEnabled);
    log_v("config: spotrefresh = %s", _spotifyRefreshToken.c_str());
    log_v("config: timeoffset = %d", _timeOffset);
    log_v("config: 24hrclock = %s", _use24HourClock ? "true" : "false");

    prefs.end();
}

void Config::save()
{
    Preferences prefs;
    prefs.begin("talos");
    
    prefs.putBool("fts", _isFirstTimeSetup);
    prefs.putString("wifissid", _wifiSsid);
    prefs.putString("wifipass", _wifiPassword);
    prefs.putBool("spotenable", _isSpotifyEnabled);
    prefs.putString("spotrefresh", _spotifyRefreshToken);
    prefs.putLong("timeoffset", _timeOffset);
    prefs.putBool("24hrclock", _use24HourClock);

    prefs.end();
}

void Config::clear()
{   
    Preferences prefs;
    prefs.begin("talos");
    prefs.clear();
    prefs.end();

    _isFirstTimeSetup = true;
    _wifiSsid = "";
    _wifiPassword = "";
    _isSpotifyEnabled = false;
    _spotifyRefreshToken = "";
    _timeOffset = 0;
    _use24HourClock = false;
}

} /* namespace ts */
