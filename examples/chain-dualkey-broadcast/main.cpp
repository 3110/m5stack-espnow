#include <M5Unified.h>

#include "EspNowManager.h"

static constexpr uint8_t KEY1_PIN = 0;
static constexpr uint8_t KEY2_PIN = 17;

static const char KEY1_CODE[] = "a";
static const char KEY2_CODE[] = "b";

EspNowManager espNowManager;

m5::Button_Class BtnA;
m5::Button_Class BtnB;

void setup(void) {
    esp_log_level_set("*", static_cast<esp_log_level_t>(LOG_LOCAL_LEVEL));
    M5.begin();
    pinMode(KEY1_PIN, INPUT);
    pinMode(KEY2_PIN, INPUT);

    if (!espNowManager.begin()) {
        ESP_LOGE("setup", "Failed to initialize ESP-NOW");
        return;
    }
}

void loop(void) {
    M5.update();
    uint32_t ms = millis();
    BtnA.setRawState(ms, !digitalRead(KEY1_PIN));
    BtnB.setRawState(ms, !digitalRead(KEY2_PIN));

    if (BtnA.wasPressed()) {
        espNowManager.broadcast((uint8_t*)KEY1_CODE, strlen(KEY1_CODE));
    }

    if (BtnB.wasPressed()) {
        espNowManager.broadcast((uint8_t*)KEY2_CODE, strlen(KEY2_CODE));
    }

    delay(10);
}