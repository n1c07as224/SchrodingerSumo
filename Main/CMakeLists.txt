file(GLOB_RECURSE SRCS "*.cpp")

idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_timer driver esp_adc
)