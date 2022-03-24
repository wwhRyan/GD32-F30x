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

typedef struct timer_capture_calc_t
{
    uint16_t readvalue1;
    uint16_t readvalue2;
    __IO uint16_t ccnumber;
    __IO uint32_t count;
    __IO uint16_t fre;
}timer_capture_calc_t;

typedef struct fan_timer_config_t
{
    rcu_periph_enum timer_clock;
    uint32_t timer_base;
    uint16_t timer_channel;
    uint32_t gpio_port;
    uint16_t gpio_pin;
    /* IRQ */
    IRQn_Type timer_IRQ;
    uint32_t channel_interrupt_flag;
    uint32_t channel_interrupt_enable;
    timer_capture_calc_t * p_st_calc;
} fan_timer_config_t;

void fan_timer_pwm_config(const fan_timer_config_t *fan_timer_config);
void fan_timer_FG_config(const fan_timer_config_t *fan_timer_config);
void timer_input_capture_IRQ(const fan_timer_config_t *fan_timer_config);

void Set_fan_timer_pwm(const fan_timer_config_t *fan_timer_config, uint16_t percent);
uint32_t Get_fan_timer_FG(const fan_timer_config_t *fan_timer_config);

#endif
