/**
 * @file timer.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-03-21
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "main.h"
#include "timer.h"
#include "systick.h"

/**
 * @brief 
 * 
 * @param timer_clock 
 * @param timer_base 
 * @param timer_channel 
 * @param gpio_port 
 * @param gpio_pin 
 */
void timer_pwm_config(rcu_periph_enum timer_clock, uint32_t timer_base, uint16_t timer_channel, uint32_t gpio_port, uint16_t gpio_pin)
{
    /* -----------------------------------------------------------------------
    timer_base configuration: generate 1 PWM signals with 1 different duty cycles:
    timer_base CLK = SystemCoreClock / 120 = 1MHz

    timer_base channel0 duty cycle = (4000/ 16000)* 100  = 25%
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(timer_clock);
    rcu_periph_clock_enable(RCU_AF);

    // timer_deinit(timer_base);

    /* timer_base configuration */
    timer_initpara.prescaler = 0;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 4800 - 1;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(timer_base, &timer_initpara);

    /* timer_channel configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(timer_base, timer_channel, &timer_ocintpara);

    /* timer_channel configuration in PWM mode0, default duty cycle 25% */
    timer_channel_output_pulse_value_config(timer_base, timer_channel, (4800 - 1) /4);
    timer_channel_output_mode_config(timer_base, timer_channel, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(timer_base, timer_channel, TIMER_OC_SHADOW_DISABLE);

    gpio_init(gpio_port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, gpio_pin);

    timer_primary_output_config(timer_base, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(timer_base);

    /* timer_base counter enable */
    timer_enable(timer_base);
}

/**
 * @brief Set the timer counter object
 * 
 * @param timer_base 
 * @param timer_channel 
 * @param percent 
 */
void set_timer_counter(uint32_t timer_base, uint16_t timer_channel, uint16_t percent)
{
    timer_channel_output_pulse_value_config(timer_base, timer_channel, (uint32_t)((uint32_t)percent * (4800 - 1) / 100));
}

/**
 * @brief 
 * 
 * @param timer_clock 
 * @param timer_base 
 * @param timer_channel 
 * @param gpio_port 
 * @param gpio_pin 
 */
void timer_counter_config(rcu_periph_enum timer_clock, uint32_t timer_base, uint16_t timer_channel,
                          uint32_t gpio_port, uint16_t gpio_pin, IRQn_Type timer_IRQ, uint32_t channel_interrupt_flag, uint32_t channel_interrupt_enable)
{
    /* timer_base configuration: input capture mode -------------------
    the external signal is connected to timer_base timer_channel pin (gpio_port gpio_pin)
    the rising edge is used as active edge
    the timer_base timer_channel is used to compute the frequency value
    ------------------------------------------------------------ */

    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(timer_clock);

    nvic_irq_enable(timer_IRQ, 1, 1);

    gpio_init(gpio_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, gpio_pin);

    // timer_deinit(timer_base);

    /* timer_base configuration */
    timer_initpara.prescaler = 119;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 65535;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(timer_base, &timer_initpara);

    /* timer_base  configuration */
    /* timer_base timer_channel input capture configuration */
    timer_icinitpara.icpolarity = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter = 0x0;
    timer_input_capture_config(timer_base, timer_channel, &timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(timer_base);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(timer_base, channel_interrupt_flag);
    /* channel 0 interrupt enable */
    timer_interrupt_enable(timer_base, channel_interrupt_enable);

    // timer_primary_output_config(timer_base, ENABLE);

    /* timer_base counter enable */
    timer_enable(timer_base);
}

void fan_timer_pwm_config(const fan_timer_config_t *fan_timer_config)
{
    E_assert(fan_timer_config != NULL);
    timer_pwm_config(fan_timer_config->timer_clock, fan_timer_config->timer_base,
                     fan_timer_config->timer_channel, fan_timer_config->gpio_port, fan_timer_config->gpio_pin);
}

void fan_timer_FG_config(const fan_timer_config_t *fan_timer_config)
{
    E_assert(fan_timer_config != NULL);
    timer_counter_config(fan_timer_config->timer_clock, fan_timer_config->timer_base,
                         fan_timer_config->timer_channel, fan_timer_config->gpio_port, fan_timer_config->gpio_pin,
                         fan_timer_config->timer_IRQ, fan_timer_config->channel_interrupt_flag, fan_timer_config->channel_interrupt_enable);
}

void Set_fan_timer_pwm(const fan_timer_config_t *fan_timer_config, uint16_t percent)
{
    E_assert(fan_timer_config != NULL);
    set_timer_counter(fan_timer_config->timer_base, fan_timer_config->timer_channel, percent);
}

uint32_t Get_fan_timer_FG(const fan_timer_config_t *fan_timer_config)
{
    E_assert(fan_timer_config != NULL);
    return fan_timer_config->p_st_calc->fre;
}

void timer_input_capture_IRQ(const fan_timer_config_t *fan_timer_config)
{
    E_assert(fan_timer_config != NULL);
    E_assert(fan_timer_config->p_st_calc != NULL);
    if (SET == timer_interrupt_flag_get(fan_timer_config->timer_base, fan_timer_config->channel_interrupt_flag))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(fan_timer_config->timer_base, fan_timer_config->channel_interrupt_flag);

        if (0 == fan_timer_config->p_st_calc->ccnumber)
        {
            /* read channel 0 capture value */
            fan_timer_config->p_st_calc->readvalue1 = timer_channel_capture_value_register_read(fan_timer_config->timer_base, fan_timer_config->timer_channel) + 1;
            fan_timer_config->p_st_calc->ccnumber = 1;
        }
        else if (1 == fan_timer_config->p_st_calc->ccnumber)
        {
            /* read channel 0 capture value */
            fan_timer_config->p_st_calc->readvalue2 = timer_channel_capture_value_register_read(fan_timer_config->timer_base, fan_timer_config->timer_channel) + 1;

            if (fan_timer_config->p_st_calc->readvalue2 > fan_timer_config->p_st_calc->readvalue1)
            {
                fan_timer_config->p_st_calc->count = (fan_timer_config->p_st_calc->readvalue2 - fan_timer_config->p_st_calc->readvalue1);
            }
            else
            {
                fan_timer_config->p_st_calc->count = ((0xFFFFU - fan_timer_config->p_st_calc->readvalue1) + fan_timer_config->p_st_calc->readvalue2);
            }

            fan_timer_config->p_st_calc->fre = 1000000U / fan_timer_config->p_st_calc->count;
            fan_timer_config->p_st_calc->ccnumber = 0;
        }
    }
}
