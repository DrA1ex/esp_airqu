#pragma once

#include <cstdint>
#include <lib/network/wifi.h>
#include <lib/utils/enum.h>

#include "constants.h"

MAKE_ENUM(PacketType, uint8_t,
    POWER, 0x00,
    BRIGHTNESS, 0x01,
    FAN_SPEED, 0x02,

    CO2, 0x20,
    TEMPERATURE, 0x21,
    HUMIDITY, 0x22,

    PM_10, 0x30,
    PM_25, 0x31,
    PM_100, 0x32,

    SYS_CONFIG_MDNS_NAME, 0x60,

    SYS_CONFIG_WIFI_MODE, 0x61,
    SYS_CONFIG_WIFI_SSID, 0x62,
    SYS_CONFIG_WIFI_PASSWORD, 0x63,
    SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL, 0x65,

    SYS_CONFIG_MQTT_ENABLED, 0x70,
    SYS_CONFIG_MQTT_HOST, 0x71,
    SYS_CONFIG_MQTT_PORT, 0x72,
    SYS_CONFIG_MQTT_USER, 0x73,
    SYS_CONFIG_MQTT_PASSWORD, 0x74,
    SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS, 0x75,

    HARDWARE_FAN_ENABLED, 0x80,

    GET_SENSOR_DATA, 0xa0,
)

typedef char ConfigString[CONFIG_STRING_SIZE];

struct __attribute ((packed)) HardwareConfig {
    bool fan_enabled = FAN_ENABLED;
};

struct __attribute ((packed)) SysConfig {
    ConfigString mdns_name{MDNS_NAME};

    WifiMode wifi_mode = WIFI_MODE;
    ConfigString wifi_ssid{WIFI_SSID};
    ConfigString wifi_password{WIFI_PASSWORD};
    uint32_t wifi_max_connection_attempt_interval = WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL;

    bool mqtt = MQTT;
    ConfigString mqtt_host = MQTT_HOST;
    uint16_t mqtt_port = MQTT_PORT;
    ConfigString mqtt_user = MQTT_USER;
    ConfigString mqtt_password = MQTT_PASSWORD;
};

struct __attribute ((packed)) Config {
    bool power = true;
    uint16_t brightness = DISPLAY_CONTRAST_DEFAULT;
    uint16_t fan_speed = FAN_SPEED_DEFAULT;

    SysConfig sys_config{};
    HardwareConfig hardware_config{};
};
