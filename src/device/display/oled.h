#pragma once

#include <GyverOLED.h>

#include "app/data.h"
#include "sys_constants.h"

template<int Type, int Buffer = OLED_BUFFER>
class OledDisplay {
    GyverOLED<Type, Buffer> _oled;

public:
    void begin();

    void update(const SensorData &data);

    inline void set_contrast(uint16_t value) { _oled.setContrast((int) std::min(value, PWM_MAX_VALUE) * 255 / PWM_MAX_VALUE); };
};

template<int Type, int Buffer>
void OledDisplay<Type, Buffer>::begin() {
    _oled.init();

    _oled.print("Loading...");
    _oled.update();
}

template<int Type, int Buffer>
void OledDisplay<Type, Buffer>::update(const SensorData &data) {
    _oled.clear();

    _oled.setCursor(0, 1);
    _oled.setScale(1);

    if (data.state.temperature != SensorState::NOT_READY) {
        _oled.print(String(data.temperature, 1));
        _oled.print(" C ");
    } else {
        _oled.print("N/A");
    }

    if (data.state.humidity != SensorState::NOT_READY) {
        _oled.print(String(data.humidity, 1));
        _oled.print("%");
    } else {
        _oled.print("N/A");
    }

    _oled.setCursor(0, 3);
    _oled.setScale(2);

    if (data.state.co2 != SensorState::NOT_READY) {
        _oled.print(data.co2);
        _oled.print(" ppm");
    } else {
        _oled.print("N/A");
    }

    _oled.setCursor(0, 6);
    _oled.setScale(1);

    _oled.print("PM: ");
    if (data.state.pms != SensorState::NOT_READY) {
        _oled.print(data.pms.pm10_env);
        _oled.print(" - ");
        _oled.print(data.pms.pm25_env - data.pms.pm10_env);
        _oled.print(" - ");
        _oled.print(data.pms.pm100_env - data.pms.pm25_env);
    } else {
        _oled.print("N/A");
    }

    _oled.update();
}
