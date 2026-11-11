#pragma once

/*
*   state_actions.h
* State action
* traduce el estado en acciones de movimiento
*/

#include "Robot/states.h"          // enum state
#include "Control/kinematics.h"    // vectordirection

VectorDirection getStateMotor(State state);