#include "pid_control.h"

#include <cstdint>
#include "esp_timer.h"

void PID::PID_calc(int16_t set_point) {

    uint64_t current_time = esp_timer_get_time();

    float error = set_point - *actual_input;
    float dTime = (current_time - previous_time) / 1000000.0f;

    // Integral
    integ_error += error * dTime;

    if(integ_error > out_max) integ_error= out_max;
    else if(integ_error < out_min) integ_error= out_min;

    // Derivada
    float dInput = (*actual_input - last_input) / dTime;

    float output = Kp * error + Ki * integ_error - Kd * dInput;

    if(output > out_max) output = out_max;
    else if(output < out_min) output = out_min;

	*out_pid = output;

    last_input = *actual_input;
    previous_time = current_time;


}



