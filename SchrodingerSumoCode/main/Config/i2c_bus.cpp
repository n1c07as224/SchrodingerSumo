#include "driver/i2c_master.h"
#include "i2c_bus.h"

namespace I2C {

    static i2c_master_bus_handle_t bus_handles[2] = {nullptr, nullptr};

    void init(i2c_port_num_t PORT, int SDA_PIN, int SCL_PIN) {
        i2c_master_bus_config_t bus_config{};

        bus_config.i2c_port = PORT;
        bus_config.sda_io_num = (gpio_num_t)SDA_PIN;
        bus_config.scl_io_num = (gpio_num_t)SCL_PIN;
        bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
        bus_config.glitch_ignore_cnt = 7;
        bus_config.flags.enable_internal_pullup = true;

        i2c_new_master_bus(&bus_config, &bus_handles[PORT]);
    }

    i2c_master_bus_handle_t getHandle(i2c_port_num_t PORT) {
        return bus_handles[PORT];
    }

}
