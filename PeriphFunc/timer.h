/**
 * @file timer.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-03-21
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef TIMER_H
#define TIMER_H

#include "port.h"
#include "gd32f30x.h"

typedef struct fan_timer_config_t{
    rcu_periph_enum timer_clock;
    uint32_t timer_base;
    uint16_t timer_channel;
    uint32_t gpio_port;
    uint16_t gpio_pin;
    /* data */
}fan_timer_config_t;


void timer_pwm_config(rcu_periph_enum timer_clock, uint32_t timer_base, uint16_t timer_channel, uint32_t gpio_port, uint16_t gpio_pin);
void timer_counter_config(rcu_periph_enum timer_clock, uint32_t timer_base, uint16_t timer_channel, uint32_t gpio_port, uint16_t gpio_pin);

uint32_t get_timer_counter(uint32_t timer_base, uint16_t timer_channel);
void set_timer_counter(uint32_t timer_base, uint16_t timer_channel, uint16_t percent);

void fan_timer_pwm_config(const fan_timer_config_t *fan_timer_config);
void fan_timer_FG_config(const fan_timer_config_t *fan_timer_config);

void Set_fan_timer_pwm(const fan_timer_config_t *fan_timer_config, uint16_t percent);
uint32_t Get_fan_timer_FG(const fan_timer_config_t *fan_timer_config);

#endif
