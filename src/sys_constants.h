#pragma once

#define WIFI_AP_MODE                            WifiMode::AP
#define WIFI_STA_MODE                           WifiMode::STA

#define WS_MAX_PACKET_SIZE                      (260u)
#define WS_MAX_PACKET_QUEUE                     (10u)

#define PACKET_SIGNATURE                        ((uint16_t) 0xDABA)

#define WEB_PORT                                (80)

#define STORAGE_PATH                            ("/__storage/")
#define STORAGE_HEADER                          ((uint32_t) 0xd0c1f2c3)
#define STORAGE_CONFIG_VERSION                  ((uint8_t) 1)
#define STORAGE_SAVE_INTERVAL                   (60000u)                // Wait before commit settings to FLASH

#define TIMER_GROW_AMOUNT                       (8u)

#define PWM_RESOLUTION                          (8u)
#define PWM_FREQUENCY                           (22000u)
#define PWM_MAX_VALUE                           ((uint16_t)((1u << PWM_RESOLUTION) - 1))
#define PWM_MAX_FREQUENCY                       (40000000ul / (1u << PWM_RESOLUTION))

#define NTP_UPDATE_INTERVAL                     (24ul * 3600 * 1000)

#define RESTART_DELAY                           (500u)
#define APP_LOOP_INTERVAL                       (10u)

#define CONFIG_STRING_SIZE                      (32u)


#define FAN_FREQUENCY                           (40000)
#define FAN_CHANNEL                             (1)
