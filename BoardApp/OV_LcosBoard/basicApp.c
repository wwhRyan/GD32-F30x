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
#include <math.h>

extern const SoftwareI2C ovp921_i2c;

#define EEPROM_ADDRESS 0xA0 //8'h 0xA0
#define EEPROM_WRITE (EEPROM_ADDRESS | 0x00)
#define EEPROM_READ (EEPROM_ADDRESS | 0x01)

const ntc_t NCP18WB473F10RB = {
    .B = 4108,
    .normal_R = 47000,
    .divided_voltage_R = 10000,
    .is_pull_up = true,
};

/**
 * @brief Get the ntc temperature
 * 
 * @param ntc object
 * @param Voltage 0~3.3V
 */
float get_ntc_temperature(const ntc_t *ntc, float Voltage)
{
    float ntc_R;
    if (ntc->is_pull_up == false)
        ntc_R = (3.3 / (double)Voltage) * ntc->divided_voltage_R - ntc->divided_voltage_R;
    else
        ntc_R = (3.3 / (3.3 - (double)Voltage)) * ntc->divided_voltage_R - ntc->divided_voltage_R;

    // printf("ntc_R = %f\r\n", ntc_R);

    return (298.15 * ntc->B) / (298.15 * log(ntc_R / ntc->normal_R) + ntc->B) - 273.15;
}

float get_temperature(int adc_value)
{
    return get_ntc_temperature(&NCP18WB473F10RB, (double)adc_value / 4095 * 3.3);
}

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

inline void R_to_G()
{
    gpio_bit_set(I_SPOKER_PORT, I_SPOKER_PIN);
    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(40);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

inline void G_to_B()
{
    gpio_bit_set(I_SPOKER_PORT, I_SPOKER_PIN);
    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(40);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

inline void G_to_R()
{
    gpio_bit_set(I_SPOKER_PORT, I_SPOKER_PIN);
    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(40);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

inline void B_to_G()
{
    gpio_bit_set(I_SPOKER_PORT, I_SPOKER_PIN);
    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(40);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

//RGBG
color_t color_index[color_num] = {
    GREEN,
    GREEN,
    GREEN,
};

void spoke(color_t last_color, color_t next_color)
{
    switch (last_color)
    {
    case RED:
        if (next_color == GREEN)
            R_to_G();
        color_index[GREEN] = BLUE;
        break;

    case GREEN:
        if (next_color == BLUE)
            G_to_B();
        if (next_color == RED)
            G_to_R();
        break;

    case BLUE:
        if (next_color == GREEN)
            B_to_G();
        color_index[GREEN] = RED;
        break;

    default:
        break;
    }
}

void color_on(color_t color)
{
    gpio_bit_reset(I_SPOKER_PORT, I_SPOKER_PIN);
}

void error_detect()
{
    gpio_bit_set(I_SPOKER_PORT, I_SPOKER_PIN);
}

void color_EN_EXIT_IRQ(color_t color)
{
    static uint8_t cnt[color_num] = {0};
    cnt[color]++;

    uint8_t x = (color + 1) % color_num;
    uint8_t y = (color + 2) % color_num;
    if (cnt[x] == 0 && cnt[y] == 0)
    {
        if (cnt[color] == 2)
        {
            spoke(color, color_index[color]);
            cnt[color] = 0;
        }
        else if (cnt[color] == 1)
        {
            color_on(color);
        }
    }
    else
    {
        cnt[x] = 0;
        cnt[y] = 0;
        cnt[color] = 1;
        error_detect();
    }
}
