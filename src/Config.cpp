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

    log_v("config: fts =config. %s", _isFirstTimeSetup ? "true" : "false");
    log_v("config: wifissid = %s", _wifiSsid.c_str());
    log_v("config: wifipass = %s", _wifiPassword.c_str());
    log_v("config: spotenabled = %d", _isSpotifyEnabled);
    log_v("config: spotrefresh = %s", _spotifyRefreshToken.c_str());

    // log_i("config: spotrefreshtime = %d")
    prefs.end();
}

void Config::save()
{
    Preferences prefs;
    prefs.begin("talos");
    
    prefs.putBool("fts", _isFirstTimeSetup);
    prefs.putString("wifissid", _wifiSsid );
    prefs.putString("wifipass", _wifiPassword );
    prefs.putBool("spotenable", _isSpotifyEnabled);
    prefs.putString("spotrefresh", _spotifyRefreshToken );

    prefs.end();
}

void Config::clear()
{   
    Preferences prefs;
    prefs.begin("talos");
    prefs.clear();
    prefs.end();
}

} /* namespace ts */
