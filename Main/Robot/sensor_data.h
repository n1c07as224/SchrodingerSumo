#pragma once
/* 
can also use the following: (pragma is just more modern and standard)
#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#endif
*/
#include <cstdint> //unit16_t (tipo de 16 bits sin signo)

/*
*   sensor_data.h
* Sensor data structure
*/

struct SensorData {
    uint16_t distFront;
    uint16_t distLeft;
    uint16_t distRight;
    uint16_t distBack;
    uint16_t distFrontLeft;
    uint16_t distFrontRight;

    bool lineFL;
    bool lineFR;
    bool lineRL;
    bool lineRR;

    bool go;

    //POR SI SE REQUIERE ENCODERS
    //float rpmLeft;
    //float rpmRight;
}