#pragma once

#include "driver/i2c_master.h"

namespace I2C {

    void init(i2c_port_num_t PORT, gpio_num_t SDA_PIN, gpio_num_t SCL_PIN);

    i2c_master_bus_handle_t getHandle(i2c_port_num_t PORT);

}

