#pragma once

/*
*   kinematics.h
* vectores
* 
*/

struct VectorDirection {
    float v;   // velocidad hacia adelante (m/s)
    float w;   // giro (rad/s)
};

// RPM objetivo de cada rueda
struct WheelRPM {
    float left;
    float right;
};

//Constantes
constexpr float WHEEL_RADIUS   = 0.030f;   // m
constexpr float WHEEL_BASE     = 0.180f;   // m
constexpr float MAX_WHEEL_RPM  = 450.0f;
constexpr float RPM_LIMIT_CMD  = MAX_WHEEL_RPM * 0.9f;
constexpr float V_ACCEL_MAX    = 4.0f;     // m/s^2
constexpr float W_ACCEL_MAX    = 30.0f;    // rad/s^2

inline float clampf(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}
WheelRPM calculateWheelRPM(VectorDirection vel, float wheelRadius, float wheelBase);

VectorDirection rampVector(VectorDirection current, VectorDirection target, float dt);
