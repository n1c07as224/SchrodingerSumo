#include "Control/kinematics.h"
#include <cmath>

WheelRPM calculateWheelRPM(VectorDirection vel, float wheelRadius, float wheelBase) {
    // m/s a RPM 
    const float k = 60.0f / (2.0f * (float)M_PI * wheelRadius);

    WheelRPM out;
    out.left  = (vel.v - vel.w * wheelBase * 0.5f) * k;
    out.right = (vel.v + vel.w * wheelBase * 0.5f) * k;

     float maxAbs = fmaxf(fabsf(out.left), fabsf(out.right));
    if (maxAbs > RPM_LIMIT_CMD) {
        float s = RPM_LIMIT_CMD / maxAbs;
        out.left  *= s;
        out.right *= s;
    }
    return out;
}

VectorDirection rampVector(VectorDirection current, VectorDirection target, float dt) {
    current.v += clampf(target.v - current.v, -V_ACCEL_MAX * dt, V_ACCEL_MAX * dt);
    current.w += clampf(target.w - current.w, -W_ACCEL_MAX * dt, W_ACCEL_MAX * dt);
    return current;
}

float clampf(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}