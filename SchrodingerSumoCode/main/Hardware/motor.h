#pragma once

#include <cstdint>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

namespace Motor {

    class MTR {
        public:

            // Propiedades
            gpio_num_t PWML_PIN;
            gpio_num_t PWMR_PIN;

            ledc_channel_t PWML;
            ledc_channel_t PWMR;

            gpio_num_t ISL_PIN;
            gpio_num_t ISR_PIN;

            adc_channel_t ISL;
            adc_channel_t ISR;

            int16_t RPM = 0;

            bool PID_STATE = 1;
            

            // Funciones
            void init();
            void move(uint16_t set_point);

    };

}


