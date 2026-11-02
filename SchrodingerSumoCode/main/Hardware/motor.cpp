#include <cstdint>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/i2c_master.h"
#include "as5600.h"

using namespace Motor;

MTR LEFT {

    .PWML_PIN = GPIO_NUM_12,
    .PWMR_PIN = GPIO_NUM_13,

    .PWML = LEDC_CHANNEL_0,
    .PWMR = LEDC_CHANNEL_1,

    .ISL_PIN = GPIO_NUM_3,
    .ISR_PIN = GPIO_NUM_10,

    .ISL = ADC_CHANNEL_0,
    .ISR = ADC_CHANNEL_1

};

MTR RIGHT {

    .PWML_PIN = GPIO_NUM_40,
    .PWMR_PIN = GPIO_NUM_41,

    .PWML = LEDC_CHANNEL_2,
    .PWMR = LEDC_CHANNEL_3,

    .ISL_PIN = GPIO_NUM_19,
    .ISR_PIN = GPIO_NUM_20,

    .ISL = ADC_CHANNEL_2,
    .ISR = ADC_CHANNEL_3

};


MTR::init() {



}



