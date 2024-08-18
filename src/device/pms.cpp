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
        Serial.println("PM: no data");
        return false;
    }

    // Read a byte at a time until we get to the special '0x42' start-byte
    if (_stream.peek() != 0x42) {
        _stream.read();
        Serial.println("PM: bad header");
        return false;
    }

    // Now read all 32 bytes
    if (_stream.available() < 32) {
        Serial.println("PM: not enough data");
        return false;
    }

    uint8_t buffer[32];
    uint16_t sum = 0;
    _stream.readBytes(buffer, 32);

    // get checksum ready
    for (uint8_t i = 0; i < 30; i++) {
        sum += buffer[i];
    }

    /* debugging
    for (uint8_t i=2; i<32; i++) {
      Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
    }
    Serial.println();
    */

    // The data comes in endian'd, this solves it, so it works on all platforms
    uint16_t buffer_u16[15];
    for (uint8_t i = 0; i < 15; i++) {
        buffer_u16[i] = buffer[2 + i * 2 + 1];
        buffer_u16[i] += (buffer[2 + i * 2] << 8);
    }

    // put it into a nice struct :)
    memcpy((void *) &data, (void *) buffer_u16, 30);

    Serial.print("PM: ");
    Serial.print(data.pm10_env);
    Serial.print(" / ");
    Serial.print(data.pm25_env);
    Serial.print(" / ");
    Serial.print(data.pm100_env);
    Serial.print(" Checksum: ");
    Serial.println(data.checksum);

    if (sum != data.checksum) {
        Serial.println("Checksum failure");
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
