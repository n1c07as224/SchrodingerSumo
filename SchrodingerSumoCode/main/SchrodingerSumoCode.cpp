/*

>>--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--<<
|                    Schrodinger Sumo                   |
>>--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--<<
______________________________
|____________________________|
 |                          |
 |        /\__/\            |
 |       /`    '\           |
 |      == x  0 ==          |
 |       \  --  /           |
 |      /        \          |
 |     /          \         |
 |    |            |        |
 |     \  ||  ||  /         |
 |      \_oo__oo_/#######o  |
 |__________________________|

Autores:
Nicolas Arredondo Jaime
Christopher Guijosa Vaca
Leonardo Zavala Angeles

Referencias:
1-Robojax, "Using Four VL53L0X Laser Distance Meters with Arduino", https://robojax.com/tutorial_view.php?lang=en&id=301#code-section-direct





*/


//----------------------------Librerias----------------------------
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//---------------------Parametros y propiedades del robot---------------------



//---------------------GPIOs---------------------
// Pines I2C
#define i2c_scl_pin
#define i2c_sda_pin

// Salidas digitales
#define XSHUT_1
#define XSHUT_2
#define XSHUT_3
#define XSHUT_4
#define XSHUT_5
#define XSHUT_6

// Entradas digitales
#define LS_FL
#define LS_FR
#define LS_RL
#define LS_RR

// Salidas PWM
#define MTR_L
#define MTR_R

// Entradas analogicas
#define IS_L
#define IS_R




void app_main(void)
{

    //---------------------Configurar protocolos de comunicacion---------------------

    // I2C
    i2c_master_bus_handle_t i2c_bus_handle;
    i2c_master_bus_config_t i2c_bus_config = {};
    i2c_bus_config.clk_source = I2C_CLK_SOURCE_DEFAULT;
    i2c_bus_config.i2c_port = I2C_NUM_0;
    i2c_bus_config.scl_io_num = i2c_scl_pin;
    i2c_bus_config.sda_io_num = i2c_sda_pin;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internall_pullup = true;

    i2c_new_master_bus(&conf, &i2cBus);

    // PWM timer
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode       = LEDC_LOW_SPEED_MODE;
    timer_conf.duty_resolution  = LEDC_TIMER_8_BIT;
    timer_conf.timer_num        = LEDC_TIMER_0;
    timer_conf.freq_hz          = 5000;

    ledc_timer_config(&timer_conf);



    //---------------------Configurar GPIOs---------------------

    static const gpio_num_t x_shut_pins[] = { //GPIO X-SHUT
        GPIO_NUM_14,
        GPIO_NUM_15,
        GPIO_NUM_16,
        GPIO_NUM_17,
        GPIO_NUM_18,
        GPIO_NUM_21
    };
    static const adc_channel_t qtr_channels[] = { //GPIO LS
        ADC_CHANNEL_0,
        ADC_CHANNEL_1,
        ADC_CHANNEL_5,
        ADC_CHANNEL_6
    };

    typedef struct {    //Agrupación para los pines de los puente h
        gpio_num_t RPWM_pin;
        gpio_num_t LPWM_pin;
        gpio_num_t R_EN_pin;
        gpio_num_t L_EN_pin;
    } motor_pins_t;
    
    static const motor_pins_t motor_pins_BTS_1 = {    //GPIOs del puente h BTS7960#1        
        .RPWM_pin = GPIO_NUM_12,
        .LPWM_pin = GPIO_NUM_13,
        .R_EN_pin = GPIO_NUM_38,
        .L_EN_pin = GPIO_NUM_39
    };
    
    static const motor_pins_t motor_pins_BTS_2 = {    //GPIOs del puente h BTS7960#2
        .RPWM_pin = GPIO_NUM_40,
        .LPWM_pin = GPIO_NUM_41,
        .R_EN_pin = GPIO_NUM_42,
        .L_EN_pin = GPIO_NUM_47
    };
    // GPIO Encoder
    #define PIN_ENC_1 GPIO_NUM_10
    #define PIN_ENC_2 GPIO_NUM_11
    // GPIO Judge controler
    #define PIN_RDY GPIO_NUM_44
    #define PIN_GO GPIO_NUM_43


}
