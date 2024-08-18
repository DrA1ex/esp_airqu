#define OLED
#define TFT

#include <HardwareSerial.h>
#include <WiFi.h>

#include <GyverBME280.h>
#include <MHZ19.h>

#ifdef OLED

#include <GyverOLED.h>

#endif

#ifdef TFT

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735

#include "../fonts/custom_font.h"
#include "../fonts/LineramaBold_10pt.h"

#endif

#include <SPI.h>

#define UART_PMS 1
#define UART_CO2 2

const unsigned int BME_ADDRESS = 0x76;

GyverBME280 bme;

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

HardwareSerial co2Uart(UART_CO2);
MHZ19 Mhz19;
HardwareSerial pmsSerial(UART_PMS);

#define SMB_DEGREE '<'
#define SMB_PERCENT ';'
#define SMB_POINT ':'

#define TFT_CS 15
#define TFT_RST 25
#define TFT_DC 26

SPIClass spi1(HSPI);

Adafruit_ST7735 tft = Adafruit_ST7735(&spi1, TFT_CS, TFT_DC, TFT_RST);

int page = 0;

auto page_count = 5;

auto header_circle_r = 3;
auto header_circle_m = 2;
auto header_y = 8;

auto header_width = page_count * header_circle_r * 2 + header_circle_m * (page_count - 1);
auto header_offset = 160 / 2 - header_width / 2;
auto header_color = 0xd6ba;

void draw_value(String value, String label);

String convert_value(float value, int fraction) {
    String res = String(value, fraction);
    res.replace('.', SMB_POINT);

    return res;
}

template<typename T>
String convert_value(T value) {
    return String(value);
}

void IRAM_ATTR BtnInterrupt1() {
    Serial.println("Btn1 pressed");
}

void IRAM_ATTR BtnInterrupt2() {
    Serial.println("Btn2 pressed");
}

void setup() {
    analogWrite(27, 512);
    Serial.begin(9600);

    Serial.println("Begin");
    attachInterrupt(digitalPinToInterrupt(18), BtnInterrupt1, RISING);
    attachInterrupt(digitalPinToInterrupt(5), BtnInterrupt2, RISING);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(1);

    oled.init();
    oled.setContrast(32);

    oled.print("Loading...");
    oled.update();

    tft.initR(INITR_BLACKTAB);

    tft.setSPISpeed(40000000);

    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    tft.println("Loading...");

    pinMode(19, OUTPUT);
    analogWrite(19, 256);

    Wire.begin();
    bme.begin(BME_ADDRESS);

    co2Uart.begin(9600, SERIAL_8N1, 17, 16);

    Mhz19.begin(co2Uart);
    Mhz19.setRange(5000);
    Mhz19.autoCalibration(false);

    pmsSerial.begin(9600, SERIAL_8N1, 33, 32);

    Serial.println("All done");
}

struct pms5003data {
    uint16_t framelen;
    uint16_t pm10_standard, pm25_standard, pm100_standard;
    uint16_t pm10_env, pm25_env, pm100_env;
    uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
};

struct pms5003data data{};

struct pms5003data _data{};

float temperature = 0, humidity = 0;

bool readPMSdata(Stream *s);

void loop() {
    Serial.println("Loop begin");

    humidity = bme.readHumidity();
    temperature = bme.readTemperature();

    Serial.print("Hum: ");
    Serial.print(humidity);
    Serial.print(" Temp: ");
    Serial.println(temperature);

    co2Uart.begin(9600, SERIAL_8N1, 17, 16);
    float co2 = Mhz19.getCO2(false);
    co2Uart.end();

    Serial.println("Reading PM...");
    if (readPMSdata(&pmsSerial)) {
        _data = data;
    } else {
        while (pmsSerial.available()) pmsSerial.read();
    }

    oled.clear();

    oled.setCursor(0, 1);
    oled.setScale(1);

    oled.print(String(temperature, 1));
    oled.print(" C ");

    oled.print(String(humidity, 1));
    oled.print("%");

    oled.setCursor(0, 3);
    oled.setScale(2);

    oled.print(String(co2, 0));
    oled.print(" ppm");

    oled.setCursor(0, 6);
    oled.setScale(1);

    oled.print("PM: ");
    oled.print(_data.pm10_env);
    oled.print(" - ");
    oled.print(_data.pm25_env - _data.pm10_env);
    oled.print(" - ");
    oled.print(_data.pm100_env - _data.pm25_env);

    Serial.println("OLED updating...");

    oled.update();

    Serial.println("OLED done");

    tft.setTextWrap(false);

    tft.fillScreen(ST77XX_BLACK);

    for (int i = 0; i < page_count; ++i) {
        auto x = header_offset + i * (header_circle_r * 2 + header_circle_m);

        if (i == page) {
            tft.fillCircle(x, header_y, header_circle_r, header_color);
        } else {
            tft.drawCircle(x, header_y, header_circle_r, header_color);
        }
    }

    switch (page) {
        case 0:
            draw_value(convert_value(co2, 0), "CO2");
            break;

        case 1:
            draw_value(convert_value(temperature, 1) + SMB_DEGREE, "TEMP");
            break;

        case 2:
            draw_value(convert_value(humidity, 1) + SMB_PERCENT, "HUM");
            break;

        case 3:
            draw_value(convert_value(_data.pm25_env), "PM2.5");
            break;

        case 4:
            draw_value(convert_value(_data.pm25_env - _data.pm10_env), "PM10");
            break;
    }

    if (++page >= page_count) page = 0;

    Serial.println("TFT done");

    delay(3000);
}

bool readPMSdata(Stream *s) {
    if (!s->available()) {
        Serial.println("PM: no data");
        return false;
    }

    // Read a byte at a time until we get to the special '0x42' start-byte
    if (s->peek() != 0x42) {
        s->read();
        Serial.println("PM: bad header");
        return false;
    }

    // Now read all 32 bytes
    if (s->available() < 32) {
        Serial.println("PM: not enought data");
        return false;
    }

    uint8_t buffer[32];
    uint16_t sum = 0;
    s->readBytes(buffer, 32);

    // get checksum ready
    for (uint8_t i = 0; i < 30; i++) {
        sum += buffer[i];
    }

    /* debugging
    for (uint8_t i=2; i<32; i++) {
      Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
    }
    Serial.println();
    */

    // The data comes in endian'd, this solves it so it works on all platforms
    uint16_t buffer_u16[15];
    for (uint8_t i = 0; i < 15; i++) {
        buffer_u16[i] = buffer[2 + i * 2 + 1];
        buffer_u16[i] += (buffer[2 + i * 2] << 8);
    }

    // put it into a nice struct :)
    memcpy((void *) &data, (void *) buffer_u16, 30);

    Serial.print("PM: ");
    Serial.print(data.pm10_env);
    Serial.print(" Checksum: ");
    Serial.println(data.checksum);

    if (sum != data.checksum) {
        Serial.println("Checksum failure");
        return false;
    }
    // success!
    return true;
}

void draw_value(String value, String label) {
    tft.setFont(&custom_font);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);

    int16_t x1, y1;
    uint16_t w, h;

    tft.getTextBounds(value, 0, 0, &x1, &y1, &w, &h);

    int16_t x = 160 / 2 - w / 2,
            y = 128 / 2 + header_y + header_circle_r * 2 - h / 2;

    tft.setCursor(x, y);
    tft.println(value);

    tft.setFont(&LineramaBold_10pt);
    tft.setTextSize(1);

    y += h;

    tft.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

    tft.setTextColor(0xd6ba);
    tft.setCursor(160 / 2 - w / 2, y + h / 2);
    tft.print(label);

    tft.setFont();
}