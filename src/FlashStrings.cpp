#include <pgmspace.h>
#include <string.h>

#include <Arduino.h>

#include "Config.h"
#include "FlashStrings.h"





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
            log_e("Invalid flashed string selection!");
            return progmem::SeverityError;
        }
    }();

    strncpy_P(buf, str, bufMax);
}

} /* namespace ts */