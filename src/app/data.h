#pragma once

#include <cstdint>
#include <lib/utils/enum.h>

struct PmsData {
    uint16_t pm10_env;
    uint16_t pm25_env;
    uint16_t pm100_env;
};

MAKE_ENUM_AUTO(SensorState, uint8_t,
    NOT_READY,
    CRITICAL,
    WARNING,
    GOOD
);

struct SensorDataState {
    SensorState co2 = SensorState::NOT_READY;
    SensorState temperature = SensorState::NOT_READY;
    SensorState humidity = SensorState::NOT_READY;
    SensorState pms = SensorState::NOT_READY;
};

struct SensorData {
    uint32_t co2;

    float temperature;
    float humidity;

    PmsData pms;

    SensorDataState state;
};
