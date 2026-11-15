#include "Robot/state_actions.h"
#include "Config/robot_config.h" 

VectorDirection getStateMotor(State state) {
    switch (state) {
        case IDLE:              return {0.0f, 0.0f};
        case SEARCHING:         return {SEARCH_V, SEARCH_W};
        case ENEMY_FOUND:       return {ATTACK_V, 0.0f};
        case PUSHING:           return {PUSH_V, 0.0f};
        case PUSHING_LOSING:    return {PUSH_LOSING_V, 0.0f};
        case ENEMY_LOST_L:      return {0.0f,  TURN_W};
        case ENEMY_LOST_R:      return {0.0f, -TURN_W};
        case ENEMY_BEHIND:      return {0.0f,  TURN_W};//direccion fija se puede cambiar signo
            //Linea delantera retrocede
        case LINE_FL:           return {-LINE_BACK_V, -LINE_TURN_W};
        case LINE_FR:           return {-LINE_BACK_V,  LINE_TURN_W};
            //Linea trasera avanza
        case LINE_RL:           return {LINE_FWD_V,  LINE_REAR_W};
        case LINE_RR:           return {LINE_FWD_V, -LINE_REAR_W};
        case CLOSE_TO_LINE:     return {0.0f, -1.0f};
        default:                return {0.0f, 0.0f};
    }
}
