/**
 * @file dac.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-03-22
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef __DAC_H__
#define __DAC_H__

#include "gd32f30x.h"
#include "main.h"
#include "systick.h"

typedef struct dac_t
{
    rcu_periph_enum dac_clock;
    uint32_t dac_base;
    uint32_t gpio_port;
    uint16_t gpio_pin;
} dac_t;

void software_triggle_dac_init(rcu_periph_enum dac_clock, uint32_t dac_base, uint32_t port, uint16_t pin);

void laser_dac_init(const dac_t *laser_dac_config);
void laser_dac_set_value(const dac_t *laser_dac_config, uint32_t value);

#endif
