import {PacketType} from "./config.js";
import {PWM_MAX_VALUE} from "./sys_constants.js";

/**
 * @type {PropertiesConfig}
 */
export const Properties = [{
    key: "sensor_data", section: "Sensors", props: [
        {
            key: "sensor_data.co2", type: "label", kind: "Uint32", cmd: PacketType.CO2,
            transform: (value) => {
                if (value === 500) return ['CO2', 'WARM UP'];

                return ['CO2', `${value} ppm`];
            }
        },
        {
            key: "sensor_data.temperature", type: "label", kind: "Float32", cmd: PacketType.TEMPERATURE,
            transform: (value) => {
                return ['Temperature', `${value.toFixed(1)} Cº`];
            }
        }, {
            key: "sensor_data.humidity", type: "label", kind: "Float32", cmd: PacketType.HUMIDITY,
            transform: (value) => {
                return ['Humidity', `${value.toFixed(1)} %`];
            }
        },
        {key: "sensor_data.pms.pm10_env", type: "label", kind: "Uint16", cmd: PacketType.PM_10, transform: (value) => ['PM 1.0', value]},
        {key: "sensor_data.pms.pm25_env", type: "label", kind: "Uint16", cmd: PacketType.PM_25, transform: (value) => ['PM 2.5', value]},
        {key: "sensor_data.pms.pm100_env", type: "label", kind: "Uint16", cmd: PacketType.PM_100, transform: (value) => ['PM 10.0', value]},
    ]
}, {
    key: "general", section:"General", props: [
        {key: "power", title: "Power", type: "trigger", kind: "Boolean", cmd: PacketType.POWER},
        {key: "brightness", title: "Brightness", type: "wheel", kind: "Uint16", limit: PWM_MAX_VALUE, cmd: PacketType.BRIGHTNESS},
        {key: "fan_speed", title: "Fan Speed", type: "wheel", kind: "Uint16", limit: PWM_MAX_VALUE, cmd: PacketType.FAN_SPEED, visibleIf: "hardware_config.fan_enabled"},
    ]
}, {
    key: "sys_config", section: "Settings", collapse: "true", props: [
        {type: "title", label: "Hardware"},
        {key: "hardware_config.fan_enabled", title: "Fan enabled", type: "trigger", kind: "Boolean", cmd: PacketType.HARDWARE_FAN_ENABLED},

        {type: "title", label: "mDNS"},
        {key: "sys_config.mdns_name", title: "Name", type: "text", kind: "FixedString", cmd: PacketType.SYS_CONFIG_MDNS_NAME, maxLength: 32},

        {type: "title", label: "WiFi"},
        {key: "sys_config.wifi_mode", title: "Mode", type: "select", kind: "Uint8", cmd: PacketType.SYS_CONFIG_WIFI_MODE, list: "wifi_mode"},
        {key: "sys_config.wifi_ssid", title: "SSID", type: "text", kind: "FixedString", cmd: PacketType.SYS_CONFIG_WIFI_SSID, maxLength: 32},

        {type: "title", label: "WiFi Extra"},
        {key: "sys_config.wifi_password", title: "Password", type: "password", kind: "FixedString", cmd: PacketType.SYS_CONFIG_WIFI_PASSWORD, maxLength: 32},
        {key: "sys_config.wifi_connection_timeout", title: "Connection Timeout", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL},

        {type: "title", label: "MQTT"},
        {key: "sys_config.mqtt_enabled", title: "MQTT Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_CONFIG_MQTT_ENABLED},
        {key: "sys_config.mqtt_host", title: "MQTT Host", type: "text", kind: "FixedString", cmd: PacketType.SYS_CONFIG_MQTT_HOST, maxLength: 32},
        {key: "sys_config.mqtt_port", title: "MQTT Port", type: "int", kind: "Uint16", cmd: PacketType.SYS_CONFIG_MQTT_PORT, min: 1, limit: 65535},
        {key: "sys_config.mqtt_user", title: "MQTT User", type: "text", kind: "FixedString", cmd: PacketType.SYS_CONFIG_MQTT_USER, maxLength: 32},
        {key: "sys_config.mqtt_password", title: "MQTT Password", type: "password", kind: "FixedString", cmd: PacketType.SYS_CONFIG_MQTT_PASSWORD, maxLength: 32},

        {type: "title", label: "Actions", extra: {m_top: true}},
        {key: "apply_sys_config", type: "button", label: "Apply Settings"}
    ]
}];