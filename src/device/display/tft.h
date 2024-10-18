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


constexpr uint16_t TFT_WIDTH = 160;
constexpr uint16_t TFT_HEIGHT = 128;

class TftDisplay {
    SPIClass _spi;
    Adafruit_ST7735 _tft;

    uint8_t _led_pin;

    const uint16_t _page_count = 6;

    const uint16_t _header_circle_r = 3;
    const uint16_t _header_circle_m = 2;
    const int16_t _header_y = 8;

    const uint16_t _header_width = _page_count * _header_circle_r * 2 + _header_circle_m * (_page_count - 1);
    const uint16_t _header_offset = TFT_WIDTH / 2 - _header_width / 2;

    const uint32_t _header_color = 0xd6ba;

    const uint16_t _state_width = 32;
    const uint16_t _state_height = 8;
    const uint16_t _state_roundness = 2;
    const int16_t _state_x = TFT_WIDTH / 2 - _state_width / 2;
    const int16_t _state_y = TFT_HEIGHT - _state_height - 8;

    uint16_t _page = 0;

    void _draw_header();
    void _draw_page(const SensorData &sensor_data);

    void _draw_value(const String &value, const String &label);
    void _draw_state(SensorState state);

public:
    TftDisplay(uint8_t tft_bus, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t led_pin);

    void begin();

    void update(const SensorData &data);

    inline void set_contrast(uint16_t value) { analogWrite(_led_pin, std::min(value, PWM_MAX_VALUE)); }
};
