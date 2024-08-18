#pragma once

#include <GyverOLED.h>

#include "data.h"

template<int Type, int Buffer = OLED_BUFFER>
class OledDisplay {
    GyverOLED<Type, Buffer> _oled;

public:
    void begin();

    void update(const SensorData &data);

    inline void set_contrast(uint8_t value) { _oled.setContrast(value); };
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

    _oled.print(String(data.temperature, 1));
    _oled.print(" C ");

    _oled.print(String(data.humidity, 1));
    _oled.print("%");

    _oled.setCursor(0, 3);
    _oled.setScale(2);

    _oled.print(data.co2);
    _oled.print(" ppm");

    _oled.setCursor(0, 6);
    _oled.setScale(1);

    _oled.print("PM: ");
    _oled.print(data.pm10);
    _oled.print(" - ");
    _oled.print(data.pm25 - data.pm10);
    _oled.print(" - ");
    _oled.print(data.pm100 - data.pm25);

    _oled.update();
}