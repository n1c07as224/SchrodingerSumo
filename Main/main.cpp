#include "Robot/sumo_robot.h"
#include "Robot/state_actions.h"
#include "Control/kinematics.h" 
#include "Config/robot_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

static SensorData readSensors(){
    SensorData s{};  //vale 0 se llama s
    s.go = true; //se puede usar para probar----------------------------
    return s;
}

extern "C" void app_main(void) {
    SumoRobot robot;
    VectorDirection cmd{0.0f, 0.0f};
    TickType_t lastWake = xTaskGetTickCount();  // para poder poner un ritmo
    //TickType_t Tipo de dato, xTaskGetTickCount numero total de ticks, configurable
    State lastLogged = COUNT;

    while(true){

        uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000);

        robot.update(readSensors(), now_ms);
        State st = robot.getCurrentState();

        VectorDirection targetDirection = getStateMotor(st);

        if(st == IDLE || st == LINE_FL || st == LINE_FR || st == LINE_RL || st == LINE_RR){
            cmd = targetDirection;
        }else{
            cmd = rampVector(cmd, targetDirection, 0.01f);
        }
        WheelRPM rpm = calculateWheelRPM(cmd, WHEEL_RADIUS, WHEEL_BASE);

        if (st != lastLogged) { ESP_LOGI("sumo", "estado=%d", (int)st); lastLogged = st; } //imprime al cambio
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(10)); //espera a tener una ventana, espacios de 10ms, 10ms, 20ms, etc
    }
}