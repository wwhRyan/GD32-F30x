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
    dac_trigger_disable(dac_base);
    dac_wave_mode_config(dac_base, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(dac_base);

    /* enable dac_base */
    // dac_enable(dac_base);

    /* enable DAC concurrent mode and set data */
    dac_concurrent_enable();
    dac_concurrent_data_set(DAC_ALIGN_12B_R, 0, 0);
}

void set_dac_value(uint32_t dac_base, uint32_t value)
{
    dac_concurrent_data_set(DAC_ALIGN_12B_R, value, value);
}

void laser_dac_init(const dac_t *laser_dac_config)
{
    software_triggle_dac_init(laser_dac_config->dac_clock, laser_dac_config->dac_base,
                              laser_dac_config->gpio_port, laser_dac_config->gpio_pin);
}

/**
 * @brief 
 * 
 * @param laser_dac_config dac_t *
 * @param value 0~4095
 */
void laser_dac_set_value(const dac_t *laser_dac_config, uint32_t value)
{
    set_dac_value(laser_dac_config->dac_base, value);
}
