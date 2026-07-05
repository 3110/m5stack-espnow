#pragma once

#include <WiFi.h>
#include <esp_log.h>
#include <esp_now.h>
#include <esp_wifi.h>

struct MacAddressStr
{
    char str[ESP_NOW_ETH_ALEN * 3];
};

inline MacAddressStr macToStr(const uint8_t* mac) {
    MacAddressStr result;
    snprintf(result.str, sizeof(result.str), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return result;
}

class EspNowManager {
public:
    static constexpr const char* VERSION = "v0.0.3";

    EspNowManager(void);
    ~EspNowManager(void) = default;

    const uint8_t* getAddress(void) const;
    bool begin(uint8_t ch = ESP_NOW_CHANNEL);
    bool registerPeer(const uint8_t* addr);
    bool unregisterPeer(const uint8_t* addr);
    bool registerCallback(esp_now_recv_cb_t cb);
    bool registerCallback(esp_now_send_cb_t cb);
    bool send(const uint8_t* peerAddr, const uint8_t* data, size_t len);
    bool broadcast(const uint8_t* data, size_t len);

protected:
    bool initEspNow(void);
    void initPeer(const uint8_t* addr, esp_now_peer_info_t& peer);
    bool registerBroadcastPeer(void);
    bool setWifiChannel(uint8_t ch);

private:
    uint8_t _channel;
    uint8_t _address[ESP_NOW_ETH_ALEN];
};
