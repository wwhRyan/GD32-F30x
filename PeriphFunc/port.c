/**
 * @file port.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-24
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "port.h"
#include "main.h"
#include "systick.h"

void extern_gpio_interrupt_init(const exti_gpio_t *p_exti_gpio)
{
    //External Interrupt init
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(p_exti_gpio->gpio_clk);
    gpio_init(p_exti_gpio->gpio_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,
              p_exti_gpio->gpio_pin);
    /* enable and set gpio EXTI interrupt to the lowest priority */
    nvic_irq_enable(p_exti_gpio->gpio_IRQ, 0, 0);
    /* connect gpio EXTI line to gpio pin */
    gpio_exti_source_select(p_exti_gpio->port_source, p_exti_gpio->pin_source);
    /* configure gpio EXTI line */
    exti_init(p_exti_gpio->exti_line, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(p_exti_gpio->exti_line);
}

void gpio_table_init(gpio_config_t *table, uint32_t num)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    for (size_t i = 0; i < num; i++)
    {
        gpio_init(table->gpio_port, table->gpio_mode, table->gpio_speed, table->gpio_pin);
        if (table->gpio_mode == GPIO_MODE_OUT_PP)
        {
            if (table->default_state == SET)
            {
                gpio_bit_set(table->gpio_port, table->gpio_pin);
            }
            else
            {
                gpio_bit_reset(table->gpio_port, table->gpio_pin);
            }
        }
        table++;
    }
}
