#pragma once

#include <SPI.h>

#define OLED_ENABLED               (0)
#define OLED_TYPE                  (SSD1306_128x64)

#define TFT_ENABLED                (1)

#define BME_ADDRESS                (0x76)

#define UART_PMS                   (1)

#define PMS_RX                     (33)
#define PMS_TX                     (32)

#define UART_CO2                   (2)
#define CO2_RX                     (17)
#define CO2_TX                     (16)

#define TFT_CS                     (15)
#define TFT_RST                    (25)
#define TFT_DC                     (26)
#define TFT_LED                    (19)

#define SPI_TFT                    (HSPI)

#define FAN_PWM_PIN                (27)

#define BTN1_PIN                   (18)
#define BTN2_PIN                   (5)
