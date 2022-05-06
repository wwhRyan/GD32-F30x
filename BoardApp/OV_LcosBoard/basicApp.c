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

#define EEPROM_ADDRESS 0xA0 //8'h 0xA0
#define EEPROM_WRITE (EEPROM_ADDRESS | 0x00)
#define EEPROM_READ (EEPROM_ADDRESS | 0x01)

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

uint8_t get_idu_value(float current)
{
    return (1.24 * 1000 - 40 * 0.075 * current) * 128 / (40 * 50 * current);
}

float get_current_value(uint8_t idu)
{
    return 9920.0 / (125.0 * idu + 24.0);
}

#define UNLOCK false
#define LOCK true
/**
 * @brief eeprom lock function
 * 
 * @param lock true: lock, false: unlock
 */
void eeprom_lock(bool lock)
{
    if (lock == false)
    {
        gpio_bit_set(EE_WP_PORT, EE_WP_PIN); // set WP pin to high, can write
        DelayUs(10);
    }
    else
    {
        DelayUs(10);
        gpio_bit_reset(EE_WP_PORT, EE_WP_PIN); // set WP pin to low, can't write
    }
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
    uint8_t digital_value = get_idu_value(current);
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
        vTaskDelay(1);
        eeprom_write((uint8_t)idx, digital_value);
    }
    return true;
}

float laser_get(int idx)
{
    return get_current_value(eeprom_read((uint8_t)idx));
}

bool eeprom_write(uint8_t addr, uint8_t data)
{
    bool ret = false;
    eeprom_lock(UNLOCK);
    ret = ISoftwareI2CRegWrite(&ovp921_i2c, EEPROM_WRITE, addr, REG_ADDR_1BYTE, &data, 1, 0xFFFF);
    eeprom_lock(LOCK);
    return ret;
}

uint8_t eeprom_read(uint8_t addr)
{
    uint8_t data;
    eeprom_lock(UNLOCK);
    ISoftwareI2CRegRead(&ovp921_i2c, EEPROM_READ, addr, REG_ADDR_1BYTE, &data, 1, 0xFFFF);
    eeprom_lock(LOCK);
    return data;
}
