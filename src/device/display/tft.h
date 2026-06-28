#pragma once

#include "Arduino.h"

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735

#include "../fonts/DataTransfer30pt.h"
#include "../fonts/LineramaBold_10pt.h"

#include "constants.h"
#include "app/data.h"

#define SMB_SPACE ((char) 0x3a)
#define SMB_POINT ((char) 0x3b)
#define SMB_PERCENT ((char) 0x3c)


constexpr uint16_t TFT_WIDTH = 160;
constexpr uint16_t TFT_HEIGHT = 128;

class TftDisplay {
    SPIClass _spi;
    Adafruit_ST7735 _tft;

    uint8_t _led_pin;

    static constexpr uint16_t _page_count = 7;

    static constexpr uint16_t _history_size = 120;
    static constexpr uint32_t _history_interval_ms = 60000;
    static constexpr uint32_t _history_save_interval_ms = 3600000;
    static constexpr uint32_t _history_storage_header = 0x54465448;
    static constexpr uint8_t _history_storage_version = 1;

    struct HistoryStorage {
        uint32_t header;
        uint8_t version;
        uint16_t head;
        uint16_t count;
        float values[_page_count][_history_size];
    };

    const uint16_t _background_color = 0x000b;
    const uint16_t _muted_color = 0x6b7d;
    const uint16_t _accent_color = 0xffff;
    const uint16_t _grid_color = 0x31ad;

    const uint16_t _header_circle_r = 2;
    const uint16_t _header_circle_m = 4;
    const int16_t _header_x = 8;
    const int16_t _header_y = 9;

    const int16_t _graph_x = 8;
    const int16_t _graph_y = 91;
    const uint16_t _graph_w = 144;
    const uint16_t _graph_h = 25;

    uint16_t _page = 0;
    float _history[_page_count][_history_size] = {};
    uint16_t _history_head = 0;
    uint16_t _history_count = 0;
    uint32_t _last_history_sample_ms = 0;
    uint32_t _last_history_save_ms = 0;
    bool _history_dirty = false;

    void _draw_header();
    void _draw_page(const SensorData &sensor_data);

    void _draw_trend(const SensorData &sensor_data, SensorState state);
    void _draw_value(const String &value, const String &label, SensorState state);
    void _draw_chart();
    void _append_history(const SensorData &sensor_data);
    void _load_history();
    void _save_history();
    void _draw_right_aligned(const String &text, int16_t right, int16_t y);
    String _format_trend_delta(float delta) const;
    float _value_for_page(const SensorData &sensor_data, uint16_t page) const;
    SensorState _state_for_page(const SensorData &sensor_data, uint16_t page) const;
    uint16_t _status_color(SensorState state) const;
    bool _is_nan(float value) const;

public:
    TftDisplay(uint8_t tft_bus, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t led_pin);

    void begin();

    void update(const SensorData &data);

    inline void set_contrast(uint16_t value) { analogWrite(_led_pin, std::min(value, PWM_MAX_VALUE)); }
};
