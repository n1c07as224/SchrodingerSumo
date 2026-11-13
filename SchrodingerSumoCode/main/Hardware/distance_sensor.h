#pragma once

#include <cstdint>
#include "driver/gpio.h"
#include "vl53l0x.h"

namespace DistanceS {

    class DS {
        public:

            // Propiedades
            uint8_t address = 0x29;
            vl53l0x_handle_t sensor_handle = NULL;
            gpio_num_t XShut;
            uint16_t distance = 0;
            
            // Funciones
            uint16_t read();

    };


    void readDistances();

    void initAll();

};
