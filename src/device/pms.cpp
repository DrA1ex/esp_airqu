#include "pms.h"

struct PmsDataPacket {
    uint16_t framelen;
    uint16_t pm10_standard, pm25_standard, pm100_standard;
    uint16_t pm10_env, pm25_env, pm100_env;
    uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
};

PmsDevice::PmsDevice(uint8_t uart) : _data{}, _stream(uart) {}

bool PmsDevice::read() {
    bool success = _refresh_data();

    if (!success) {
        while (_stream.available()) _stream.read();
    }

    return success;
}
bool PmsDevice::_refresh_data() {
    PmsDataPacket data{};

    if (!_stream.available()) {
        D_PRINT("PM: no data");
        return false;
    }

    // Read a byte at a time until we get to the special '0x42' start-byte
    if (_stream.peek() != 0x42) {
        _stream.read();
        D_PRINT("PM: bad header");
        return false;
    }

    // Now read all 32 bytes
    if (_stream.available() < 32) {
        D_PRINT("PM: not enough data");
        return false;
    }

    uint8_t buffer[32];
    uint16_t sum = 0;
    _stream.readBytes(buffer, 32);

    // get checksum ready
    for (uint8_t i = 0; i < 30; i++) {
        sum += buffer[i];
    }

    D_WRITE("PM: read data ")
        VERBOSE(D_PRINT_HEX(buffer, sizeof(buffer)));

    // The data comes in endian'd, this solves it, so it works on all platforms
    uint16_t buffer_u16[15];
    for (uint8_t i = 0; i < 15; i++) {
        buffer_u16[i] = buffer[2 + i * 2 + 1];
        buffer_u16[i] += buffer[2 + i * 2] << 8;
    }

    // put it into a nice struct :)
    memcpy(&data, buffer_u16, 30);

    D_PRINTF("PM: Parsed data: %i / %i / %i Checksum: 0x%0.4x\r\n", data.pm10_env, data.pm25_env, data.pm100_env, data.checksum);

    if (sum != data.checksum) {
        D_PRINT("PM: Checksum failure");
        return false;
    }

    // Success

    _data.pm10_env = data.pm10_env;
    _data.pm25_env = data.pm25_env;
    _data.pm100_env = data.pm100_env;

    return true;
}

void PmsDevice::begin(int8_t rx_pin, int8_t tx_pin) {
    _stream.begin(9600, SERIAL_8N1, rx_pin, tx_pin);
}
