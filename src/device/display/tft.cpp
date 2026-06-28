#include "tft.h"

#include <LittleFS.h>
#include <cstring>
#include <math.h>

namespace {
const char *TFT_HISTORY_PATH = "/__storage/tft_history";
}

TftDisplay::TftDisplay(uint8_t tft_bus, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t led_pin) :
    _spi(tft_bus), _tft(&_spi, cs_pin, dc_pin, rst_pin), _led_pin(led_pin) {}
void TftDisplay::begin() {
    _tft.initR(INITR_BLACKTAB);

    _tft.setSPISpeed(40000000);

    _tft.setRotation(1);
    _tft.fillScreen(_background_color);

    pinMode(_led_pin, OUTPUT);
    _load_history();

    _tft.println("Loading...");
}

void TftDisplay::_draw_header() {
    _tft.setTextWrap(false);
    _tft.fillScreen(_background_color);

    for (int i = 0; i < _page_count; ++i) {
        int16_t x = _header_x + i * (_header_circle_r * 2 + _header_circle_m);

        if (i == _page) {
            _tft.fillCircle(x, _header_y, (int16_t) _header_circle_r, _accent_color);
        } else {
            _tft.drawCircle(x, _header_y, (int16_t) _header_circle_r, _muted_color);
        }
    }
}

String convert_value(float value, int fraction) {
    String res = String(value, fraction);
    res.replace('.', SMB_POINT);

    return res;
}

template<typename T>
String convert_value(T value) {
    return String(value);
}

float TftDisplay::_value_for_page(const SensorData &sensor_data, uint16_t page) const {
    switch (page) {
        case 0:
            return sensor_data.co2;
        case 1:
            return sensor_data.temperature;
        case 2:
            return sensor_data.humidity;
        case 3:
            return sensor_data.tvoc;
        case 4:
            return sensor_data.pms.pm10_env;
        case 5:
            return sensor_data.pms.pm25_env;
        case 6:
            return sensor_data.pms.pm100_env;
        default:
            return NAN;
    }
}

SensorState TftDisplay::_state_for_page(const SensorData &sensor_data, uint16_t page) const {
    switch (page) {
        case 0:
            return sensor_data.state.co2;
        case 1:
            return sensor_data.state.temperature;
        case 2:
            return sensor_data.state.humidity;
        case 3:
            return sensor_data.state.tvoc;
        case 4:
        case 5:
        case 6:
            return sensor_data.state.pms;
        default:
            return SensorState::NOT_READY;
    }
}

void TftDisplay::_draw_page(const SensorData &sensor_data) {
    String value = "000";
    SensorState state = _state_for_page(sensor_data, _page);
    String label;

    switch (_page) {
        case 0:
            label = "CO2";
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.co2);
            }
            break;

        case 1:
            label = "TEMP";
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.temperature, 1);
            }
            break;

        case 2:
            label = "HUM";
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.humidity, 1) + SMB_PERCENT;
            }
            break;

        case 3:
            label = "TVOC";
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.tvoc, 0);
            }
            break;

        case 4:
            label = "PM1.0";
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.pms.pm10_env);
            }
            break;

        case 5:
            label = "PM2.5";
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.pms.pm25_env);
            }
            break;

        case 6:
            label = "PM10";
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.pms.pm100_env);
            }
            break;

        default:
            return;
    }

    _draw_trend(sensor_data, state);
    _draw_value(value, label, state);
    _draw_chart();
}

void TftDisplay::_draw_value(const String &value, const String &label, SensorState state) {
    _tft.setFont(&DataTransfer30pt);
    _tft.setTextSize(1);
    _tft.setTextColor(ST77XX_WHITE);

    int16_t x1, y1;
    uint16_t w, h;

    _tft.getTextBounds(value, 0, 0, &x1, &y1, &w, &h);

    int16_t x = TFT_WIDTH / 2 - w / 2;
    int16_t y = 58;

    _tft.setCursor(x, y);
    _tft.println(value);

    _tft.setFont(&LineramaBold_10pt);
    _tft.setTextSize(1);

    y += h;

    _tft.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

    _tft.setTextColor(_status_color(state));
    _tft.setCursor(TFT_WIDTH / 2 - w / 2, 80);
    _tft.print(label);

    _tft.setFont();
}

uint16_t TftDisplay::_status_color(SensorState state) const {
    switch (state) {
        case SensorState::CRITICAL:
            return 0xd0a3;

        case SensorState::WARNING:
            return 0xec84;

        case SensorState::GOOD:
            return 0x2e25;

        default:
            return _muted_color;
    }
}

bool TftDisplay::_is_nan(float value) const {
    return isnan(value);
}

String TftDisplay::_format_trend_delta(float delta) const {
    float abs_delta = delta < 0 ? -delta : delta;
    const char *suffix = "";

    if (abs_delta >= 1000000.0f) {
        abs_delta /= 1000000.0f;
        suffix = "kk";
    } else if (abs_delta >= 1000.0f) {
        abs_delta /= 1000.0f;
        suffix = "k";
    }

    String result = delta >= 0 ? "+" : "-";
    if (abs_delta < 10.0f) {
        String value = String(abs_delta, 2);
        while (value.endsWith("0")) value.remove(value.length() - 1);
        if (value.endsWith(".")) value.remove(value.length() - 1);
        result += value;
    } else {
        result += String(abs_delta, 0);
    }
    result += suffix;

    return result;
}

void TftDisplay::_draw_right_aligned(const String &text, int16_t right, int16_t y) {
    int16_t x1, y1;
    uint16_t w, h;
    _tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    _tft.setCursor(right - x1 - w, y);
    _tft.print(text);
}

void TftDisplay::_draw_trend(const SensorData &sensor_data, SensorState state) {
    _tft.setFont();
    _tft.setTextSize(1);
    _tft.setTextColor(_accent_color);

    String trend = "--";
    const int16_t trend_right = TFT_WIDTH - 8;
    const int16_t trend_y = 6;

    if (state == SensorState::NOT_READY || _history_count == 0) {
        _draw_right_aligned(trend, trend_right, trend_y);
        return;
    }

    float oldest_value = NAN;
    for (uint16_t i = 0; i < _history_count; ++i) {
        uint16_t index = (_history_head + _history_size - _history_count + i) % _history_size;
        float value = _history[_page][index];
        if (_is_nan(value)) continue;

        oldest_value = value;
        break;
    }

    float current_value = _value_for_page(sensor_data, _page);
    if (_is_nan(oldest_value) || _is_nan(current_value)) {
        _draw_right_aligned(trend, trend_right, trend_y);
        return;
    }

    trend = _format_trend_delta(current_value - oldest_value);
    _draw_right_aligned(trend, trend_right, trend_y);
}

void TftDisplay::_draw_chart() {
    _tft.drawFastHLine(_graph_x, _graph_y + _graph_h / 2, _graph_w, _grid_color);
    _tft.drawFastHLine(_graph_x, _graph_y + _graph_h - 1, _graph_w, _grid_color);

    _tft.setFont();
    _tft.setTextSize(1);
    _tft.setTextColor(_accent_color);
    _tft.setCursor(_graph_x, 119);
    _tft.print("-2h");
    _tft.setCursor(_graph_x + _graph_w - 18, 119);
    _tft.print("now");

    if (_history_count < 2) return;

    float min_value = 0;
    float max_value = 0;
    bool has_value = false;
    for (uint16_t i = 0; i < _history_count; ++i) {
        uint16_t index = (_history_head + _history_size - _history_count + i) % _history_size;
        float value = _history[_page][index];
        if (_is_nan(value)) continue;

        if (!has_value) {
            min_value = value;
            max_value = value;
            has_value = true;
        } else {
            if (value < min_value) min_value = value;
            if (value > max_value) max_value = value;
        }
    }

    if (!has_value) return;
    if (max_value - min_value < 0.1f) {
        max_value += 1;
        min_value -= 1;
    }

    int16_t last_x = 0;
    int16_t last_y = 0;
    bool has_last = false;
    for (uint16_t i = 0; i < _history_count; ++i) {
        uint16_t index = (_history_head + _history_size - _history_count + i) % _history_size;
        float value = _history[_page][index];
        if (_is_nan(value)) {
            has_last = false;
            continue;
        }

        int16_t x = _graph_x + ((_history_count == 1) ? 0 : (int32_t)i * (_graph_w - 1) / (_history_count - 1));
        int16_t y = _graph_y + _graph_h - 2 - (int32_t)((value - min_value) * (_graph_h - 4) / (max_value - min_value));

        if (has_last) {
            _tft.drawLine(last_x, last_y, x, y, _accent_color);
        }
        _tft.fillCircle(x, y, 1, _accent_color);

        last_x = x;
        last_y = y;
        has_last = true;
    }
}

void TftDisplay::_append_history(const SensorData &sensor_data) {
    uint32_t now = millis();
    if (_history_count > 0 && now - _last_history_sample_ms < _history_interval_ms) return;

    for (uint16_t page = 0; page < _page_count; ++page) {
        _history[page][_history_head] = _state_for_page(sensor_data, page) == SensorState::NOT_READY
            ? NAN
            : _value_for_page(sensor_data, page);
    }

    _history_head = (_history_head + 1) % _history_size;
    if (_history_count < _history_size) ++_history_count;
    _last_history_sample_ms = now;
    _history_dirty = true;

    if (now - _last_history_save_ms >= _history_save_interval_ms) {
        _save_history();
    }
}

void TftDisplay::_load_history() {
    File file = LittleFS.open(TFT_HISTORY_PATH, "r");
    if (!file) return;

    if (file.size() != sizeof(HistoryStorage)) {
        file.close();
        return;
    }

    HistoryStorage storage{};
    size_t read_size = file.read((uint8_t *) &storage, sizeof(storage));
    file.close();

    if (read_size != sizeof(storage)) return;
    if (storage.header != _history_storage_header || storage.version != _history_storage_version) return;
    if (storage.head >= _history_size || storage.count > _history_size) return;

    _history_head = storage.head;
    _history_count = storage.count;
    memcpy(_history, storage.values, sizeof(_history));
    _history_dirty = false;
    _last_history_save_ms = millis();
}

void TftDisplay::_save_history() {
    if (!_history_dirty) return;

    LittleFS.mkdir(STORAGE_PATH);

    File file = LittleFS.open(TFT_HISTORY_PATH, "w");
    if (!file) return;

    HistoryStorage storage{};
    storage.header = _history_storage_header;
    storage.version = _history_storage_version;
    storage.head = _history_head;
    storage.count = _history_count;
    memcpy(storage.values, _history, sizeof(_history));

    size_t write_size = file.write((uint8_t *) &storage, sizeof(storage));
    file.close();

    if (write_size == sizeof(storage)) {
        _history_dirty = false;
        _last_history_save_ms = millis();
    }
}

void TftDisplay::update(const SensorData &data) {
    _append_history(data);
    _draw_header();
    _draw_page(data);

    if (++_page >= _page_count) _page = 0;
}
