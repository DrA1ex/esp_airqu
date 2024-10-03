import {AppConfigBase} from "./lib/index.js";

export const DEFAULT_ADDRESS = "esp_airqu.local";
export const CONFIG_STRING_SIZE = 32;

export const PacketType = {
    CO2: 0x20,
    TEMPERATURE: 0x21,
    HUMIDITY: 0x22,

    PM_10: 0x30,
    PM_25: 0x31,
    PM_100: 0x32,

    SYS_CONFIG_MDNS_NAME: 0x60,

    SYS_CONFIG_WIFI_MODE: 0x61,
    SYS_CONFIG_WIFI_SSID: 0x62,
    SYS_CONFIG_WIFI_PASSWORD: 0x63,
    SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL: 0x65,

    SYS_CONFIG_MQTT_ENABLED: 0x70,
    SYS_CONFIG_MQTT_HOST: 0x71,
    SYS_CONFIG_MQTT_PORT: 0x72,
    SYS_CONFIG_MQTT_USER: 0x73,
    SYS_CONFIG_MQTT_PASSWORD: 0x74,
    SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS: 0x75,

    GET_SENSOR_DATA: 0xa0,
}

const CfgStrLength = 32;

export class Config extends AppConfigBase {
    power;
    sys_config;

    sensor_data;

    constructor(props) {
        super(props);

        this.lists["wifi_mode"] = [
            {code: 0, name: "AP"},
            {code: 1, name: "STA"},
        ]
    }

    async load(ws) {
        const [_, sensorDataPacket] = await Promise.all([
            super.load(ws),
            ws.request(PacketType.GET_SENSOR_DATA),
        ]);

        this.sensor_data = this.#parseSensorData(sensorDataPacket.parser());
    }

    parse(parser) {
        this.sys_config = this.#parseSysConfig(parser);
    }

    #parseSysConfig(parser) {
        return {
            mdns_name: parser.readFixedString(CONFIG_STRING_SIZE),

            wifi_mode: parser.readUint8(),
            wifi_ssid: parser.readFixedString(CONFIG_STRING_SIZE),
            wifi_password: parser.readFixedString(CONFIG_STRING_SIZE),
            wifi_connection_timeout: parser.readUint32(),

            mqtt_enabled: parser.readBoolean(),
            mqtt_host: parser.readFixedString(CONFIG_STRING_SIZE),
            mqtt_port: parser.readUint16(),
            mqtt_user: parser.readFixedString(CONFIG_STRING_SIZE),
            mqtt_password: parser.readFixedString(CONFIG_STRING_SIZE),
        };
    }

    #parseSensorData(parser) {
        return {
            co2: parser.readInt32(),

            temperature: parser.readFloat32(),
            humidity: parser.readFloat32(),

            pms: {
                pm10_env: parser.readUint16(),
                pm25_env: parser.readUint16(),
                pm100_env: parser.readUint16(),
            }
        };
    }
}