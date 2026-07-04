#include "EspNowManager.h"

static const char* TAG = "EspNowManager";
static constexpr int MIN_WIFI_CHANNEL = 1;
static constexpr int MAX_WIFI_CHANNEL = 13;

const uint8_t BROADCAST_ADDRESS[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF,
                                                     0xFF, 0xFF, 0xFF};

EspNowManager::EspNowManager(void) : _channel(0) {
    uint8_t addr[ESP_NOW_ETH_ALEN] = {0};
    esp_read_mac(addr, ESP_MAC_WIFI_STA);
    memcpy(this->_address, addr, ESP_NOW_ETH_ALEN);
}

const uint8_t* EspNowManager::getAddress(void) const {
    return this->_address;
}

bool EspNowManager::begin(uint8_t ch) {
    ESP_LOGD(TAG, "STA MAC: %s", macToStr(this->_address).str);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, false);
    delay(100);

    if (ch != 0 && !setWifiChannel(ch)) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi channel: %u", ch);
        return false;
    }
    this->_channel = ch;
    ESP_LOGD(TAG, "Wi-Fi channel set to: %u", this->_channel);
    return initEspNow();
}

bool EspNowManager::initEspNow(void) {
    const esp_err_t e = esp_now_init();
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW init failed: %s", esp_err_to_name(e));
        return false;
    }
    ESP_LOGD(TAG, "ESP-NOW initialized");
    return true;
}

void EspNowManager::initPeer(const uint8_t* addr, esp_now_peer_info_t& peer) {
    memset(&peer, 0, sizeof(peer));
    memcpy(peer.peer_addr, addr, ESP_NOW_ETH_ALEN);
    peer.channel = this->_channel;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
}

bool EspNowManager::registerPeer(const uint8_t* addr) {
    esp_now_peer_info_t peerInfo;
    initPeer(addr, peerInfo);
    const esp_err_t e = esp_now_add_peer(&peerInfo);
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register peer: %s", macToStr(addr).str);
        return false;
    }
    ESP_LOGI(TAG, "Registered peer: %s", macToStr(addr).str);
    return e == ESP_OK;
}

bool EspNowManager::unregisterPeer(const uint8_t* addr) {
    const esp_err_t e = esp_now_del_peer(addr);
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unregister peer: %s", macToStr(addr).str);
        return false;
    }
    ESP_LOGD(TAG, "Unregistered peer: %s", macToStr(addr).str);
    return e == ESP_OK;
}

bool EspNowManager::registerBroadcastPeer(void) {
    if (!registerPeer(BROADCAST_ADDRESS)) {
        ESP_LOGE(TAG, "Failed to register broadcast peer");
        return false;
    }
    ESP_LOGD(TAG, "Registered broadcast peer");
    return true;
}

bool EspNowManager::registerCallback(esp_now_recv_cb_t cb) {
    const esp_err_t e = esp_now_register_recv_cb(cb);
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register receive callback: %s",
                 esp_err_to_name(e));
        return false;
    }
    ESP_LOGD(TAG, "Registered receive callback");
    return e == ESP_OK;
}

bool EspNowManager::registerCallback(esp_now_send_cb_t cb) {
    const esp_err_t e = esp_now_register_send_cb(cb);
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register send callback: %s",
                 esp_err_to_name(e));
        return false;
    }
    ESP_LOGD(TAG, "Registered send callback");
    return e == ESP_OK;
}

bool EspNowManager::send(const uint8_t* peerAddr, const uint8_t* data,
                         size_t len) {
    const esp_err_t e = esp_now_send(peerAddr, data, len);
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send data: %s", esp_err_to_name(e));
        return false;
    }
    ESP_LOGD(TAG, "Data queued to: %s", macToStr(peerAddr).str);
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, len, ESP_LOG_DEBUG);
    return e == ESP_OK;
}

bool EspNowManager::broadcast(const uint8_t* data, size_t len) {
    if (!esp_now_is_peer_exist(BROADCAST_ADDRESS)) {
        if (!registerBroadcastPeer()) {
            return false;
        }
    }

    const esp_err_t e = esp_now_send(BROADCAST_ADDRESS, data, len);
    if (e != ESP_OK) {
        ESP_LOGE(TAG, "Failed to broadcast data: %s", esp_err_to_name(e));
        return false;
    }
    ESP_LOGD(TAG, "Broadcast data queued to: %s",
             macToStr(BROADCAST_ADDRESS).str);
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, len, ESP_LOG_DEBUG);
    return e == ESP_OK;
}

bool EspNowManager::setWifiChannel(uint8_t ch) {
    if (ch < MIN_WIFI_CHANNEL || ch > MAX_WIFI_CHANNEL) {
        ESP_LOGE(TAG, "Invalid channel: %u", ch);
        return false;
    }

    esp_err_t err = esp_wifi_set_promiscuous(true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable promiscuous mode: %s",
                 esp_err_to_name(err));
        return false;
    }

    const esp_err_t channelErr =
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    if (channelErr != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set channel: %u, error: %s", ch,
                 esp_err_to_name(channelErr));
    }

    const esp_err_t disableErr = esp_wifi_set_promiscuous(false);
    if (disableErr != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disable promiscuous mode: %s",
                 esp_err_to_name(disableErr));
    }

    return channelErr == ESP_OK && disableErr == ESP_OK;
}
