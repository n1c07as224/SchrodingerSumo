#pragma once

#include "esp_adc/adc_oneshot.h"


extern adc_oneshot_unit_handle_t adc_unit1_handle;
extern adc_oneshot_unit_handle_t adc_unit2_handle;


namespace ADC_UNIT {

        void init(); 

}



