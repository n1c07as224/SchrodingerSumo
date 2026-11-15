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

static bool see(uint16_t dist, uint16_t threshold) {
    return dist > 0 && dist < threshold;
}

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

    if (nextState == SEARCHING) {
    
        if (currentState == ENEMY_FOUND || currentState == PUSHING) forceState(SEARCHING, now_ms);

    } 
    
    else {
    changeState(nextState, now_ms);
    }
    
}

void SumoRobot::changeState(State newState, uint32_t now_ms) {

    if (newState == currentState) return;

    if (getPriority(newState) < getPriority(currentState)) return;  

    beforePreviousState = previousState;
    previousState = currentState;
    currentState = newState;
    stateStartTime = now_ms;

}

void SumoRobot::forceState(State newState, uint32_t now_ms) {

    beforePreviousState = previousState;
    previousState = currentState;
    currentState = newState;
    stateStartTime = now_ms;

}

void SumoRobot::checkTimeout(uint32_t now_ms) {

    uint32_t elapsedTime = now_ms - stateStartTime; //ellapsed time it is just final time - start time, the difference or so

    switch (currentState) {

        case LINE_FL:
        case LINE_FR:
        case LINE_RL:
        case LINE_RR:
            if (elapsedTime > LINE_TURN_TIME_MS) {
                forceState(SEARCHING, now_ms);
            }
            break;
        
        case ENEMY_LOST_L:
        case ENEMY_LOST_R:
            if (elapsedTime > ENEMY_LOST_TIME_MS) {
                forceState(SEARCHING, now_ms);
            }
            break;
        
        case PUSHING_LOSING:
            if (elapsedTime > PUSHING_LOSING_TIME_MS) {
                forceState(SEARCHING, now_ms);
            }
            break;
        case ENEMY_BEHIND:
            if (elapsedTime > ENEMY_BEHIND_TIME_MS){
                forceState(SEARCHING, now_ms);
            }
        default:
            break;
    }
}


State SumoRobot::getCurrentState() const {

    return currentState;

}


State SumoRobot::decideNextState(const SensorData& s) {

    if (s.lineFL) return LINE_FL;
    if (s.lineFR) return LINE_FR;
    if (s.lineRL) return LINE_RL;
    if (s.lineRR) return LINE_RR;

    if (see(s.distFront, PUSH_THRESHOLD)) return PUSHING;

    if (see(s.distFront, ENEMY_THRESHOLD) ||
        see(s.distFrontLeft, ENEMY_THRESHOLD) ||
        see(s.distFrontRight, ENEMY_THRESHOLD)) return ENEMY_FOUND;

    if (see(s.distLeft, ENEMY_THRESHOLD))  return ENEMY_LOST_L;
    if (see(s.distRight, ENEMY_THRESHOLD)) return ENEMY_LOST_R;
    if (see(s.distBack, ENEMY_THRESHOLD)) return ENEMY_BEHIND;
    //if (see(s.distBack, BACK_SENSOR_DIFF))
    return SEARCHING;
}