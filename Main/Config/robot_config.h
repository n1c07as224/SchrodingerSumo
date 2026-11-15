#pragma once

#include <cstdint>

/*
*   robot_config.h
*   Constantes de ajuste de la estrategia del robot.
*   Aqui se cambian las constantes
*/

//distancia


constexpr uint16_t ENEMY_THRESHOLD = 400;
constexpr uint16_t BACK_SENSOR_DIFF = 200;
//menor a este valor es igual a empuja
constexpr uint16_t PUSH_THRESHOLD  = 150;

// tiempos
constexpr uint32_t LINE_TURN_TIME_MS        = 300;//cuanto dura line
constexpr uint32_t ENEMY_LOST_TIME_MS       = 500;//cuanto dura enemy lost
constexpr uint32_t PUSHING_LOSING_TIME_MS   = 400;//cuanto dura Pushing Losing
constexpr uint32_t ENEMY_BEHIND_TIME_MS     = 400;//cuanto tiempo dura enemy behind 

//velocidades con constantes
constexpr float SEARCH_V       = 0.25f;
constexpr float SEARCH_W       = 5.0f;//arco para barrer el dohyo
constexpr float TURN_W         = 10.0f;//giro rapido en sitio
constexpr float ATTACK_V       = 0.9f;
constexpr float PUSH_V         = 1.2f;
constexpr float PUSH_LOSING_V  = 1.3f;
constexpr float LINE_BACK_V    = 0.7f;//salida al ver linea delantera
constexpr float LINE_TURN_W    = 6.0f;
constexpr float LINE_FWD_V     = 0.8f;//salida al ver linea trasera
constexpr float LINE_REAR_W    = 3.0f;