/**
 * @file adc_mcu.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-03-22
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef __ADC_MCU_H__
#define __ADC_MCU_H__

#include "gd32f30x.h"
#include "main.h"
#include "systick.h"

typedef struct ntc_adc_config_t
{
    rcu_periph_enum adc_clock;
    uint32_t adc_base;
    uint16_t adc_channel;
    uint32_t gpio_port;
    uint16_t gpio_pin;

} ntc_adc_config_t;

void software_triggle_adc_init(rcu_periph_enum adc_clock, uint32_t adc_base, uint32_t port, uint16_t pin);
uint16_t get_adc_channel_sample(uint32_t adc_base, uint8_t channel);

void ntc_adc_config(const ntc_adc_config_t *ntc_adc_config);
uint16_t get_ntc_adc_sample(const ntc_adc_config_t *ntc_adc_config);

#endif
