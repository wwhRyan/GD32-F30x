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
#include "i2c.h"

extern const SoftwareI2C ovp921_i2c;

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

/**
 * @brief 
 * 
 * @param idx R\G\B\Y 0\1\2\3
 * @param current A
 * @note i2c addr 0x66
 * @note i2c speed 100K
 * @note min 127, max 21
 */
bool laser_set(int idx, float current)
{
    uint8_t digital_value = (1.24 * 1000 - 40 * 0.075 * current) * 128 / (40 * 50 * current);
    if (digital_value > 127 || digital_value < 21)
    {
        debug_printf("%s param current value error!\n", __func__);
        return false;
    }
    else if (idx > 3 || idx < 0)
    {
        debug_printf("%s param idx error!\n", __func__);
        return false;
    }
    else
    {
        ISoftwareI2CRegWrite(&ovp921_i2c, 0x66, (uint8_t)idx, REG_ADDR_1BYTE, &digital_value, 1, 5);
    }
    return true;
}
