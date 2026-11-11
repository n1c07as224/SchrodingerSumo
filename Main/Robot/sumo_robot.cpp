#include "Robot/sumo_robot.h"
#include "Config/robot_config.h"

//look inside X being Sumorobot for Y which is SumoRobot(). and it has a : as a simple fast version of {} and =
// being which items e.g. CurrentState, into whatever is in () being IDLE
//E.g.
/* 
    SumoRobot::SumoRobot() {
        currentState = IDLE;
        previousState = IDLE;
        beforePreviousState = IDLE;
        stateStartTime = 0;
    }
*/
SumoRobot::SumoRobot() : currentState(IDLE), previousState(IDLE), beforePreviousState(IDLE), stateStartTime(0) {
    // Constructor implementation
}

int SumoRobot::getPriority(State state) const {
    switch (state) {
        case IDLE:
            return 0;
        case SEARCHING:
            return 1;
        case ENEMY_FOUND:
            return 2;
        case ENEMY_LOST_L:
        case ENEMY_LOST_R:
            return 3;
        case PUSHING:
            return 4;
        case PUSHING_LOSING:
            return 5;
        case LINE_FL:
        case LINE_FR:
        case LINE_RL:
        case LINE_RR:
            return 6;
        case CLOSE_TO_LINE:
            return 7;
        default:
            return -1; // Invalid state
    }
}

void SumoRobot::update(const SensorData& sensorData, uint32_t now_ms) {
    checkTimeout(now_ms);
    State nextState = decideNextState(sensorData);
    if (nextState != currentState) {
        changeState(nextState, now_ms);
    }
}

void SumoRobot ::changeState(State newState, uint32_t now_ms) {
    if (newState != currentState) {
        beforePreviousState = previousState;
        previousState = currentState;
        currentState = newState;
        stateStartTime = now_ms;
    }
}

void SumoRobot::forceState(State newState, uint32_t now_ms) {
    beforePreviousState = previousState;
    previousState = currentState;
    currentState = newState;
    stateStartTime = now_ms;
}

void SumoRobot::checkTimeout(uint32_t now_ms) {
    uint32_t elapsedTime = now_ms - stateStartTime;
    switch (currentState) {
        case ENEMY_LOST_L:
        case ENEMY_LOST_R:
            if (elapsedTime > TIMEOUT_ENEMY_LOST) {
                changeState(SEARCHING, now_ms);
            }
            break;
        case PUSHING_LOSING:
            if (elapsedTime > TIMEOUT_PUSHING_LOSING) {
                changeState(SEARCHING, now_ms);
            }
            break;
        default:
            break;
    }
}

void SumoRobot::forceState(State newState, uint32_t now_ms) {
    beforePreviousState = previousState;
    previousState = currentState;
    currentState = newState;
    stateStartTime = now_ms;
}

State SumoRobot::getCurrentState() const {
    return currentState;
}
