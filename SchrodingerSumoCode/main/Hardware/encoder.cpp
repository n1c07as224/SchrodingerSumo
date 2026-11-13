
#include "as5600.h"
#include "i2c_bus.h"
#include "../Datos_globales.h"


static as5600_handle_t as5600_handle_1 = nullptr;
static as5600_handle_t as5600_handle_2 = nullptr;

static uint16_t last_radians_1;
static uint16_t last_radians_2;

static uint64_t previous_time;

namespace Encoder {

void initAll() {

    i2c_master_bus_handle_t bus_handle_1 = i2c::getHandle(0);
    i2c_master_bus_handle_t bus_handle_2 = i2c::getHandle(1);


    as5600_i2c_config_t as5600_config = {
        .scl_speed_hz = 100000
    };

    ESP_ERROR_CHECK(as5600_new_sensor(bus_handle_1, &as5600_config, &as5600_handle_1));
    ESP_ERROR_CHECK(as5600_new_sensor(bus_handle_2, &as5600_config, &as5600_handle_2));

}



void readRPM() {

    uint64_t current_time = esp_timer_get_time();
    float dTime = (current_time - previous_time) / 1000000.0f;

    if (dTime <= 0) return;

    float actual_radians_1;
    float actual_radians_2;

    ESP_ERROR_CHECK(as5600_get_angle_radians(handle_1, &actual_radians_1));
    ESP_ERROR_CHECK(as5600_get_angle_radians(handle_2, &actual_radians_2));

    float dRadians_1 = actual_radians_1 - last_radians_1;
    float dRadians_2 = actual_radians_2 - last_radians_2;

    // Para cuando salta de 360 a 0 grados
    if (dRadians_1 > M_PI) dRadians_1 -= 2.0f * M_PI;
    if (dRadians_1 < -M_PI) dRadians_1 += 2.0f * M_PI;

    if (dRadians_2 > M_PI) dRadians_2 -= 2.0f * M_PI;
    if (dRadians_2 < -M_PI) dRadians_2 += 2.0f * M_PI;

    float rpm_1 = (dRadians_1 / dTime) * 9.549296f;
    float rpm_2 = (dRadians_2 / dTime) * 9.549296f;

    MTR_DATA::L_rpm.store(rpm_1);
    MTR_DATA::R_rpm.store(rpm_2);

    last_radians_1 = actual_radians_1;
    last_radians_2 = actual_radians_2;

    previous_time = current_time;
}

}


