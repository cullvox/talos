#include <pgmspace.h>
#include <string.h>

#include "Version.h"
#include "Print.h"
#include "FlashStrings.h"

#define TS_PROGMEM_STR(name, value) static const PROGMEM char* name = (value)

namespace ts {
namespace progmem {

/* General */
TS_PROGMEM_STR(Title, "TALOS");
TS_PROGMEM_STR(TitleFull, "Time and Ambient Locale Output System");
TS_PROGMEM_STR(Credit, "by Caden Miller");
TS_PROGMEM_STR(Version, TALOS_VERSION_STRING);

TS_PROGMEM_STR(Loading0, "Warming Up Machines...");
TS_PROGMEM_STR(Loading1, "Thinking...");
TS_PROGMEM_STR(Loading2, "Computing, uh, Numbers...");
TS_PROGMEM_STR(Loading3, "Finding Nirmata...");     // The Creator
TS_PROGMEM_STR(Loading4, "Making Computer Sauces...");

/* Primary */
TS_PROGMEM_STR(Error, "ERROR");
TS_PROGMEM_STR(Err_Wifi_ConFailed, "WiFi: Connection Failed");
TS_PROGMEM_STR(Err_Wifi_ConLost, "WiFi: Connection Lost");
TS_PROGMEM_STR(Err_Wifi_Discon, "WiFi: Disconnected");
TS_PROGMEM_STR(Err_Wifi_SsidNotFound, "WiFi: SSID Not Found");

/* Secondary */
TS_PROGMEM_STR(Sol_Wifi_Recon, "Attempting to Reconnect in 30 Seconds");
TS_PROGMEM_STR(Sol_Reboot, "Rebooting in 30 Seconds");

TS_PROGMEM_STR(NowPlaying, "now playing");
TS_PROGMEM_STR(WasPlaying, "was playing");

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
        case eError: return progmem::Error;

        case eLoading0: return progmem::Loading0;
        case eLoading1: return progmem::Loading1;
        case eLoading2: return progmem::Loading2;
        case eLoading3: return progmem::Loading3;
        case eLoading4: return progmem::Loading4;
        
        case eErr_Wifi_ConFailed: return progmem::Err_Wifi_ConFailed;
        case eErr_Wifi_ConLost: return progmem::Err_Wifi_ConLost;
        case eErr_Wifi_Discon: return progmem::Err_Wifi_Discon; 
        case eErr_Wifi_SsidNotFound: return progmem::Err_Wifi_SsidNotFound; 
        
        case eSol_Reboot: return progmem::Sol_Reboot;
        case eSol_Wifi_Recon: return progmem::Sol_Wifi_Recon;

        case eNowPlaying: return progmem::NowPlaying;
        case eWasPlaying: return progmem::WasPlaying;

        default:
            TS_ERROR("Invalid flashed string selection!");
            return progmem::Error;
        }
    }();

    strncpy_P(buf, str, bufMax);
}

} /* namespace ts */