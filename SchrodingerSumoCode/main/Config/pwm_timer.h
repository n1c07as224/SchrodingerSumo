#pragma once

#include "driver/ledc.h"

namespace PWM_TIMER {

   void init(ledc_timer_t timer_num, ledc_timer_bit_t bits_resolution); 

}




