#pragma once

#include <stdint.h>

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
        eTitle,
        eTitleFull,
        eCredit,
        eVersion,
        eError,

        eLoading0,
        eLoading1,
        eLoading2,
        eLoading3,
        eLoading4,

        eErrWifiConFailed,
        eErrWifiConLost,
        eErrWifiDiscon,
        eErrWifiSsidNotFound,
        
        eSolReboot,
        eSolWifiRecon,
        eSolRestartFTS,
        
        eSpotAuthFailed,
        eSpotNowPlaying,
        eSpotWasPlaying,
    };

    /** @brief Copys a hidden static PROGMEM string to a buffer. */
    static void copyTo(Select select, char* pBuf, uint32_t bufMax);
};

} /* namespace ts */
