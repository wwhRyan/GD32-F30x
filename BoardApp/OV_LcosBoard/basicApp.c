/**
 * @file basicApp.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-04-14
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "basicApp.h"

void laser_on(void)
{
    gpio_bit_reset(LD_EN_L_PORT, LD_EN_L_PIN);
    gpio_bit_set(LD_EN_H_PORT, LD_EN_H_PIN);
}

void laser_off(void)
{
    gpio_bit_reset(LD_EN_H_PORT, LD_EN_H_PIN);
    gpio_bit_set(LD_EN_L_PORT, LD_EN_L_PIN);
}
