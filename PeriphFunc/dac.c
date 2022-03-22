/**
 * @file dac.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-03-22
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "dac.h"

void software_triggle_dac_init(rcu_periph_enum dac_clock, uint32_t dac_base, uint32_t port, uint16_t pin)
{
    rcu_periph_clock_enable(dac_clock);
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, pin);

    dac_deinit();
    /* configure the dac_base */
    dac_trigger_source_config(dac_base, DAC_TRIGGER_SOFTWARE);
    dac_trigger_enable(dac_base);
    dac_wave_mode_config(dac_base, DAC_WAVE_DISABLE);

    /* enable dac_base and set data */
    dac_enable(dac_base);
    dac_data_set(dac_base, DAC_ALIGN_12B_R, 0);
}

void set_dac_value(uint32_t dac_base, uint32_t value)
{
    dac_data_set(dac_base, DAC_ALIGN_12B_R, value);
}

void laser_dac_init(const laser_dac_config_t *laser_dac_config)
{
    software_triggle_dac_init(laser_dac_config->dac_clock, laser_dac_config->dac_base,
                              laser_dac_config->gpio_port, laser_dac_config->gpio_pin);
}
void laser_dac_set_value(const laser_dac_config_t *laser_dac_config, uint32_t value)
{
    set_dac_value(laser_dac_config->dac_base, value);
}
