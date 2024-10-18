#include "tft.h"

TftDisplay::TftDisplay(uint8_t tft_bus, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t led_pin) :
    _spi(tft_bus), _tft(&_spi, cs_pin, dc_pin, rst_pin), _led_pin(led_pin) {}
void TftDisplay::begin() {
    _tft.initR(INITR_BLACKTAB);

    _tft.setSPISpeed(40000000);

    _tft.setRotation(1);
    _tft.fillScreen(ST77XX_BLACK);

    pinMode(_led_pin, OUTPUT);

    _tft.println("Loading...");
}

void TftDisplay::_draw_header() {
    _tft.setTextWrap(false);
    _tft.fillScreen(ST77XX_BLACK);

    for (int i = 0; i < _page_count; ++i) {
        int16_t x = _header_offset + i * (_header_circle_r * 2 + _header_circle_m);

        if (i == _page) {
            _tft.fillCircle(x, _header_y, (int16_t) _header_circle_r, _header_color);
        } else {
            _tft.drawCircle(x, _header_y, (int16_t) _header_circle_r, _header_color);
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

void TftDisplay::_draw_page(const SensorData &sensor_data) {
    String value = "000";
    SensorState state = SensorState::NOT_READY;
    String label;

    switch (_page) {
        case 0:
            label = "CO2";
            state = sensor_data.state.co2;
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.co2);
            }
            break;

        case 1:
            label = "TEMP";
            state = sensor_data.state.temperature;
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.temperature, 1) + SMB_DEGREE;
            }
            break;

        case 2:
            label = "HUM";
            state = sensor_data.state.humidity;
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.humidity, 1) + SMB_PERCENT;
            }
            break;

        case 3:
            label = "PM1.0";
            state = sensor_data.state.pms;
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.pms.pm10_env);
            }
            break;

        case 4:
            label = "PM2.5";
            state = sensor_data.state.pms;
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.pms.pm25_env - sensor_data.pms.pm10_env);
            }
            break;

        case 5:
            label = "PM10";
            state = sensor_data.state.pms;
            if (state != SensorState::NOT_READY) {
                value = convert_value(sensor_data.pms.pm100_env - sensor_data.pms.pm25_env);
            }
            break;

        default:
            return;
    }

    _draw_value(value, label);
    _draw_state(state);
}

void TftDisplay::_draw_value(const String &value, const String &label) {
    _tft.setFont(&custom_font);
    _tft.setTextSize(1);
    _tft.setTextColor(ST77XX_WHITE);

    int16_t x1, y1;
    uint16_t w, h;

    _tft.getTextBounds(value, 0, 0, &x1, &y1, &w, &h);

    int16_t x = TFT_WIDTH / 2 - w / 2;
    int16_t y = TFT_HEIGHT / 2 + _header_y + _header_circle_r * 2 - h / 2;

    _tft.setCursor(x, y);
    _tft.println(value);

    _tft.setFont(&LineramaBold_10pt);
    _tft.setTextSize(1);

    y += h;

    _tft.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

    _tft.setTextColor(_header_color);
    _tft.setCursor(TFT_WIDTH / 2 - w / 2, y + h / 2);
    _tft.print(label);

    _tft.setFont();
}

void TftDisplay::_draw_state(SensorState state) {
    uint16_t color;
    switch (state) {
        case SensorState::CRITICAL:
            color = 0xd0a3;
            break;

        case SensorState::WARNING:
            color = 0xec84;
            break;

        case SensorState::GOOD:
            color = 0x2e25;
            break;

        default:
            return;
    }

    _tft.fillRoundRect(_state_x, _state_y, _state_width, _state_height, _state_roundness, color);
}

void TftDisplay::update(const SensorData &data) {
    _draw_header();
    _draw_page(data);

    if (++_page >= _page_count) _page = 0;
}
