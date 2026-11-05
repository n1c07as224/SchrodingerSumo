#include "driver/ledc.h"
#include "pwm_timer.h"


using namespace PWM_TIMER; 


void init(ledc_timer_t timer_num, ledc_timer_bit_t bits_resolution) {

    ledc_timer_config_t timer_config{};
    timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_config.timer_num = timer_num;
    timer_config.duty_resolution = bits_resolution;
    timer_config.freq_hz = 5000;
    timer_config.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

}

