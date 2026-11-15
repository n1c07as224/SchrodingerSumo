#include <cstdint>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "adc_unit.h"
#include "pwm_timer.h"
#include "motor.h"

namespace Motor {

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


void initAll() {

    ADC_UNIT::init();


    PWM_TIMER::init(LEDC_TIMER_0, LEDC_TIMER_10_BIT);


    // Configurar canales PWM
    auto configPWM = [](gpio_num_t pin, ledc_channel_t channel) {

        ledc_channel_config_t config{};

        config.gpio_num = pin;
        config.speed_mode = LEDC_LOW_SPEED_MODE;
        config.channel = channel;
        config.timer_sel = LEDC_TIMER_0;
        config.duty = 0;
        config.hpoint = 0;

        ESP_ERROR_CHECK(ledc_channel_config(&config));
    };


    // Motor izquierdo
    configPWM(LEFT.PWML_PIN, LEFT.PWML);
    configPWM(LEFT.PWMR_PIN, LEFT.PWMR);

    // Motor derecho
    configPWM(RIGHT.PWML_PIN, RIGHT.PWML);
    configPWM(RIGHT.PWMR_PIN, RIGHT.PWMR);

}


}
