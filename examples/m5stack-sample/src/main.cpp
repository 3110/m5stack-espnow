#include "EspNowManager.h"

EspNowManager espNowManager;
const uint8_t MESSAGE[] = "Hello, world!";
const uint32_t MESSAGE_INTERVAL_MS = 5000;

void OnDataSent(const uint8_t* addr, esp_now_send_status_t status) {
    ESP_LOGD(
        "OnDataSent", "From: %s To: %s, Status: %s",
        macToStr(espNowManager.getAddress()).str, macToStr(addr).str,
        status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataReceived(const uint8_t* addr, const uint8_t* data, int len) {
    ESP_LOGD("OnDataReceived", "From: %s", macToStr(addr).str);
    ESP_LOG_BUFFER_HEXDUMP("OnDataReceived", data, len, ESP_LOG_DEBUG);
}

void setup(void) {
    esp_log_level_set("*", static_cast<esp_log_level_t>(LOG_LOCAL_LEVEL));

    if (!espNowManager.begin()) {
        ESP_LOGE("setup", "Failed to initialize ESP-NOW");
        return;
    }
    if (!espNowManager.registerCallback(OnDataSent)) {
        ESP_LOGE("setup", "Failed to register send callback");
        return;
    }
    if (!espNowManager.registerCallback(OnDataReceived)) {
        ESP_LOGE("setup", "Failed to register receive callback");
        return;
    }
}

void loop(void) {
    espNowManager.broadcast(MESSAGE,
                            strlen(reinterpret_cast<const char*>(MESSAGE)));
    delay(MESSAGE_INTERVAL_MS);
}
