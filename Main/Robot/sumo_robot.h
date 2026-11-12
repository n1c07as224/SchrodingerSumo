#pragma once

#include "Robot/sensor_data.h"     // struct SensorData
#include "Robot/states.h"          // enum state
#include <cstdint> //unit16_t (tipo de 16 bits sin signo)
//Note, it does not need to spefici if it is public or private, because of the header in both cpp and h file, just the h
/* Sumo robot class 
* cerebro del robot, contiene los estados y la logica de control
*/

class SumoRobot {
    
public:
    SumoRobot();
    //   el sensor data. con el & sacas una referencia al objeto, para no copiarlo y ahorrar memoria. el nombre es arbitrario
    void update(const SensorData& sensorData, uint32_t now_ms); //actualizar el estado del robot basado en los datos de los sensores
    
    void changeState(State newState, uint32_t now_ms); //cambiar el estado del robot y registrar el tiempo de inicio del estado

    void forceState(State newState, uint32_t now_ms); // Forzar un estado específico

    State getCurrentState() const;

private:
    int getPriority(State state) const;    //get priority 

    State decideNextState(const SensorData& sensorData); //decidir el siguiente estado basado en los datos de los sensores

    void checkTimeout(uint32_t now_ms); //verificar si se ha excedido el tiempo de espera para un estado

    State currentState;
    State previousState;
    State beforePreviousState;
    uint32_t stateStartTime; // Tiempo en milisegundos cuando se ingresó

};

