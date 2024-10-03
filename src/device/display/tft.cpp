#include "tft.h"

TftDisplay::TftDisplay(uint8_t tft_bus, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t led_pin) :
    _spi(tft_bus), _tft(&_spi, cs_pin, dc_pin, rst_pin), _led_pin(led_pin) {}
void TftDisplay::begin() {
    _tft.initR(INITR_BLACKTAB);

    _tft.setSPISpeed(40000000);

    _tft.setRotation(1);
    _tft.fillScreen(ST77XX_BLACK);

    pinMode(_led_pin, OUTPUT);
    set_contrast(TFT_CONTRAST_DEFAULT);

    _tft.println("Loading...");
}

void TftDisplay::_draw_header() {
    _tft.setTextWrap(false);
    _tft.fillScreen(ST77XX_BLACK);

    for (int i = 0; i < page_count; ++i) {
        auto x = header_offset + i * (header_circle_r * 2 + header_circle_m);

        if (i == page) {
            _tft.fillCircle(x, header_y, header_circle_r, header_color);
        } else {
            _tft.drawCircle(x, header_y, header_circle_r, header_color);
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

void TftDisplay::_draw_page(const SensorData &sensorData) {
    switch (page) {
        case 0:
            _draw_value(convert_value(sensorData.co2), "CO2");
            break;

        case 1:
            _draw_value(convert_value(sensorData.temperature, 1) + SMB_DEGREE, "TEMP");
            break;

        case 2:
            _draw_value(convert_value(sensorData.humidity, 1) + SMB_PERCENT, "HUM");
            break;

        case 3:
            _draw_value(convert_value(sensorData.pms.pm10_env), "PM1.0");
            break;

        case 4:
            _draw_value(convert_value(sensorData.pms.pm25_env - sensorData.pms.pm10_env), "PM2.5");
            break;

        case 5:
            _draw_value(convert_value(sensorData.pms.pm100_env - sensorData.pms.pm25_env), "PM10");
            break;
    }
}

void TftDisplay::_draw_value(String value, String label) {
    _tft.setFont(&custom_font);
    _tft.setTextSize(1);
    _tft.setTextColor(ST77XX_WHITE);

    int16_t x1, y1;
    uint16_t w, h;

    _tft.getTextBounds(value, 0, 0, &x1, &y1, &w, &h);

    int16_t x = 160 / 2 - w / 2,
        y = 128 / 2 + header_y + header_circle_r * 2 - h / 2;

    _tft.setCursor(x, y);
    _tft.println(value);

    _tft.setFont(&LineramaBold_10pt);
    _tft.setTextSize(1);

    y += h;

    _tft.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

    _tft.setTextColor(header_color);
    _tft.setCursor(160 / 2 - w / 2, y + h / 2);
    _tft.print(label);

    _tft.setFont();
}

void TftDisplay::update(const SensorData &data) {
    _draw_header();
    _draw_page(data);

    if (++page >= page_count) page = 0;
}
