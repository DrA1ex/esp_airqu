#pragma once

#include <cstdint>
#include <cmath>

#define PWM_BITS                                (8u)
#define PWM_FREQ                                (20000u)
#define PWM_MAX_VALUE                           uint16_t((1 << PWM_BITS) - 1)

#define FAN_SPEED_DEFAULT                       uint16_t(128)
#define TFT_CONTRAST_DEFAULT                    uint16_t(96)
#define OLED_CONTRAST_DEFAULT                   uint16_t(32)

#define BTN_SILENCE_INTERVAL                    (50u)
#define BTN_HOLD_INTERVAL                       (1000u)
#define BTN_PRESS_WAIT_INTERVAL                 (500u)
#define BTN_RESET_INTERVAL                      (1000u)