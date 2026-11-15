#pragma once

/*
*   states.h
* State 
*/

enum State {

    IDLE,
    SEARCHING,
    ENEMY_FOUND,
    ENEMY_LOST_L,
    ENEMY_LOST_R,
    ENEMY_BEHIND,
    PUSHING,
    PUSHING_LOSING,
    LINE_FL,
    LINE_FR,
    LINE_RL,
    LINE_RR,
    CLOSE_TO_LINE,
    COUNT               //enumeration trick

};

//Like a table of contents, but for states, to get the priority of each state,file with two cells
struct StateConfig {
    int state_num;
    int state_priority;
};

//priority of each state, higher number means higher priority. The numbers are different, IDLE gives tons of problems if at 5
constexpr StateConfig robotStatePriority[COUNT] {
    {IDLE, 0},
    {SEARCHING, 1},
    {CLOSE_TO_LINE, 1},
    {ENEMY_FOUND, 2},
    {ENEMY_LOST_L, 2},
    {ENEMY_LOST_R, 2},
    {ENEMY_BEHIND, 2},    
    {PUSHING_LOSING, 2},    
    {PUSHING, 3},
    {LINE_FL, 4},
    {LINE_FR, 4},
    {LINE_RL, 4},
    {LINE_RR, 4}
};
