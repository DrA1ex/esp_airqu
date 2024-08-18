#pragma once

#include <Arduino.h>

#include "const.h"

typedef void (*ButtonOnClickFn)(uint8_t count);
typedef ButtonOnClickFn ButtonOnHoldFn;

template<uint8_t PIN>
class Button {
    static volatile bool _hold;
    static volatile int _click_count;

    static volatile unsigned long _last_impulse_time;

    static ButtonOnClickFn _click_handler;
    static ButtonOnHoldFn _hold_handler;

public:
    void begin();

    void handle();

    inline void set_on_click(ButtonOnClickFn fn) { _click_handler = fn; }
    inline void set_on_hold(ButtonOnHoldFn fn) { _hold_handler = fn; }

private:
    static void _handle_interrupt();

    static void _handle_rising_interrupt();
    static void _handle_falling_interrupt();
};

template<uint8_t PIN> volatile bool Button<PIN>::_hold = false;
template<uint8_t PIN> volatile int Button<PIN>::_click_count = 0;
template<uint8_t PIN> volatile unsigned long Button<PIN>::_last_impulse_time = 0;
template<uint8_t PIN> ButtonOnClickFn Button<PIN>::_click_handler = nullptr;
template<uint8_t PIN> ButtonOnHoldFn Button<PIN>::_hold_handler = nullptr;

template<uint8_t PIN>
void Button<PIN>::begin() {
    pinMode(PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN), _handle_interrupt, CHANGE);
}

template<uint8_t PIN>
void Button<PIN>::_handle_interrupt() {
    auto silence_time = millis() - _last_impulse_time;
    _last_impulse_time = millis();

    if (silence_time < BTN_SILENCE_INTERVAL) return;

    if (digitalRead(PIN)) {
        _handle_rising_interrupt();
    } else {
        _handle_falling_interrupt();
    }
}

template<uint8_t PIN>
void Button<PIN>::_handle_rising_interrupt() {
    const auto delta = millis() - _last_impulse_time;
    if (delta > BTN_RESET_INTERVAL) {
        _click_count = 0;
    }
}

template<uint8_t PIN>
void Button<PIN>::_handle_falling_interrupt() {
    if (_hold) {
        _hold = false;
        _click_count = 0;
    } else {
        _click_count++;
    }
}

template<uint8_t PIN>
void Button<PIN>::handle() {
    unsigned long delta = millis() - _last_impulse_time;

    const bool state = digitalRead(PIN);
    if (!_hold && state && delta >= BTN_HOLD_INTERVAL) {
        _hold = true;
        _click_count++;
    } else if (_click_count && !_hold && delta >= BTN_RESET_INTERVAL) {
        _click_count = 0;
    } else if (_hold && !state) {
        _hold = false;
        _click_count = 0;
    }

    if (_hold) {

        if (_hold_handler != nullptr) {
            _hold_handler(_click_count);
        }
    } else if (_click_count && delta > BTN_PRESS_WAIT_INTERVAL) {

        if (_click_handler != nullptr) {
            _click_handler(_click_count);
        }

        _click_count = 0;
    }
}