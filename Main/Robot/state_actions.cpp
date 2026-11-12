#include "Robot/state_actions.h"

VectorDirection getStateMotor(State state) {
    switch (state) {
        case IDLE:
            return VectorDirection{0.0f, 0.0f};
        case SEARCHING:
            return VectorDirection{0.0f, 1.0f};
        case ENEMY_FOUND:
            return VectorDirection{0.0f, 1.0f};
        case ENEMY_LOST_L:
            return VectorDirection{-1.0f, 1.0f};
        case ENEMY_LOST_R:
            return VectorDirection{1.0f, 1.0f};
        case PUSHING:
            return VectorDirection{0.0f, 1.0f};
        case PUSHING_LOSING:
            return VectorDirection{0.0f, 1.0f};
        case LINE_FL:
            return VectorDirection{-1.0f, -1.0f};
        case LINE_FR:
            return VectorDirection{1.0f, -1.0f};
        case LINE_RL:
            return VectorDirection{-1.0f, -1.0f};
        case LINE_RR:
            return VectorDirection{1.0f, -1.0f};
        case CLOSE_TO_LINE:
            return VectorDirection{0.0f, -1.0f};
        default:
            return VectorDirection{0.0f, 0.0f}; // Default to no movement
    }
}