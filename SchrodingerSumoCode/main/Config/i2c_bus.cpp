#include "i2c.h"

namespace I2CBus {

    static i2c_master_bus_handle_t bus_handles[2] = {nullptr, nullptr};

    void init(i2c_port_num_t port, int sda_pin, int scl_pin) {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = PORT,
            .sda_io_num = (gpio_num_t)SDA_PIN,
            .scl_io_num = (gpio_num_t)SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags = { .enable_internal_pullup = true }
        };
        i2c_new_master_bus(&bus_config, &bus_handles[PORT]);
    }

    i2c_master_bus_handle_t getHandle(i2c_port_num_t PORT) {
        return bus_handles[PORT];
    }

}
