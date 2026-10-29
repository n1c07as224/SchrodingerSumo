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
// QTR-1A
#define QTR_OUT1    GPIO_NUM_1
#define QTR_OUT2    GPIO_NUM_2
#define QTR_OUT3    GPIO_NUM_6
#define QTR_OUT4    GPIO_NUM_7

// MPU6050, VL53L0X, AS5600 
#define i2c_sda_pin GPIO_NUM_8
#define i2c_scl_pin GPIO_NUM_9

// VL53L0X
#define XSHUT_1     GPIO_NUM_14
#define XSHUT_2     GPIO_NUM_15
#define XSHUT_3     GPIO_NUM_16
#define XSHUT_4     GPIO_NUM_17
#define XSHUT_5     GPIO_NUM_18
#define XSHUT_6     GPIO_NUM_21

// AS5600
#define AS5600_OUT1 GPIO_NUM_10
#define AS5600_OUT2 GPIO_NUM_11
#define AS5600_SDA2 GPIO_NUM_4    
#define AS5600_SCL2 GPIO_NUM_5

// BTS7960 #1
#define BTS1_RPWM   GPIO_NUM_12
#define BTS1_LPWM   GPIO_NUM_13
#define BTS1_R_EN   GPIO_NUM_38
#define BTS1_L_EN   GPIO_NUM_39

// BTS7960 #2
#define BTS2_RPWM   GPIO_NUM_40
#define BTS2_LPWM   GPIO_NUM_41
#define BTS2_R_EN   GPIO_NUM_42
#define BTS2_L_EN   GPIO_NUM_47

// ModR01
#define ModR01_RDY  GPIO_NUM_45
#define ModR01_GO   GPIO_NUM_48


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
    i2c_bus_config.flags.enable_internal_pullup = true;

    i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);

    // PWM timer
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode       = LEDC_LOW_SPEED_MODE;
    timer_conf.duty_resolution  = LEDC_TIMER_8_BIT;
    timer_conf.timer_num        = LEDC_TIMER_0;
    timer_conf.freq_hz          = 5000;

    ledc_timer_config(&timer_conf);



    //---------------------Configurar GPIOs---------------------


}