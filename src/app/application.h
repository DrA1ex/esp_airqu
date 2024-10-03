#pragma once

#include <GyverBME280.h>
#include <MHZ19.h>

#include "lib/bootstrap.h"

#include "config.h"
#include "constants.h"
#include "metadata.h"

#include "device/pms.h"
#include "device/display/oled.h"
#include "device/display/tft.h"

class Application {
    std::unique_ptr<Bootstrap<Config, PacketType>> _bootstrap = nullptr;
    std::unique_ptr<ConfigMetadata> _metadata = nullptr;

    std::unique_ptr<OledDisplay<OLED_TYPE>> _oled_display;
    std::unique_ptr<TftDisplay> _tft_display;

    std::unique_ptr<HardwareSerial> _co2_uart;
    std::unique_ptr<MHZ19> _mhz19;
    std::unique_ptr<GyverBME280> _bme;
    std::unique_ptr<PmsDevice> _pms_device;

    SensorData _sensor_data{};

public:
    [[nodiscard]] inline Config &config() const { return _bootstrap->config(); }
    [[nodiscard]] inline SysConfig &sys_config() const { return config().sys_config; }

    void begin();
    void event_loop();

    inline void restart() { _bootstrap->restart(); }

private:
    void _setup();
    void _update_data();
    void _redraw_data();
    void _send_notifications();
};
