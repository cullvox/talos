#pragma once

#include <stdint.h>

#include "Prone.h"

namespace ts {

class Network : public Prone {
public:

    enum class Authorization : uint32_t {
        eOpen         = 0,           /** @brief No authorisation required (open) */
        eWpaTkipPsk   = 0x00200002,  /** @brief WPA authorisation */
        eWpa2AesPsk   = 0x00400004,  /** @brief WPA2 authorisation (preferred) */
        eWpa2MixedPsk = 0x00400006,  /** @brief WPA2/WPA mixed authorisation */
    };

    struct WifiScanResult {
        uint8_t     ssidLength;
        char        ssid[32];
        uint8_t     accessPointMacAddress[8]; 
        uint16_t    channel;  
        uint8_t     auth_mode;
        int16_t     rssi; 
    };

    typedef void (* FuncOnRequestReceive)(uint32_t bufferLength, const uint8_t* pBuffer, void* pUserData);

    bool _bWifiConnected = false;

public:

    Network();
    ~Network();

    /** @brief We only allow responses to be a certain size because of memory 
        constraints on a Pico W. If there was more memory that would be
        better and the requirement could be lifted in time. The current 
        limit is 24 kb. */
    static inline const uint16_t maxHttpResponseSize = 24576; 


    /** @brief Enables WiFi. */
    void enableStationMode();

    /** @brief Scans for WiFi to connect to. */
    void blockingScanWifi(uint32_t msTimeout = 5000);

    /** @brief Connects to a WiFi, returns false on failure. */
    bool blockingConnectWifi(const char* pSSID, const char* pPassword, uint32_t msTimeout = 30000, Authorization auth = Authorization::eWpa2AesPsk);
    
    /** @brief Returns true if the wifi signal is currently established. */
    bool isWifiConnected() const { return _bWifiConnected; }

    /** @brief Sends an HTTP request to a server and returns the content of its response.
     *  @param pRequest The request to send out.
     *  @param contentLength The amount of bytes received from the response.
     *  @param headerOffset Add this value to the return value to get the start of the actual contents.
     *  @param msTimeout How long to wait for a response from the server.
     *  @param pCert Bytes used in the certification, can be nullptr.
     *  @param certLength Amount of bytes in the @ref pCert param, can be 0.
     *  @return uint8_t* Response content, will be nullptr on failure. 
    */
    uint8_t* blockingHttpRequest(const char* pServer, const char* pRequest, uint16_t& contentLength, uint16_t& headerOffset, uint32_t msTimeout = 15000, const uint8_t* pCert = nullptr, uint32_t certLength = 0);
    
    void poll();

    void enumerateWifi(uint32_t* pWifiScanCount, WifiScanResult*);

};

} /* namespace ts */


