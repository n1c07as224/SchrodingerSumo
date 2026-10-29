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


}
