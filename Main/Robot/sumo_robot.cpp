#include "Robot/sumo_robot.h"
#include "Config/robot_config.h"

//This file does not need public private, the header already has them
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

    for (int i = 0; i < COUNT; ++i) {
        if (robotStatePriority[i].state_num == state) {
            return robotStatePriority[i].state_priority;
        }
    }
    return -1;
}

void SumoRobot::update(const SensorData& sensorData, uint32_t now_ms) {

    if(!sensorData.go) {
        forceState(IDLE, now_ms);
        return;
    }

    if (currentState == IDLE) {
        changeState(SEARCHING, now_ms);
    }
    
    checkTimeout(now_ms);
    State nextState = decideNextState(sensorData);
    
}

void SumoRobot ::changeState(State newState, uint32_t now_ms) {

    if (newState == currentState) {
        return; // No change needed
    }

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
