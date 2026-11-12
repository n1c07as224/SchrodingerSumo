#pragma once

#include <cstdint>

/*
*   robot_config.h
*   Constantes de ajuste de la estrategia del robot.
*   Aqui se cambian las constantes
*/

// distancia

// Menor a este valor = se ve al enemigo
constexpr uint16_t ENEMY_THRESHOLD = 400;
// Menor a este valor es igual a empuja
constexpr uint16_t PUSH_THRESHOLD  = 150;

// tiempos
constexpr uint32_t LINE_TURN_TIME_MS      = 300;   //cuanto dura line
constexpr uint32_t ENEMY_LOST_TIME_MS     = 500;   // cuanto dura enemy lost
constexpr uint32_t PUSHING_LOSING_TIME_MS = 400;  //cuanto dura Pushing Losing