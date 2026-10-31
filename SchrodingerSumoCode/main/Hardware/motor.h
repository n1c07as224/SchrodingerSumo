#pragma once

namespace Motor1 {

constexpr gpio_num_t PWML_PIN = GPIO_NUM_12;
constexpr gpio_num_t PWMR_PIN = GPIO_NUM_13;

constexpr ledc_channel_t PWML = LEDC_CHANNEL_0;
constexpr ledc_channel_t PWMR = LEDC_CHANNEL_1;

constexpr gpio_num_t ISL_PIN = GPIO_NUM_3;
constexpr gpio_num_t ISR_PIN = GPIO_NUM_10;

constexpr adc_channel_t ISL = ADC_CHANNEL_0;
constexpr adc_channel_t ISR = ADC_CHANNEL_1;

}




namespace Motor2 {

constexpr gpio_num_t PWML_PIN = GPIO_NUM_40;
constexpr gpio_num_t PWMR_PIN = GPIO_NUM_41;

constexpr ledc_channel_t PWML = LEDC_CHANNEL_2;
constexpr ledc_channel_t PWMR = LEDC_CHANNEL_3;

constexpr gpio_num_t ISL_PIN = GPIO_NUM_19;
constexpr gpio_num_t ISR_PIN = GPIO_NUM_20;

constexpr adc_channel_t ISL = ADC_CHANNEL_2;
constexpr adc_channel_t ISR = ADC_CHANNEL_3;

}




