#include "app/application.h"

Application ApplicationInstance;

void setup() {
#ifdef DEBUG
    Serial.begin(115200); {
        auto start_t = millis();
        while (!Serial && (millis() - start_t) < 15000ul) delay(1);
    }

    delay(2000);
#endif

    analogWriteResolution(PWM_RESOLUTION);
    analogWriteFrequency(std::min<uint32_t>(PWM_FREQUENCY, PWM_MAX_FREQUENCY));

    ApplicationInstance.begin();
}

void loop() {
    ApplicationInstance.event_loop();
}
