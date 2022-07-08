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
extern const dac_t laser_dac;

#define EEPROM_ADDRESS 0xA0 // 8'h 0xA0
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

// DAC_VALUE=(1.24/24+1.24/36-0.015*电流值）*(4095*36/3.3)
void laser_dac_set(float current)
{
    laser_dac_set_value(&laser_dac, (uint32_t)((1.24 / 24 + 1.24 / 30 - 0.015 * (double)current) * (4095 * 30 / 3.3)));
}

uint8_t get_idu_value(float current)
{
    return (1.24 * 1000 - 40 * 0.075 * current) * 128 / (40 * 50 * current);
}

float get_current_value(uint8_t idu)
{
    return 9920.0 / (125.0 * idu + 24.0);
}

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
        xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
        ISoftwareI2CRegWrite(&ovp921_i2c, 0x66, (uint8_t)idx, REG_ADDR_1BYTE, &digital_value, 1, 5);
        xSemaphoreGive(i2c_Semaphore);
        vTaskDelay(1);
    }
    return true;
}

eeprom_t eeprom;
const uint32_t eeprom_magic_number = 0x12344321;

bool eeprom_write(uint8_t addr, uint8_t data)
{
    bool ret = false;
    eeprom_lock(UNLOCK);
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegWrite(&ovp921_i2c, EEPROM_WRITE, addr, REG_ADDR_1BYTE, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    eeprom_lock(LOCK);
    return ret;
}

uint8_t eeprom_read(uint8_t addr)
{
    uint8_t data;
    eeprom_lock(UNLOCK);
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ISoftwareI2CRegRead(&ovp921_i2c, EEPROM_READ, addr, REG_ADDR_1BYTE, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    eeprom_lock(LOCK);
    return data;
}

/**
 * @brief Call before rtos task
 */
void init_eeprom()
{

    ISoftwareI2CRegRead(&ovp921_i2c, EEPROM_WRITE, 0x00,
                        REG_ADDR_1BYTE, ((uint8_t *)&eeprom), sizeof(eeprom_t), 0xFFFF);

    if (eeprom.magic_num != eeprom_magic_number)
    {
        eeprom_lock(UNLOCK);

        printf("reset eeprom!\n");
        eeprom.magic_num = eeprom_magic_number;
        eeprom.red = 1.00;
        eeprom.green = 0.80;
        eeprom.blue = 0.623;
        eeprom.light_source_time = 0;

        for (size_t i = 0; i < sizeof(eeprom_t); i++)
        {
            ISoftwareI2CRegWrite(&ovp921_i2c, EEPROM_WRITE, i,
                                 REG_ADDR_1BYTE, ((uint8_t *)&eeprom) + i, 1, 0xFFFF);
            DelayMs(10);
        }
        eeprom_lock(LOCK);
    }
    else
    {
        printf("Ok eeprom.\n");
    }
}

void reload_idu_current(void)
{
    uint8_t red_idu_value = eeprom.red;
    uint8_t green_idu_value = eeprom.green;
    uint8_t blue_idu_value = eeprom.blue;

    if (red_idu_value <= 127 && red_idu_value >= 21)
    {
        laser_set(RED, get_current_value(red_idu_value));
        debug_printf("RED: %f A\r\n", get_current_value(red_idu_value));
    }
    else
    {
        laser_set(RED, 1.0);
        debug_printf("laser_set RED is %f A\r\n", 1.0);
    }

    if (green_idu_value <= 127 && green_idu_value >= 21)
    {
        laser_set(GREEN, get_current_value(green_idu_value));
        debug_printf("GREEN: %f A\r\n", get_current_value(green_idu_value));
    }
    else
    {
        laser_set(GREEN, 0.8);
        debug_printf("laser_set GREEN is %f A\r\n", 0.8);
    }

    if (blue_idu_value <= 127 && blue_idu_value >= 21)
    {
        laser_set(BLUE, get_current_value(blue_idu_value));
        debug_printf("BLUE: %f A\r\n", get_current_value(blue_idu_value));
    }
    else
    {
        laser_set(BLUE, 0.623);
        debug_printf("laser_set BLUE is %f A\r\n", 0.623);
    }
}

// DISCHARGE2 voltage big --> little
// DISCHARGE current big --> little

#define R_CURRENT (2.5 + 0.3)
#define G_CURRENT (4.6 + 0.5)
#define B_CURRENT (4.6 + 0.5)

inline void R_to_G()
{
    laser_dac_set(G_CURRENT);

    // gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(100); // 100 -> 72us; 40 -> 28us
    // gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

inline void G_to_B()
{
    laser_dac_set(B_CURRENT);

    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(100);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

inline void G_to_R()
{
    laser_dac_set(R_CURRENT);

    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    // gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(100);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    // gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

inline void B_to_G()
{
    laser_dac_set(G_CURRENT);

    // gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(100);
    // gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

// RGBG
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

void color_EN_EXIT_IRQ(color_t color)
{
    static uint8_t cnt[color_num] = {0};
    cnt[color]++;

    uint8_t x = (color + 1) % color_num;
    uint8_t y = (color + 2) % color_num;
    if (cnt[x] == 0 && cnt[y] == 0)
    {
        if (cnt[color] == 2) // It is time to switch different colors and change the current value of different colors
        {
            spoke(color, color_index[color]);
            cnt[color] = 0;
        }
    }
    else
    { // Two red, green and blue pulses appear at the same time, abnormal, clear the count value
        cnt[x] = 0;
        cnt[y] = 0;
        cnt[color] = 1;
        // error_detect();
    }
}

char *get_sn(int number, char *buff)
{
    char *tmp = NULL;
    switch (number)
    {
    case 0:
        tmp = eeprom.Sn_LightEngine;
        break;
    case 1:
        tmp = eeprom.Sn_SourceLight;
        break;
    case 2:
        tmp = eeprom.Sn_Projector;
        break;
    default:
        E_assert(0);
        break;
    }

    if (*((uint32_t *)tmp) != 0xffffffff && *((uint32_t *)tmp) != 0)
        strcpy(buff, tmp);
    else
        strcpy(buff, "NULL");

    return buff;
}
