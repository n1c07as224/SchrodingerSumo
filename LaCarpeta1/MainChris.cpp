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
2-GeeksforGeeks, "Understanding constexpr Specifier in C++", https://www.geeksforgeeks.org/cpp/understanding-constexper-specifier-in-cpp/
3-Descubrearduino, "Cómo hacer un PID con Arduino", https://descubrearduino.com/como-hacer-un-pid-con-arduino/
4-Del Código a la Arquitectura, "Máquinas de Estado: El patrón para eliminar los "If" anidados", https://youtu.be/sTyRxey86Jw?si=h-pryMeM7pt8JGrz 
5-W3schools, "C++ Enumeration (enum)", https://www.w3schools.com/cpp/cpp_enum.asp
6-Bro Code, "What is a switch?", https://www.youtube.com/watch?v=Bx9b12FCF5o



Cosas a tomar en cuenta:
- Los GPIO estan declarados para que el codigo sea ejecutado en un esp32s3
- Todas las medidas usan el sistema internacional de unidades


*/





//------------------------------------------SETUP------------------------------------------

//----------------------------Librerias----------------------------
#include <stdio.h>
#include <cstdint>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "iostream"



//---------------------Propiedades del robot---------------------
// Medidas fisicas del robot
constexpr uint8_t r_llantas = 30;

// Constantes y variables matematicas
constexpr float Kp = 0.0f;
constexpr float Ki = 0.0f;
constexpr float Kd = 0.0f;

float previous_time = 0.0f;
float current_time = 0.0f;
float last_error = 0.0f;
float integ_error = 0.0f;

// Frecuencia en la lectura de sensores
constexpr uint16_t dDS = 150;
constexpr uint16_t dDS_fast = 50;
constexpr uint16_t dIMU = 150;
constexpr uint16_t dIS = 150;

//Variables Time
constexpr uint16_t loop_time = 10; //ms
int64_t now = esp_timer_get_time() / 1000; 


//---------------------Estados y prioridades---------------------
enum State {

    IDLE,
    SEARCHING,
    ENEMY_FOUNDED,
    ENEMY_LOST_L,
    ENEMY_LOST_R,
    PUSHING,
    PUSHING_LOSING,
    LINE_FL,
    LINE_FR,
    LINE_RL,
    LINE_RR,
    CLOSE_TO_LINE,
    COUNT

};

// Prioridad que tiene cada estado
struct StateConfig {

    int state_num;
    int state_priority;

};

constexpr StateConfig robotStatePriority[COUNT] {

    {IDLE, 5},
    {SEARCHING, 1},
    {ENEMY_FOUNDED, 2},
    {ENEMY_LOST_L, 2},
    {ENEMY_LOST_R, 2},
    {PUSHING, 2},
    {PUSHING_LOSING, 3},
    {LINE_FL, 4},
    {LINE_FR, 4},
    {LINE_RL, 4},
    {LINE_RR, 4},
    {CLOSE_TO_LINE, 1}

};

// Historial de estados
State robotState = IDLE;
State lastRobotState = IDLE;
State beforeLastRobotState = IDLE;




//---------------------GPIOs---------------------
// Pines I2C
constexpr gpio_num_t i2c_sda_pin  = GPIO_NUM_8;
constexpr gpio_num_t i2c_scl_pin  = GPIO_NUM_9;
constexpr gpio_num_t i2c_sda2_pin = GPIO_NUM_4;
constexpr gpio_num_t i2c_scl2_pin = GPIO_NUM_5;

// Salidas digitales
constexpr gpio_num_t XSHUT_1 = GPIO_NUM_14;
constexpr gpio_num_t XSHUT_2 = GPIO_NUM_15;
constexpr gpio_num_t XSHUT_3 = GPIO_NUM_16;
constexpr gpio_num_t XSHUT_4 = GPIO_NUM_17;
constexpr gpio_num_t XSHUT_5 = GPIO_NUM_18;
constexpr gpio_num_t XSHUT_6 = GPIO_NUM_21;

// Entradas digitales
constexpr gpio_num_t LS_FL = GPIO_NUM_1;
constexpr gpio_num_t LS_FR = GPIO_NUM_2;
constexpr gpio_num_t LS_RL = GPIO_NUM_6;
constexpr gpio_num_t LS_RR = GPIO_NUM_7;

// Salidas PWM
constexpr gpio_num_t MTR1_L_PIN = GPIO_NUM_12;
constexpr gpio_num_t MTR1_R_PIN = GPIO_NUM_13;
constexpr gpio_num_t MTR2_L_PIN = GPIO_NUM_40;
constexpr gpio_num_t MTR2_R_PIN = GPIO_NUM_41;

// Canales PWM
constexpr ledc_channel_t MTR1_L = LEDC_CHANNEL_0;
constexpr ledc_channel_t MTR1_R = LEDC_CHANNEL_1;
constexpr ledc_channel_t MTR2_L = LEDC_CHANNEL_2;
constexpr ledc_channel_t MTR2_R = LEDC_CHANNEL_3;

// Entradas ADC
constexpr gpio_num_t IS1_L_PIN = GPIO_NUM_3;
constexpr gpio_num_t IS1_R_PIN = GPIO_NUM_10;
constexpr gpio_num_t IS2_L_PIN = GPIO_NUM_19;
constexpr gpio_num_t IS2_R_PIN = GPIO_NUM_20;

// Canales ADC
static adc_oneshot_unit_handle_t adc1_handle = nullptr;
constexpr adc_channel_t IS1_L = ADC_CHANNEL_0;
constexpr adc_channel_t IS1_R = ADC_CHANNEL_1;
constexpr adc_channel_t IS2_L = ADC_CHANNEL_2;
constexpr adc_channel_t IS2_R = ADC_CHANNEL_3;




//---------------------Inicializar funciones---------------------
void init_gpio_adc_pwm();









//=======================================================MAIN-START=======================================================
extern "C" void app_main(void)
{

    //---------------------Configurar protocolos de comunicacion---------------------

    // I2C
    i2c_master_bus_handle_t i2c_bus_handle;
    i2c_master_bus_config_t i2c_bus_config = {};
    i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_config.i2c_port = I2C_NUM_0;
    i2c_bus_config.scl_io_num = i2c_scl_pin;
    i2c_bus_config.sda_io_num = i2c_sda_pin;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internal_pullup = true;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));

    // PWM timer
    ledc_timer_config_t timer_config{};
    timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_config.timer_num = LEDC_TIMER_0;
    timer_config.duty_resolution = LEDC_TIMER_8_BIT;
    timer_config.freq_hz = 5000;
    timer_config.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    // ADC
    adc_oneshot_unit_init_cfg_t adc_init_config = {};
    adc_init_config.unit_id = ADC_UNIT_1;
    adc_init_config.clk_src = ADC_RTC_CLK_SRC_DEFAULT;
    adc_init_config.ulp_mode = ADC_ULP_MODE_DISABLE;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_config, &adc1_handle));


    //---------------------Inicializar sensores y motores---------------------
    init_gpio_adc_pwm();








}
//=======================================================MAIN-END=======================================================







//------------------------------------------FUNCTIONS------------------------------------------

//---------------------Configurar GPIOs---------------------
void init_gpio_adc_pwm() {

    // Salidas digitales
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_1, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_2, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_3, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_4, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_5, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_6, GPIO_MODE_OUTPUT));

    // Entradas digitales
    ESP_ERROR_CHECK(gpio_set_direction(LS_FL, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LS_FR, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LS_RL, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LS_RR, GPIO_MODE_INPUT));

    // Salidas PWM
    ledc_channel_config_t MTR1_L_Config{};
    MTR1_L_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR1_L_Config.channel = MTR1_L;
    MTR1_L_Config.timer_sel = LEDC_TIMER_0;
    MTR1_L_Config.gpio_num = MTR1_L_PIN;
    MTR1_L_Config.duty = 0;
    MTR1_L_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR1_L_Config));


    ledc_channel_config_t MTR1_R_Config{};
    MTR1_R_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR1_R_Config.channel = MTR1_R;
    MTR1_R_Config.timer_sel = LEDC_TIMER_0;
    MTR1_R_Config.gpio_num = MTR1_R_PIN;
    MTR1_R_Config.duty = 0;
    MTR1_R_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR1_R_Config));


    ledc_channel_config_t MTR2_L_Config{};
    MTR2_L_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR2_L_Config.channel = MTR2_L;
    MTR2_L_Config.timer_sel = LEDC_TIMER_0;
    MTR2_L_Config.gpio_num = MTR2_L_PIN;
    MTR2_L_Config.duty = 0;
    MTR2_L_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR2_L_Config));


    ledc_channel_config_t MTR2_R_Config{};
    MTR2_R_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR2_R_Config.channel = MTR2_R;
    MTR2_R_Config.timer_sel = LEDC_TIMER_0;
    MTR2_R_Config.gpio_num = MTR2_R_PIN;
    MTR2_R_Config.duty = 0;
    MTR2_R_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR2_R_Config));

    // Entradas ADC
    adc_oneshot_chan_cfg_t adc_channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS1_L, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS1_R, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS2_L, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS2_R, &adc_channel_config));

}

void init_distance_sensors() {



}

void updateState(){

}

void executeState(State){
    /*IDLE,
    SEARCHING,
    ENEMY_FOUNDED,
    ENEMY_LOST_L,
    ENEMY_LOST_R,
    PUSHING,
    PUSHING_LOSING,
    LINE_FL,
    LINE_FR,
    LINE_RL,
    LINE_RR,
    CLOSE_TO_LINE,
    COUNT*/

    switch (State)
    {
    case  expression:
        /* code */
        break;
    
    default:
        break;
    }

}

float PID_calc(int set_point, int rpm_read) {

    float error = set_point - rpm_read;
    float dTime = current_time - previous_time;

    // Integral
    integ_error += error * dTime;

    // Derivada
    float deriv_error = (error - last_error) / dTime;

    float result = Kp * error + Ki * integ_error + Kd * deriv_error;

    last_error = error;
    previous_time = current_time;

    return result;
}

while (1){
    updateState();
}