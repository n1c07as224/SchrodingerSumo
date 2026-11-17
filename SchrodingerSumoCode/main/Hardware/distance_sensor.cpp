#include "distance_sensor.h"

#include "i2c_bus.h"
#include "driver/gpio.h"
#include <cstdint>
extern "C" {
    #include "vl53l0x.h"
}
#include "Datos_globales.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace DistanceS {

DS FL {.address = 0x30, .XShut = GPIO_NUM_14};
DS FC {.address = 0x31, .XShut = GPIO_NUM_15};
DS FR {.address = 0x32, .XShut = GPIO_NUM_16};
DS RL {.address = 0x33, .XShut = GPIO_NUM_17};
DS RC {.address = 0x34, .XShut = GPIO_NUM_18};
DS RR {.address = 0x35, .XShut = GPIO_NUM_21};


static DS* Sensors[6] = {&FL, &FC, &FR, &RL, &RC, &RR};



void initAll() {

    i2c_master_bus_handle_t bus_handle = I2C::getHandle(0);

    // Declarar como salida todos los XShut y apagarlos todos
    for (int i = 0; i < 6; i++) {

        gpio_set_direction(Sensors[i]->XShut, GPIO_MODE_OUTPUT);

        gpio_set_level(Sensors[i]->XShut, 0);

    }

    vTaskDelay(pdMS_TO_TICKS(10));

    // Iniciar sensores
    for (int i = 0; i < 6; i++) {

        gpio_set_level(Sensors[i]->XShut, 1);

        vTaskDelay(pdMS_TO_TICKS(10));

        esp_err_t err = vl53l0x_create(&(Sensors[i]->sensor_handle), bus_handle);
        if (err != ESP_OK) {
            printf("Error creando handle para sensor %d\n", i);
            continue;
        }
        else {
            printf("Sensor creado \n");
        }

        err = vl53l0x_init(Sensors[i]->sensor_handle); 
        if (err != ESP_OK) {
            printf("Error iniciando sensor %d\n", i);
            continue;
        }
        else {
            printf("Sensor iniciado \n");
        }

        err = vl53l0x_set_address(Sensors[i]->sensor_handle, bus_handle, Sensors[i]->address);
        if (err != ESP_OK) {
            printf("Error cambiando direccion %d\n", i);
            continue;
        }
        else {
            printf("Direccion cambiada \n");
        }

        err = vl53l0x_set_mode(Sensors[i]->sensor_handle, VL53L0X_MODE_CONTINUOUS_TIMED);
        if (err != ESP_OK) {
            printf("Error en modo continuo %d\n", i);
            continue;
        }
        else {
            printf("modo continuo activado \n");
        }
    

        err = vl53l0x_set_inter_measurement(Sensors[i]->sensor_handle, 100);
        if (err != ESP_OK) {
            printf("Error en set inter %d\n", i);
            continue;
        }
        else {
            printf("Set inter correcto \n");
        }

    }

    // Inicializa las lecturas continuas
    for (int i = 0; i < 6; i++) {


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
    else {
        printf("dato no listo");
    }

    printf("%d\n", data.distance_mm);

    return distance;
}


void readDistances() {
    FL.read();
    FC.read();
    FR.read();
    RL.read();
    RC.read();
    RR.read();

    DS_DATA::FL_dist.store(FL.distance);
    DS_DATA::FC_dist.store(FC.distance);
    DS_DATA::FR_dist.store(FR.distance);
    DS_DATA::RL_dist.store(RL.distance);
    DS_DATA::RC_dist.store(RC.distance);
    DS_DATA::RR_dist.store(RR.distance);
}

}

