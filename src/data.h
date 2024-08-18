#pragma once

#include <cstdint>

struct SensorData {
    int co2;

    float temperature;
    float humidity;

    uint16_t pm10;
    uint16_t pm25;
    uint16_t pm100;
};