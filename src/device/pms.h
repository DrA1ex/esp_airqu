#include <cstdint>

#include "Arduino.h"

#include "data.h"

class PmsDevice {
    PmsData _data;
    HardwareSerial _stream;

    bool _refresh_data();

public:
    explicit PmsDevice(uint8_t uart);

    void begin(int8_t rx_pin, int8_t tx_pin);

    bool read();

    inline const PmsData &data() const { return _data; };
};