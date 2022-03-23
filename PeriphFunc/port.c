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

void gpio_table_init(gpio_config_t *table, uint32_t num)
{
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    /* SWD remap */
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

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
