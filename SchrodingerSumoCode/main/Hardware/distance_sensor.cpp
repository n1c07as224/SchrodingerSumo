#include "distance_sensors.h"

#include "Config/i2c_bus.h"
#include "driver/gpio.h"
#include <cstdint>
#include "vl53l0x.h"

using namespace DistanceS;

DS FL {.address = 0x29, .XShut = GPIO_NUM_14};
DS FC {.address = 0x30, .XShut = GPIO_NUM_15};
DS FR {.address = 0x31, .XShut = GPIO_NUM_16};
DS RL {.address = 0x32, .XShut = GPIO_NUM_17};
DS RC {.address = 0x33, .XShut = GPIO_NUM_18};
DS RR {.address = 0x34, .XShut = GPIO_NUM_21};


static DS* Sensors[6] = {&FL, &FC, &FR, &RL, &RC, &RR};



void initAll() {

    i2c_master_bus_handle_t bus_handle = i2c::getHandle(0);

    // Declarar como salida todos los XShut y apagarlos todos
    for (int i = 0; i < 6; i++) {

        gpio_set_direction(Sensors[i]->XShut, GPIO_MODE_OUTPUT);

        gpio_set_level(Sensors[i]->XShut, 0);

    }

    vTaskDelay(pdMS_TO_TICKS(2));

    // Iniciar sensores
    for (int i = 0; i < 6; i++) {

        gpio_set_level(Sensors[i]->XShut, 1);

        vTaskDelay(pdMS_TO_TICKS(2));

        vl53l0x_create(&(Sensors[i]->sensor_handle), bus_handle);
        vl53l0x_set_address(Sensors[i]->sensor_handle, bus_handle, Sensors[i]->address);
        vl53l0x_init(Sensors[i]->sensor_handle);
    
    }

    // Inicializa las lecturas continuas
    for (int i = 0; i < 6; i++) {

        vl53l0x_set_mode(Sensors[i]->sensor_handle, VL53L0X_MODE_CONTINUOUS_TIMED);
        vl53l0x_set_inter_measurement(Sensors[i]->sensor_handle, 100);
        vl53l0x_start_measurement(Sensors[i]->sensor_handle);

    }

}

// Lectura individual
uint16_t DS::read() {

    bool ready;
    vl53l0x_data_t data;

    vl53l0x_get_ready(sensor_handle, &ready);

    if (ready) {
        vl53l0x_get_data(sensor_handle, &data);
        distance = data.distance_mm;
    }

    return distance;
}


distances getAllDistances() {
    FL.read();
    FC.read();
    FR.read();
    RL.read();
    RC.read();
    RR.read();

    return {FL.distance, FC.distance, FR.distance, RL.distance, RC.distance, RR.distance};
}


