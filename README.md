# airqu-esp32
Air Quality monitor for ESP32


## Schematics

<img width="887" alt="image" src="https://github.com/user-attachments/assets/04f780c2-2373-4a84-981a-415270994d5e">

Project: [EasyEDA Pro](https://github.com/user-attachments/files/16648917/ProProject_AirQu_ESP32_2024-07-24.epro.zip)

## Hardware:

| Type             | Hardware        |
|------------------|-----------------|
| Controller       | ESP32 38P       |
| CO2              | MH-Z19B         |
| Temp. / Humidity | BME280 3.3V     |
| Particle Matter  | PMS5003 or other|
| TFT Display      | Any ST7735      |
| OLED Display     | Any SSD1306     |
| Buttons          | Any             |
| Fan              | Any 12V 2.5mm   |

## Installation

Install (Platform.IO)[https://platformio.org/install] first and then:

```bash
git clone https://github.com/DrA1ex/airqu-esp32.git
cd airqu-esp32

pio run -t upload
```
