#pragma once

#include <lib/base/metadata.h>
#include <lib/utils/metadata.h>

#include "app/config.h"
#include "app/data.h"

DECLARE_META_TYPE(AppMetaProperty, PacketType)

DECLARE_META(SysConfigMeta, AppMetaProperty,
    MEMBER(FixedString, mdns_name),
    MEMBER(Parameter<uint8_t>, wifi_mode),
    MEMBER(FixedString, wifi_ssid),
    MEMBER(FixedString, wifi_password),
    MEMBER(Parameter<uint32_t>, wifi_max_connection_attempt_interval),
    MEMBER(Parameter<bool>, mqtt),
    MEMBER(FixedString, mqtt_host),
    MEMBER(Parameter<uint16_t>, mqtt_port),
    MEMBER(FixedString, mqtt_user),
    MEMBER(FixedString, mqtt_password),
)

DECLARE_META(PmsDataMeta, AppMetaProperty,
    MEMBER(Parameter<uint16_t>, pm10_env),
    MEMBER(Parameter<uint16_t>, pm25_env),
    MEMBER(Parameter<uint16_t>, pm100_env),
)

DECLARE_META(SensorDataMeta, AppMetaProperty,
    MEMBER(Parameter<uint32_t>, co2),
    MEMBER(Parameter<float>, temperature),
    MEMBER(Parameter<float>, humidity),

    SUB_TYPE(PmsDataMeta, pms)
)

DECLARE_META(DataConfigMeta, AppMetaProperty,
    MEMBER(ComplexParameter<Config>, config),
    MEMBER(ComplexParameter<SensorData>, sensor_data),
)

DECLARE_META(ConfigMetadata, AppMetaProperty,
    SUB_TYPE(SysConfigMeta, sys_config),

    SUB_TYPE(SensorDataMeta, sensor_data),
    SUB_TYPE(DataConfigMeta, data),
)

inline ConfigMetadata build_metadata(Config &config, SensorData &sensor_data) {
    return {
        .sys_config = {
            .mdns_name = {
                PacketType::SYS_CONFIG_MDNS_NAME,
                {config.sys_config.mdns_name, CONFIG_STRING_SIZE}
            },
            .wifi_mode = {
                PacketType::SYS_CONFIG_WIFI_MODE,
                (uint8_t *) &config.sys_config.wifi_mode
            },
            .wifi_ssid = {
                PacketType::SYS_CONFIG_WIFI_SSID,
                {config.sys_config.wifi_ssid, CONFIG_STRING_SIZE}
            },
            .wifi_password = {
                PacketType::SYS_CONFIG_WIFI_PASSWORD,
                {config.sys_config.wifi_password, CONFIG_STRING_SIZE}
            },
            .wifi_max_connection_attempt_interval = {
                PacketType::SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL,
                &config.sys_config.wifi_max_connection_attempt_interval
            },
            .mqtt = {
                PacketType::SYS_CONFIG_MQTT_ENABLED,
                &config.sys_config.mqtt
            },
            .mqtt_host = {
                PacketType::SYS_CONFIG_MQTT_HOST,
                {config.sys_config.mqtt_host, CONFIG_STRING_SIZE}
            },
            .mqtt_port = {
                PacketType::SYS_CONFIG_MQTT_PORT,
                &config.sys_config.mqtt_port
            },
            .mqtt_user = {
                PacketType::SYS_CONFIG_MQTT_USER,
                {config.sys_config.mqtt_user, CONFIG_STRING_SIZE}
            },
            .mqtt_password = {
                PacketType::SYS_CONFIG_MQTT_PASSWORD,
                {config.sys_config.mqtt_password, CONFIG_STRING_SIZE}
            },
        },

        .sensor_data{
            .co2 = {
                PacketType::CO2,
                MQTT_OUT_TOPIC_CO2,
                &sensor_data.co2
            },
            .temperature = {
                PacketType::TEMPERATURE,
                MQTT_OUT_TOPIC_TEMPERATURE,
                &sensor_data.temperature
            },
            .humidity = {
                PacketType::HUMIDITY,
                MQTT_OUT_TOPIC_HUMIDITY,
                &sensor_data.humidity
            },

            .pms = {
                .pm10_env = {
                    PacketType::PM_10,
                    MQTT_OUT_TOPIC_PM_10,
                    &sensor_data.pms.pm10_env
                },
                .pm25_env = {
                    PacketType::PM_25,
                    MQTT_OUT_TOPIC_PM_25,
                    &sensor_data.pms.pm25_env
                },
                .pm100_env = {
                    PacketType::PM_100,
                    MQTT_OUT_TOPIC_PM_100,
                    &sensor_data.pms.pm100_env
                },
            }
        },

        .data{
            .config = ComplexParameter(&config),
            .sensor_data = ComplexParameter(&sensor_data)
        },
    };
}
