#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "Print.h"
#include "Network.h"


namespace ts {

Network::Network()
{
//    if (cyw43_arch_init() == 0)
//    {
//        TS_ERROR("Could not initialize cyw43-arch!");
//        _what = Result::eCyw43Error;
//        return;
//    }
//
//    _what = Result::eOK;
    

}

Network::~Network()
{
//    cyw43_arch_deinit();
}

void Network::enableStationMode()
{
//    cyw43_arch_enable_sta_mode();
}

//static int scan_result(void *env, const cyw43_ev_scan_result_t *result) 
//{
//    (void)env;
//
//    if (result) {
//        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
//            result->ssid, result->rssi, result->channel,
//            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
//            result->auth_mode);
//    }
//    return 0;
//}

void Network::blockingScanWifi(uint32_t msScanTimeout) 
{
//    (void)msScanTimeout;
//
//    absolute_time_t scan_test = nil_time;
//    bool scan_in_progress = false;
//    while(true) {
//        if (absolute_time_diff_us(get_absolute_time(), scan_test) < 0) {
//            if (!scan_in_progress) {
//                cyw43_wifi_scan_options_t scan_options = {};
//                int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
//                if (err == 0) {
//                    printf("\nPerforming wifi scan\n");
//                    scan_in_progress = true;
//                } else {
//                    printf("Failed to start scan: %d\n", err);
//                    scan_test = make_timeout_time_ms(10000); // wait 10s and scan again
//                }
//            } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
//                scan_test = make_timeout_time_ms(10000); // wait 10s and scan again 
//                scan_in_progress = false; 
//            }
//        }
//
//        cyw43_arch_poll();
//        sleep_ms(1);
//    }
}

bool Network::blockingConnectWifi(const char* pSSID, const char* pPassword, uint32_t msTimeout, Authorization auth)
{
    WiFi.begin(pSSID, pPassword);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
}

// static struct altcp_tls_config* pTlsConfig = nullptr;

// struct TlsClient
// {
// public:
//     struct altcp_pcb*   pPcb;
//     bool                complete;
//     int                 error;
//     const char*         pHttpRequest;
//     int                 timeout;
//     struct pbuf*        pPacketsHead;
//     struct pbuf*        pPacketsTail;
// 
//     static err_t onClose(void* pUser)
//     {
//         TlsClient* pState = (TlsClient*)pUser;
//         err_t err = ERR_OK;
// 
//         pState->complete = true;
//         if (pState->pPcb != NULL) {
//             altcp_arg(pState->pPcb, NULL);
//             altcp_poll(pState->pPcb, NULL, 0);
//             altcp_recv(pState->pPcb, NULL);
//             altcp_err(pState->pPcb, NULL);
//             altcp_tls_free_entropy();
//             err = altcp_close(pState->pPcb);
//             if (err != ERR_OK) {
//                 TS_ERRORF("close failed %d, calling abort\n", err);
//                 altcp_abort(pState->pPcb);
//                 err = ERR_ABRT;
//             }
//             pState->pPcb = nullptr;
//         }
//         return err; 
//     }
// 
//     static err_t onConnected(void* pUser, struct altcp_pcb* pPcb, err_t err) {
//         (void)pPcb;
//         (void)err;
// 
//         TlsClient* pState = (TlsClient*)pUser;
//         if (err != ERR_OK) 
//         {
//             TS_ERRORF("TLS Client: connect failed %d\n", err);
//             return onClose(pState);
//         }
// 
//         TS_DEBUG("TLS Client: connected to server, sending request\n");
//         err = altcp_write(pState->pPcb, pState->pHttpRequest, strlen(pState->pHttpRequest), TCP_WRITE_FLAG_COPY);
//         if (err != ERR_OK) {
//             TS_ERRORF("TLS Client: error writing data, err=%d", err);
//             return onClose(pState);
//         }
// 
//         return ERR_OK;
//     }
// 
// 
//     static err_t onPoll(void* pUser, struct altcp_pcb *pPcb) {
//         (void)pPcb;
// 
//         TlsClient* pState = (TlsClient*)pUser;
//         printf("TLS Client: timed out\n");
//         pState->error = PICO_ERROR_TIMEOUT;
//         return onClose(pUser);
//     }
// 
// 
//     static void onError(void* pUser, err_t err)
//     {
//         TlsClient* pState = (TlsClient*)pUser;
//         TS_ERRORF("TLS Client: error %d\n", err);
// 
//         onClose(pState);
//         
//         pState->error = PICO_ERROR_GENERIC;
//     }
// 
//     static err_t onReceive(void* pUser, struct altcp_pcb* pPcb, struct pbuf* pPacket, err_t err)
//     {
//         (void)err;
// 
//         TlsClient *pState = (TlsClient*)pUser;
// 
//         if (!pPacket) {
//             TS_DEBUG("TLS Client: connection closed\n");
//             return onClose(pState);
//         }
// 
//         /* Copy the response to our buffer. */
//         if (pPacket->tot_len <= 0) {
//             TS_WARNING("TLS Client: Packet size less than or equal to zero!");
//             pbuf_free(pPacket);
//             return ERR_OK;
//         }
// 
// 
//         /* If a response already exists we must add a new part. */
//         if (pState->pPacketsTail)
//         {
//             pbuf_chain(pState->pPacketsTail, pPacket);
//         }
//         else
//         {
//             pState->pPacketsHead = pPacket;
//             pState->pPacketsTail = pPacket;
//         }
//                 
//         if (pState->pPacketsHead->tot_len > Network::maxHttpResponseSize) {
//             /* We only allow responses to be a certain size because of memory 
//                 constraints on a Pico W. If there was more memory that would be
//                 better and the requirement could be lifted in time. */
//             TS_ERROR("TLS Client: Response too large to be read!");
//             pbuf_free(pState->pPacketsHead);
//             return ERR_OK;
//         }
// 
//         altcp_recved(pPcb, pPacket->tot_len);
// 
//         return ERR_OK; 
//     }
// 
//     static void connectToServerIp(const ip_addr_t* pIpAddr, TlsClient* pState)
//     {
//         err_t err;
//         u16_t port = 443;
// 
//         TS_DEBUGF("TLS Client: Connecting to server IP %s port %d\n", ipaddr_ntoa(pIpAddr), port);
//         err = altcp_connect(pState->pPcb, pIpAddr, port, onConnected);
// 
//         if (err != ERR_OK)
//         {
//             TS_ERRORF("TLS Client: Error initiating connect, err=%d\n", err);
//             onClose(pState);
//         }
//     }
// 
//     static void onDnsFound(const char* pHostname, const ip_addr_t* pIpAddr, void* pUser)
//     {
//         if (pIpAddr)
//         {
//             TS_DEBUG("TLS Client: DNS resolving complete\n");
//             connectToServerIp(pIpAddr, (TlsClient*)pUser);
//         }
//         else
//         {
//             TS_DEBUGF("TLS Client: error resolving hostname %s\n", pHostname);
//             onClose(pUser);
//         }
//     }
// 
//     static bool onOpen(const char* pHostname, void* pUser)
//     {
//         err_t err;
//         ip_addr_t server_ip;
//         TlsClient* pState = (TlsClient*)pUser;
//         
// 
//         pState->pPcb = altcp_tls_new(pTlsConfig, IPADDR_TYPE_ANY);
//         if (!pState->pPcb) {
//             TS_ERROR("TLS Client: failed to create pcb\n");
//             return false;
//         }
// 
// 
//         altcp_arg(pState->pPcb, pState);
//         altcp_poll(pState->pPcb, onPoll, pState->timeout * 2);
//         altcp_recv(pState->pPcb, onReceive);
//         altcp_err(pState->pPcb, onError);
// 
//         /* Set SNI */
//         mbedtls_ssl_set_hostname((mbedtls_ssl_context *)altcp_tls_context(pState->pPcb), pHostname);
// 
// 
//         TS_DEBUGF("TLS Client: resolving %s\n", pHostname);
// 
//         // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
//         // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
//         // these calls are a no-op and can be omitted, but it is a good practice to use them in
//         // case you switch the cyw43_arch type later.
//         cyw43_arch_lwip_begin();
// 
//         err = dns_gethostbyname(pHostname, &server_ip, onDnsFound, pState);
//         if (err == ERR_OK)
//         {
//             /* host is in DNS cache */
//             connectToServerIp(&server_ip, pState);
//         }
//         else if (err != ERR_INPROGRESS)
//         {
//             TS_ERRORF("TLS Client: error initiating DNS resolving, err=%d\n", err);
//             onClose(pState->pPcb);
//         }
// 
//         cyw43_arch_lwip_end();
//         
//         return err == ERR_OK || err == ERR_INPROGRESS;
//     }
// 
//     static TlsClient* init() 
//     {
//         TlsClient* pState = (TlsClient*)calloc(1, sizeof(TlsClient));
//         if (!pState) {
//             TS_ERROR("TLS Client: failed to allocate state\n");
//             return nullptr;
//         }
// 
//         return pState;
//     }
// 
// };
// 
// uint8_t* Network::blockingHttpRequest(const char* pServer, const char* pRequest, uint16_t& contentLength, uint16_t& headerOffset, uint32_t msTimeout, const uint8_t* pCert, uint32_t certLength)
// {
// 
//     /* No CA certificate checking */
//     pTlsConfig = altcp_tls_create_config_client(pCert, certLength);
// 
//     if (!pTlsConfig) {
//         TS_ERROR("TLS Client: Could not create altcp client config");
//         return nullptr;
//     } 
//     
//     //mbedtls_ssl_conf_authmode(&tls_config->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
// 
//     TlsClient* pState = TlsClient::init();
//     if (!pState) return nullptr;
// 
//     pState->pHttpRequest = pRequest;
//     pState->timeout = msTimeout;
// 
//     if (!TlsClient::onOpen(pServer, pState)) return nullptr;
// 
//     /* Continue to read and poll until request is complete. */
//     while(!pState->complete) {
//         cyw43_arch_poll();
//         cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
//     }
// 
//     uint16_t responseLength = pState->pPacketsHead->tot_len;
//     uint8_t* pResponse = (uint8_t*)malloc(responseLength);
//     RequestHeader* pHeader = h3_request_header_new();
//     if (!pResponse || !pHeader) 
//     { 
//         TS_ERROR("Could not allocate response!");
//         goto cleanup;
//     }
// 
//     /* Copy the response from the packets. */
//     pbuf_copy_partial(pState->pPacketsHead, pResponse, responseLength, 0);
// 
//     char* pEndHeader;
//     if (h3_request_header_parse(pHeader, (const char*)pResponse, pState->pPacketsHead->tot_len, &pEndHeader) != 0)
//     {
//         TS_ERROR("Could not parse HTTP headers!");
//         h3_request_header_free(pHeader);
//         goto cleanup;
//     }
//     
//     contentLength = 0;
//     for (uint32_t i = 0; i < pHeader->HeaderSize; i++)
//     {
//         /* Find the length of the content in the header. */
//         if (strncmp("content-length", pHeader->Fields[i].FieldName, pHeader->Fields[i].FieldNameLen) == 0 ||
//             strncmp("Content-Length", pHeader->Fields[i].FieldName, pHeader->Fields[i].FieldNameLen) == 0)
//         {
//             contentLength = strtol(pHeader->Fields[i].Value, nullptr, 10);
//             break; /* The only header needed, for now. */
//         }
//     }
// 
//     if (contentLength == 0) 
//     {
//         TS_WARNING("Header contained no content-length!");
//         goto cleanup;
//     }
// 
//     headerOffset = pEndHeader - (char*)pResponse;
// 
//     h3_request_header_free(pHeader);
//     pbuf_free(pState->pPacketsHead);
//     free(pState);
//     
//     altcp_tls_free_config(pTlsConfig);
// 
//     return (uint8_t*)pResponse;
// 
// cleanup:
// 
//     h3_request_header_free(pHeader);
//     
//     if (pState->pPacketsHead)
//         pbuf_free(pState->pPacketsHead);
// 
//     free(pState);
//     altcp_tls_free_config(pTlsConfig);
// 
//     return nullptr;
// }

} /* namespace ts */