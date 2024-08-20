# airqu-esp32
Air Quality monitor for ESP32


## Schematics

<img width="887" alt="image" src="https://github.com/user-attachments/assets/04f780c2-2373-4a84-981a-415270994d5e">

<img width="868" alt="image" src="https://github.com/user-attachments/assets/7c8fe11f-f467-487e-a4e9-d680bf2d9a22">

<img width="430" alt="image" src="https://github.com/user-attachments/assets/321c2131-4625-4528-ac73-f0c5bce21910">


Project: [EasyEDA Pro](https://github.com/user-attachments/files/16648917/ProProject_AirQu_ESP32_2024-07-24.epro.zip)

## Hardware:

| Type             | Hardware        | Interface     |
|------------------|-----------------|---------------|
| Controller       | ESP32 38P       |               |
| Particle Matter  | PMS5003 or other|   UART        |
| CO2              | MH-Z19B         |   UART        |
| Temp. / Humidity | BME280 3.3V     |   I2C         |
| OLED Display     | Any SSD1306     |   I2C         |
| TFT Display      | Any ST7735      |   SPI         |
| Buttons          | Any             |               |
| Fan              | Any 12V 40mm    |               |

## Installation

Install [Platform.IO](https://platformio.org/install) first and then:

```bash
git clone https://github.com/DrA1ex/airqu-esp32.git
cd airqu-esp32

pio run -t upload
```
