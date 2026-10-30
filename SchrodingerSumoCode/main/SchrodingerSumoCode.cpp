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

Referencias:
1-Robojax, "Using Four VL53L0X Laser Distance Meters with Arduino", https://robojax.com/tutorial_view.php?lang=en&id=301#code-section-direct


Cosas a tomar en cuenta:
- Los GPIO estan declarados para que el codigo sea ejecutado en un esp32s3
- Todas las medidas usan el sistema internacional de unidades


*/


//----------------------------Librerias----------------------------
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//---------------------Propiedades del robot---------------------
// Medidas fisicas
constexpr uint8_t r_llantas = 30;

// Constantes PID
constexpr float Kp;
constexpr float Ki;
constexpr float Kd;

// Lectura de sensores
constexpr uint_t dDistance = 150;
constexpr uint_t dDistance_fast = 50;
constexpr uint_t dDistance = 150;
constexpr uint_t dDistance = 150;

//---------------------GPIOs---------------------
// Pines I2C
constexpr uint8_t i2c_sda_pin  = 8;
constexpr uint8_t i2c_scl_pin  = 9;
constexpr uint8_t i2c_sda2_pin = 4;
constexpr uint8_t i2c_scl2_pin = 5;

// Salidas digitales
constexpr uint8_t XSHUT_1 = 14;
constexpr uint8_t XSHUT_2 = 15;
constexpr uint8_t XSHUT_3 = 16;
constexpr uint8_t XSHUT_4 = 17;
constexpr uint8_t XSHUT_5 = 18;
constexpr uint8_t XSHUT_6 = 21;

// Entradas digitales
constexpr uint8_t LS_FL = 1;
constexpr uint8_t LS_FR = 2;
constexpr uint8_t LS_RL = 6;
constexpr uint8_t LS_RR = 7;

// Salidas PWM
constexpr uint8_t MTR_L_PIN = 0;
constexpr uint8_t MTR_R_PIN = 0;

// Canales PWM
constexpr ledc_channel_t MTR_L = LEDC_CHANNEL_0;
constexpr ledc_channel_t MTR_R = LEDC_CHANNEL_1;

// Entradas analógicas
constexpr uint8_t IS_L = 0;
constexpr uint8_t IS_R = 0;



//---------------------Estados---------------------
struct FlagsState {

    bool waiting = true;
    uint8_t searching = 0; // 0= no buscar, 1= buscando, 2= buscar en la izquierda, 3= buscar en la derecha
    bool enemy_found = false;
    uint8_t pushing = 0; // 0= no empujando, 1= empujando, 2= empujando y perdiendo
    uint8_t line = 0; //0= sin linea, 1= linea trasera izquierda, 2= linea trasera derecha, 3= linea frontal izquierda, 4= linea frontal derecha, 5= sensor trasero detecta linea proxima

}






//========================================MAIN-START========================================
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
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);



    //---------------------Configurar GPIOs---------------------
    // Salidas digitales
    gpio_set_direction(XSHUT_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(XSHUT_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(XSHUT_3, GPIO_MODE_OUTPUT);
    gpio_set_direction(XSHUT_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(XSHUT_5, GPIO_MODE_OUTPUT);
    gpio_set_direction(XSHUT_6, GPIO_MODE_OUTPUT);

    // Entradas digitales
    gpio_set_direction(LS_FL, GPIO_MODE_INPUT);
    gpio_set_direction(LS_FR, GPIO_MODE_INPUT);
    gpio_set_direction(LS_RL, GPIO_MODE_INPUT);
    gpio_set_direction(LS_RR, GPIO_MODE_INPUT);

    // Salidas PWM
    ledc_channel_config_t MTR_L_Config = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MTR_L_PIN,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&MTR_L_Config);

    ledc_channel_config_t MTR_R_Config = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MTR_R_PIN,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&MTR_R_Config);


}
//========================================MAIN-END========================================




//---------------------Funciones---------------------



