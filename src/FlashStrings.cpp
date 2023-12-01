#include <pgmspace.h>
#include <string.h>

#include "Version.h"
#include "Print.h"
#include "FlashStrings.h"


#define TS_PROGMEM_STR(name, value) \
    static const PROGMEM char* name = (value); \
    static_assert(sizeof(name) <= TS_FLASH_STRINGS_MAX_LENGTH)

namespace ts {
namespace progmem {

/* General */
TS_PROGMEM_STR(Title, "TALOS");
TS_PROGMEM_STR(TitleFull, "Time and Ambient Locale Output System");
TS_PROGMEM_STR(Credit, "by Caden Miller");
TS_PROGMEM_STR(Version, TALOS_VERSION_STRING);

TS_PROGMEM_STR(SetupBeginning, "FIRST TIME SETUP");
TS_PROGMEM_STR(SetupJoinWifi, "To start join TALOS_##'s WiFi on your device");
TS_PROGMEM_STR(SetupAlmostDone, "You're almost done");
TS_PROGMEM_STR(SetupSpotToContinue, "To continue go to talos.local/spotify to authenticate."); 

TS_PROGMEM_STR(Loading0, "Warming Up Machines...");
TS_PROGMEM_STR(Loading1, "Thinking...");
TS_PROGMEM_STR(Loading2, "Computing, uh, Numbers...");
TS_PROGMEM_STR(Loading3, "Finding Nirmata..."); /* The Creator */
TS_PROGMEM_STR(Loading4, "Making Computer Sauces...");

TS_PROGMEM_STR(SeverityError, "ERROR");
TS_PROGMEM_STR(SeverityInfo, "INFO");

/* Primary */
TS_PROGMEM_STR(ErrWifiConFailed, "WiFi Connection Failed");
TS_PROGMEM_STR(ErrWifiConLost, "WiFi Connection Lost");
TS_PROGMEM_STR(ErrWifiDiscon, "WiFi Disconnected");
TS_PROGMEM_STR(ErrWifiSsidNotFound, "WiFi SSID Not Found");
TS_PROGMEM_STR(ErrSpotAuthFailed, "Spotify authentication failed");

/* Secondary */
TS_PROGMEM_STR(SolWifiRecon, "Attempting to reconnect in 30 seconds");
TS_PROGMEM_STR(SolReboot, "Rebooting in 30 seconds");
TS_PROGMEM_STR(SolRestartFTS, "Restarting first time setup in a moment");

TS_PROGMEM_STR(PlaybackNowPlaying, "now playing");
TS_PROGMEM_STR(PlaybackWasPlaying, "was playing");

} /* namespace progmem */


void Strings::copyTo(Select select, char* buf, uint32_t bufMax)
{
    const char* str = [select]()
    {
        switch (select)
        {
        case eTitle: return progmem::Title;
        case eTitleFull: return progmem::TitleFull;
        case eCredit: return progmem::Credit;
        case eVersion: return progmem::Version;

        case eSetupBeginning: return progmem::SetupBeginning;
        case eSetupJoinWifi: return progmem::SetupJoinWifi;
        case eSetupAlmostDone: return progmem::SetupAlmostDone;
        case eSetupSpotToContinue: return progmem::SetupSpotToContinue;

        case eLoading0: return progmem::Loading0;
        case eLoading1: return progmem::Loading1;
        case eLoading2: return progmem::Loading2;
        case eLoading3: return progmem::Loading3;
        case eLoading4: return progmem::Loading4;

        case eSeverityError: return progmem::SeverityError;
        case eSeverityInfo: return progmem::SeverityInfo;
        
        case eErrWifiConFailed: return progmem::ErrWifiConFailed;
        case eErrWifiConLost: return progmem::ErrWifiConLost;
        case eErrWifiDiscon: return progmem::ErrWifiDiscon; 
        case eErrWifiSsidNotFound: return progmem::ErrWifiSsidNotFound;
        case eErrSpotAuthFailed: return progmem::ErrSpotAuthFailed; 
        
        case eSolReboot: return progmem::SolReboot;
        case eSolWifiRecon: return progmem::SolWifiRecon;
        case eSolRestartFTS: return progmem::SolRestartFTS;

        case ePlaybackNowPlaying: return progmem::PlaybackNowPlaying;
        case ePlaybackWasPlaying: return progmem::PlaybackWasPlaying;

        default:
            TS_ERROR("Invalid flashed string selection!");
            return progmem::SeverityError;
        }
    }();

    strncpy_P(buf, str, bufMax);
}

} /* namespace ts */