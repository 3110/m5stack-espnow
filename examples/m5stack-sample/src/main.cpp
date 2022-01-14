#include <M5Stack.h>

#include "Debug.h"
#include "EspNowManager.h"

EspNowManager espNowManager;
const uint8_t MESSAGE[] = "Hello, world!";
const uint32_t MESSAGE_INTERVAL_MS = 5000;

void OnDataSent(const uint8_t* addr, esp_now_send_status_t status) {
    SERIAL_PRINT("OnDataSent(");
    SERIAL_MAC_ADDRESS_PRINT(addr);
    SERIAL_PRINTF_LN("): %s", status == ESP_NOW_SEND_SUCCESS
                                  ? "Delivery Success"
                                  : "Delivery Fail");
}

void OnDataReceived(const uint8_t* addr, const uint8_t* data, int len) {
    SERIAL_PRINT("OnDataReceived(");
    SERIAL_MAC_ADDRESS_PRINT(addr);
    SERIAL_PRINTLN("): [");
    SERIAL_DUMP(data, len);
    SERIAL_PRINTLN("]");
}

void setup(void) {
    M5.begin();
    espNowManager.begin();
    espNowManager.registerCallback(OnDataSent);
    espNowManager.registerCallback(OnDataReceived);
    espNowManager.registerPeer(EspNowManager::BROADCAST_ADDRESS);
}

void loop(void) {
    M5.update();
    espNowManager.send(EspNowManager::BROADCAST_ADDRESS, MESSAGE,
                       sizeof(MESSAGE));
    delay(MESSAGE_INTERVAL_MS);
}