#pragma once

#include <stdint.h>

#define TS_FLASH_STRINGS_MAX_LENGTH 128 /* Does not include null-terminator. */

namespace ts {



/** @brief Holds and provides function for strings stored in flash memory.
 * 
 *  If you wish to change any string values that are flashed 
 *  go into FlashStrings.cpp to change them.
 * 
 */
class Strings {
public:

    /** @brief Enum values representing valid string responses. */
    enum Select {
        eTitle,         /* TALOS */
        eTitleFull,     /* Time and Ambient Locale Output System*/
        eCredit,        /* by Caden Miller */
        eVersion,

        eSetupBeginning,      /* First Time Setup */
        eSetupJoinWifi,       /* To start join TALOS_##'s WiFi on your device */
        eSetupAlmostDone,     /* You're almost done */
        eSetupSpotToContinue, /* To finish setup login to Spotify */

        eLoading0,
        eLoading1,
        eLoading2,
        eLoading3,
        eLoading4,

        eSeverityError, /* ERROR */
        eSeverityInfo,  /* INFO */

        eErrWifiConFailed,
        eErrWifiConLost,
        eErrWifiDiscon,
        eErrWifiSsidNotFound,
        eErrSpotAuthFailed,
        
        eSolReboot,
        eSolWifiRecon,
        eSolRestartFTS,
        
        ePlaybackNowPlaying,
        ePlaybackWasPlaying,
    };

    /** @brief Copys a hidden static PROGMEM string to a buffer. */
    static void copyTo(Select select, char* pBuf, uint32_t bufMax);
};

} /* namespace ts */
