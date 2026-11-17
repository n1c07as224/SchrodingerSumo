idf_component_register(
    SRCS
        "main.cpp"
        "Config/adc_unit.cpp"
        "Config/i2c_bus.cpp"
        "Config/pwm_timer.cpp"
        "Control/pid_control.cpp"
        "Hardware/motor.cpp"
        "Hardware/encoder.cpp"
        "Hardware/distance_sensor.cpp"
        "Hardware/line_sensor.cpp"

    INCLUDE_DIRS
        "."
        "Config"
        "Hardware"
        "Control"

    REQUIRES
        esp_adc
        esp_driver_i2c
        esp_driver_ledc
        esp_driver_gpio
        esp_timer
)
