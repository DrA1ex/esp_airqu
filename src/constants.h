#pragma once

#include "credentials.h"
#include "hardware.h"
#include "sys_constants.h"

#define WIFI_MODE                               (WIFI_AP_MODE)

#define WEB_AUTH                                (1)                     // Use basic auth for non-local connections

#define WIFI_CONNECTION_CHECK_INTERVAL          (5000u)                 // Interval (ms) between Wi-Fi connection check
#define WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL    (120000u)               // Max time (ms) to wait for Wi-Fi connection before switch to AP mode


// 0 - Newer switch to AP mode

#define MDNS_NAME                               "esp_airqu"

#define FAN_SPEED_DEFAULT                       uint16_t(128)
#define TFT_CONTRAST_DEFAULT                    uint16_t(96)
#define OLED_CONTRAST_DEFAULT                   uint16_t(32)

#define DISPLAY_UPDATE_TIMEOUT_DEFAULT          (3000)
#define SENSOR_UPDATE_TIMEOUT_DEFAULT           (3000)
#define SENSOR_DATA_SEND_TIMEOUT_DEFAULT        (15000)

#define MQTT                                    (0)                     // Enable MQTT server

#define MQTT_CONNECTION_TIMEOUT                 (15000u)                // Connection attempt timeout to MQTT server
#define MQTT_RECONNECT_TIMEOUT                  (5000u)                 // Time before new reconnection attempt to MQTT server


#define MQTT_PREFIX                             ""

#define MQTT_OUT_PREFIX                         MQTT_PREFIX "/out"
#define MQTT_OUT_TOPIC_CO2                      MQTT_OUT_PREFIX "/co2"
#define MQTT_OUT_TOPIC_TEMPERATURE              MQTT_OUT_PREFIX "/temperature"
#define MQTT_OUT_TOPIC_HUMIDITY                 MQTT_OUT_PREFIX "/humidity"
#define MQTT_OUT_TOPIC_PM_10                    MQTT_OUT_PREFIX "/pm_10"
#define MQTT_OUT_TOPIC_PM_25                    MQTT_OUT_PREFIX "/pm_25"
#define MQTT_OUT_TOPIC_PM_100                   MQTT_OUT_PREFIX "/pm_100"
