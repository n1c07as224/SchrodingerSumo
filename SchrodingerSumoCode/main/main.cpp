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
6-Pkolt, "pkolt/vl53l0x README", https://components.espressif.com/components/pkolt/vl53l0x/versions/1.0.0/readme#api
7-Brett Beauregard, "Improving the Beginner’s PID – Introduction", http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/

Cosas a tomar en cuenta:
- Los GPIO estan declarados para que el codigo sea ejecutado en un esp32s3
- Todas las medidas usan el sistema internacional de unidades
- Todas las funciones y declaraciones encargadas del control del harware se encuentran en el directorio /Hardware


*/





//------------------------------------------SETUP------------------------------------------

//----------------------------Librerias Externas----------------------------
#include <stdio.h>
#include <cstdint>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//----------------------------Librerias Propias----------------------------
#include "i2c_bus.h"
#include "pwm_timer.h"
#include "adc_unit.h"
#include "motor.h"
#include "distance_sensor.h"
#include "line_sensor.h"
#include "Datos_globales.h"




//---------------------Estados y prioridades---------------------
enum State {

    IDLE,
    SEARCHING,
    ENEMY_FOUND,
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
    {ENEMY_FOUND, 2},
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







//=======================================================MAIN-START=======================================================
extern "C" void app_main(void)
{

    //---------------------Inicializar Protocolos---------------------
    I2C::init(I2C_NUM_0, GPIO_NUM_9, GPIO_NUM_8); // Bus i2c 1 para todos los componentes i2c
    I2C::init(I2C_NUM_1, GPIO_NUM_4, GPIO_NUM_5); // Bus i2c 2 para el segundo as5600 unicamente




    //---------------------Inicializar Sensores Y Motores---------------------
    DistanceS::initAll();
    vTaskDelay(pdMS_TO_TICKS(2000));

    DistanceS::readDistances();


    printf("Distancia FL: %d\n", DS_DATA::FL_dist.load()); 
    printf("Distancia FC: %d\n", DS_DATA::FC_dist.load()); 
    printf("Distancia FR: %d\n", DS_DATA::FR_dist.load()); 
    printf("Distancia RL: %d\n", DS_DATA::RL_dist.load()); 
    printf("Distancia RC: %d\n", DS_DATA::RC_dist.load()); 
    printf("Distancia RR: %d\n", DS_DATA::RR_dist.load()); 







}
//=======================================================MAIN-END=======================================================



