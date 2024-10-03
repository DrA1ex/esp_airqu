#pragma once

#include <cstdint>

struct PmsData {
    uint16_t pm10_env;
    uint16_t pm25_env;
    uint16_t pm100_env;
};

struct SensorData {
    uint32_t co2;

    float temperature;
    float humidity;

    PmsData pms;
};
