#include "esp_adc/adc_oneshot.h"
#include "adc_unit.h"


using namespace ADC_UNIT;


adc_oneshot_unit_handle_t adc_unit1_handle = NULL;
adc_oneshot_unit_handle_t adc_unit2_handle = NULL;

void init() {

    // iniciar ADC unit 1
    adc_oneshot_unit_init_cfg_t adc_init_config = {};
    adc_init_config.unit_id = ADC_UNIT_1;
    adc_init_config.clk_src = ADC_RTC_CLK_SRC_DEFAULT;
    adc_init_config.ulp_mode = ADC_ULP_MODE_DISABLE;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_config, &adc_unit1_handle));

    // iniciar ADC unit 2
    adc_init_config.unit_id = ADC_UNIT_2;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_config, &adc_unit2_handle));





    adc_oneshot_chan_config_t chan_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };




    // iniciar canales conectados a los GPIO

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_unit1_handle, ADC_CHANNEL_2, &chan_config)); //GPIO 3
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_unit1_handle, ADC_CHANNEL_9, &chan_config)); //GPIO 10
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_unit2_handle, ADC_CHANNEL_8, &chan_config)); //GPIO 19
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_unit2_handle, ADC_CHANNEL_9, &chan_config)); //GPIO 20


}



