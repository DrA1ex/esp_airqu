#include "application.h"

#include <esp32-hal-ledc.h>

void Application::begin() {
    D_PRINT("Starting application...");

    if (!LittleFS.begin()) {
        D_PRINT("Unable to initialize FS");
    }

    _bootstrap = std::make_unique<Bootstrap<Config, PacketType>>(&LittleFS);

    auto &sys_config = _bootstrap->config().sys_config;
    _bootstrap->begin({
        .mdns_name = sys_config.mdns_name,
        .wifi_mode = sys_config.wifi_mode,
        .wifi_ssid = sys_config.wifi_ssid,
        .wifi_password = sys_config.wifi_password,
        .wifi_connection_timeout = sys_config.wifi_max_connection_attempt_interval,
        .mqtt_enabled = sys_config.mqtt,
        .mqtt_host = sys_config.mqtt_host,
        .mqtt_port = sys_config.mqtt_port,
        .mqtt_user = sys_config.mqtt_user,
        .mqtt_password = sys_config.mqtt_password,
    });

    _metadata = std::make_unique<ConfigMetadata>(build_metadata(config(), _sensor_data));

    if (config().hardware_config.fan_enabled) {
        ledcAttachPin(FAN_PWM_PIN, FAN_CHANNEL);
        ledcChangeFrequency(1, std::min<uint32_t>(FAN_FREQUENCY, PWM_MAX_FREQUENCY), PWM_RESOLUTION);
        ledcWrite(FAN_CHANNEL, std::min(config().fan_speed, PWM_MAX_VALUE));
    }

    if constexpr (OLED_ENABLED) {
        _oled_display = std::make_unique<OledDisplay<OLED_TYPE>>();
        _oled_display->begin();
        _oled_display->set_contrast(display_brightness());
    }

    if constexpr (TFT_ENABLED) {
        _tft_display = std::make_unique<TftDisplay>(SPI_TFT, TFT_CS, TFT_DC, TFT_RST, TFT_LED);
        _tft_display->begin();
        _tft_display->set_contrast(display_brightness());
    }

    Wire.begin();
    _bme = std::make_unique<GyverBME280>();
    _bme->begin(BME_ADDRESS);

    _co2_uart = std::make_unique<HardwareSerial>(UART_CO2);
    _co2_uart->begin(9600, SERIAL_8N1, CO2_RX, CO2_TX);

    _mhz19 = std::make_unique<MHZ19>();
    _mhz19->begin(*_co2_uart);
    _mhz19->setRange(5000);
    _mhz19->autoCalibration(false);

    _pms_device = std::make_unique<PmsDevice>(UART_PMS);
    _pms_device->begin(PMS_RX, PMS_TX);

    _setup();
}

void Application::_setup() {
    auto &ws_server = _bootstrap->ws_server();
    auto &mqtt_server = _bootstrap->mqtt_server();

    _metadata = std::make_unique<ConfigMetadata>(build_metadata(config(), _sensor_data));
    _metadata->visit([this, &ws_server, &mqtt_server](AbstractPropertyMeta *meta) {
        auto binary_protocol = (BinaryProtocolMeta<PacketType> *) meta->get_binary_protocol();
        if (binary_protocol->packet_type.has_value()) {
            ws_server->register_parameter(*binary_protocol->packet_type, meta->get_parameter());
            VERBOSE(D_PRINTF("WebSocket: Register property %s\r\n", __debug_enum_str(*binary_protocol->packet_type)));
        }

        auto mqtt_protocol = meta->get_mqtt_protocol();
        if (mqtt_protocol->topic_in && mqtt_protocol->topic_out) {
            mqtt_server->register_parameter(mqtt_protocol->topic_in, mqtt_protocol->topic_out, meta->get_parameter());
            VERBOSE(D_PRINTF("MQTT: Register property %s <-> %s\r\n", mqtt_protocol->topic_in, mqtt_protocol->topic_out));
        } else if (mqtt_protocol->topic_out) {
            mqtt_server->register_notification(mqtt_protocol->topic_out, meta->get_parameter());
            VERBOSE(D_PRINTF("MQTT: Register notification -> %s\r\n", mqtt_protocol->topic_out));
        }
    });

    ws_server->register_data_request(PacketType::GET_SENSOR_DATA, _metadata->data.sensor_data);

    ws_server->register_data_request((PacketType) SystemPacketTypeEnum::GET_CONFIG, _metadata->data.config);
    ws_server->register_command((PacketType) SystemPacketTypeEnum::RESTART, [this] { _bootstrap->restart(); });

    _bootstrap->timer().add_interval([this](auto) {
        if (config().power) _update_data();
    }, DISPLAY_UPDATE_TIMEOUT_DEFAULT);

    _bootstrap->timer().add_interval([this](auto) {
        if (config().power) _redraw_data();
    }, SENSOR_UPDATE_TIMEOUT_DEFAULT);

    _bootstrap->timer().add_interval([this](auto) {
        if (config().power) _send_notifications();
    }, SENSOR_DATA_SEND_TIMEOUT_DEFAULT);

    NotificationBus::get().subscribe([this](auto *sender, auto *prop) {
        _process_notifications(sender, prop);
    });
}

void Application::event_loop() {
    _bootstrap->event_loop();
}

void Application::_update_data() {
    _sensor_data.humidity = _bme->readHumidity();
    _sensor_data.temperature = _bme->readTemperature();

    D_PRINTF("Humidity: %.2f\r\n", _sensor_data.humidity);
    D_PRINTF("Temperature: %.2f\r\n", _sensor_data.temperature);

    _sensor_data.co2 = _mhz19->getCO2(false);

    D_PRINTF("CO2: %i\r\n", _sensor_data.co2);

    if (_pms_device->read()) {
        _sensor_data.pms = _pms_device->data();
    }
}

void Application::_redraw_data() {
    if constexpr (OLED_ENABLED) {
        _oled_display->update(_sensor_data);
    }

    if constexpr (TFT_ENABLED) {
        _tft_display->update(_sensor_data);
    }
}

void Application::_send_notifications() {
    auto &bus = NotificationBus::get();

    bus.notify_parameter_changed(this, _metadata->sensor_data.co2);
    bus.notify_parameter_changed(this, _metadata->sensor_data.temperature);
    bus.notify_parameter_changed(this, _metadata->sensor_data.humidity);
    bus.notify_parameter_changed(this, _metadata->sensor_data.pms.pm10_env);
    bus.notify_parameter_changed(this, _metadata->sensor_data.pms.pm25_env);
    bus.notify_parameter_changed(this, _metadata->sensor_data.pms.pm100_env);
}

void Application::_process_notifications(void *sender, const AbstractParameter *prop) {
    if (sender == this) return;

    if (prop == _metadata->power) {
        if constexpr (OLED_ENABLED) _oled_display->set_contrast(display_brightness());
        if constexpr (TFT_ENABLED) _tft_display->set_contrast(display_brightness());

        uint16_t fan_speed = config().power ? std::min(config().fan_speed, PWM_MAX_VALUE) : 0;
        if (config().hardware_config.fan_enabled) ledcWrite(FAN_CHANNEL, fan_speed);

        if (config().power) _send_notifications();
    } else if (prop == _metadata->brightness && config().power) {
        if constexpr (OLED_ENABLED) _oled_display->set_contrast(config().brightness);
        if constexpr (TFT_ENABLED) _tft_display->set_contrast(config().brightness);
    } else if (prop == _metadata->fan_speed && config().hardware_config.fan_enabled && config().power) {
        ledcWrite(FAN_CHANNEL, std::min(config().fan_speed, PWM_MAX_VALUE));
    }

    _bootstrap->save_changes();
}
