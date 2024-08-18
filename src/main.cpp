#include <HardwareSerial.h>
#include <WiFi.h>

#include <GyverBME280.h>
#include <MHZ19.h>

#include "hardware.h"

#include "device/pms.h"

#include "display/oled.h"
#include "display/tft.h"

#include "misc/button.h"

GyverBME280 bme;

#ifdef OLED_ENABLED
OledDisplay<SSD1306_128x64> oled;
#endif

#ifdef TFT_ENABLED
TftDisplay tft(SPI_TFT, TFT_CS, TFT_DC, TFT_RST, TFT_LED);
#endif

HardwareSerial co2Uart(UART_CO2);
MHZ19 Mhz19;

PmsDevice pmsDevice(UART_PMS);

static Button<BTN1_PIN> button1;
static Button<BTN2_PIN> button2;

SensorData sensorData;

void IRAM_ATTR BtnInterrupt1() {
    Serial.println("Btn1 pressed");
}

void IRAM_ATTR BtnInterrupt2() {
    Serial.println("Btn2 pressed");
}

void setup() {
    Serial.begin(115200);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(1);

    analogWrite(FAN_PWM_PIN, 512);

    Serial.println("Begin");

    oled.begin();
    tft.begin();

    Wire.begin();
    bme.begin(BME_ADDRESS);

    co2Uart.begin(9600, SERIAL_8N1, CO2_RX, CO2_TX);

    Mhz19.begin(co2Uart);
    Mhz19.setRange(5000);
    Mhz19.autoCalibration(false);

    pmsDevice.begin(PMS_RX, PMS_TX);

    button1.begin();
    button2.begin();

    Serial.println("All done");
}

void loop() {
    Serial.println("Loop begin");

    sensorData.humidity = bme.readHumidity();
    sensorData.temperature = bme.readTemperature();

    Serial.print("Hum: ");
    Serial.print(sensorData.humidity);
    Serial.print(" Temp: ");
    Serial.println(sensorData.temperature);

    sensorData.co2 = Mhz19.getCO2(false);

    Serial.print("CO2: ");
    Serial.println(sensorData.co2);

    Serial.println("Reading PM...");
    if (pmsDevice.read()) {
        sensorData.pm10 = pmsDevice.data().pm10_env;
        sensorData.pm25 = pmsDevice.data().pm25_env;
        sensorData.pm100 = pmsDevice.data().pm100_env;
    }

    oled.update(sensorData);
    Serial.println("OLED done");

    tft.update(sensorData);
    Serial.println("TFT done");

    delay(3000);
}