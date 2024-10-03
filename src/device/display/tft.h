#pragma once

#include "Arduino.h"

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735

#include "../fonts/custom_font.h"
#include "../fonts/LineramaBold_10pt.h"

#include "constants.h"
#include "app/data.h"

#define SMB_DEGREE '<'
#define SMB_PERCENT ';'
#define SMB_POINT ':'

class TftDisplay {
    SPIClass _spi;
    Adafruit_ST7735 _tft;

    uint8_t _led_pin;

    const uint16_t page_count = 6;

    const uint16_t header_circle_r = 3;
    const uint16_t header_circle_m = 2;
    const uint16_t header_y = 8;

    const uint16_t header_width = page_count * header_circle_r * 2 + header_circle_m * (page_count - 1);
    const uint16_t header_offset = 160 / 2 - header_width / 2;

    const uint32_t header_color = 0xd6ba;

    uint16_t page = 0;

    void _draw_header();
    void _draw_page(const SensorData &sensorData);

    void _draw_value(String value, String label);

public:
    TftDisplay(uint8_t tft_bus, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t led_pin);

    void begin();

    void update(const SensorData &data);

    inline void set_contrast(uint16_t value) {analogWrite(_led_pin, std::min(value, PWM_MAX_VALUE));}
};
