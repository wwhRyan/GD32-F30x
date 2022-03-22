/**
 * @file port.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-24
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef __PORT_H__
#define __PORT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "gd32f30x.h"

    typedef struct gpio_config_t
    {
        uint32_t gpio_port;
        uint16_t gpio_pin;
        uint8_t gpio_mode;
        uint8_t gpio_speed;
        bit_status default_state;
    } gpio_config_t;

    void gpio_table_init(gpio_config_t *table, uint32_t num);
    void gpio_5_9_external_interrupt_init(void);

#endif
