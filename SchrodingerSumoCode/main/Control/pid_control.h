#pragma once

#include <cstdint>
#include "esp_timer.h"


class PID {
    public:

    // Propiedades
    float Kp = 0.0f;
    float Ki = 0.0f;
    float Kd = 0.0f;

    int16_t out_max = 1000;
    int16_t out_min = 20;

    int16_t* out_pid = NULL;
    int16_t* actual_input = NULL;

    // Funciones
    void PID_calc(int16_t set_point);

    private:

    int16_t last_input = 0;
    uint64_t previous_time = 0;
    float integ_error = 0.0f;

};




