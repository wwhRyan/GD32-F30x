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
#include "eeprom.h"
#include "i2c.h"
#include "rti_vc_api.h"
#include "rti_vc_rdc.h"
#include "rti_vc_regio.h"
#include "sort.h"
#include "utils.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

extern const SoftwareI2C raontech_i2c;
extern const dac_t laser_dac;

const ntc_t NCP18WB473F10RB = {
    .B = 4108,
    .normal_R = 47000,
    .divided_voltage_R = 10000,
    .is_pull_up = true,
};

uint8_t get_reg(uint8_t dev_addr, uint16_t reg_addr)
{
    uint8_t reg_val = 0;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ISoftwareI2CRegRead(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, I2C_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return reg_val;
}

bool set_reg(uint8_t dev_addr, uint16_t reg_addr, uint8_t reg_val)
{
    bool ret;
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);

    ret = ISoftwareI2CRegWrite(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, I2C_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return ret;
}

bool get_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegRead(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, I2C_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return ret;
}

bool set_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);

    ret = ISoftwareI2CRegWrite(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, I2C_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return ret;
}

/**
 * @brief Get the ntc temperature
 *
 * @param ntc object
 * @param Voltage 0~3.3V
 */
float get_ntc_temperature(const ntc_t* ntc, float Voltage)
{
    float ntc_R;
    if (ntc->is_pull_up == false)
        ntc_R = (3.3 / (double)Voltage) * ntc->divided_voltage_R - ntc->divided_voltage_R;
    else
        ntc_R = (3.3 / (3.3 - (double)Voltage)) * ntc->divided_voltage_R - ntc->divided_voltage_R;

    return (298.15 * ntc->B) / (298.15 * log(ntc_R / ntc->normal_R) + ntc->B) - 273.15;
}

float get_temperature(temperature_t* p_temp)
{
    float voltage = get_ntc_adc_sample(p_temp->p_ntc_adc_config);
    float measure_temp = get_ntc_temperature(p_temp->p_ntc, voltage);
    array_shift(p_temp->buff, sensor_num, 4);
    p_temp->buff[sensor_num - 1] = (int)(measure_temp * 10);
    quick_sort(p_temp->buff, 0, sensor_num - 1);
    p_temp->temperature = p_temp->buff[5 / 2 + 1];
    return (float)p_temp->temperature / 10;
}

float get_i2c_temperature(temperature_i2c_t* p_temp)
{
    int value = eeprom_read(p_temp->p_i2c, 0x00);
    int temperature;
    if (GET_BIT(value, 11))
        temperature = -value * 10 / 16;
    else
        temperature = value * 10 / 16;

    array_shift(p_temp->buff, sensor_num, 4);
    p_temp->buff[sensor_num - 1] = temperature;
    quick_sort(p_temp->buff, 0, sensor_num - 1);
    p_temp->temperature = p_temp->buff[5 / 2 + 1];
    return (float)p_temp->temperature / 10;
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
    if (digital_value > 127 || digital_value < 21) {
        debug_printf("%s param current value error!\n", __func__);
        return false;
    } else if (idx > 3 || idx < 0) {
        debug_printf("%s param idx error!\n", __func__);
        return false;
    } else {
        xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
        ISoftwareI2CRegWrite(&raontech_i2c, 0x66, (uint8_t)idx, REG_ADDR_1BYTE, &digital_value, 1, 5);
        xSemaphoreGive(i2c_Semaphore);
        vTaskDelay(1);
    }
    return true;
}

void reload_idu_current(void)
{
    uint8_t red_idu_value = eeprom.red;
    uint8_t green_idu_value = eeprom.green;
    uint8_t blue_idu_value = eeprom.blue;

    if (red_idu_value <= 127 && red_idu_value >= 21) {
        laser_set(RED, get_current_value(red_idu_value));
        debug_printf("RED: %f A\r\n", get_current_value(red_idu_value));
    } else {
        laser_set(RED, 1.0);
        debug_printf("laser_set RED is %f A\r\n", 1.0);
    }

    if (green_idu_value <= 127 && green_idu_value >= 21) {
        laser_set(GREEN, get_current_value(green_idu_value));
        debug_printf("GREEN: %f A\r\n", get_current_value(green_idu_value));
    } else {
        laser_set(GREEN, 0.8);
        debug_printf("laser_set GREEN is %f A\r\n", 0.8);
    }

    if (blue_idu_value <= 127 && blue_idu_value >= 21) {
        laser_set(BLUE, get_current_value(blue_idu_value));
        debug_printf("BLUE: %f A\r\n", get_current_value(blue_idu_value));
    } else {
        laser_set(BLUE, 0.623);
        debug_printf("laser_set BLUE is %f A\r\n", 0.623);
    }
}

// DISCHARGE2 voltage big --> little
// DISCHARGE current big --> little

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
    switch (last_color) {
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
    static uint8_t cnt[color_num] = { 0 };
    cnt[color]++;

    uint8_t x = (color + 1) % color_num;
    uint8_t y = (color + 2) % color_num;
    if (cnt[x] == 0 && cnt[y] == 0) {
        if (cnt[color] == 2) // It is time to switch different colors and change the current value of different colors
        {
            spoke(color, color_index[color]);
            cnt[color] = 0;
        }
    } else { // Two red, green and blue pulses appear at the same time, abnormal, clear the count value
        cnt[x] = 0;
        cnt[y] = 0;
        cnt[color] = 1;
        // error_detect();
    }
}

char* get_sn(int number, char* buff)
{
    char* tmp = NULL;
    switch (number) {
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

    if (*((uint32_t*)tmp) != 0xffffffff && *((uint32_t*)tmp) != 0)
        strcpy(buff, tmp);
    else
        strcpy(buff, "NULL");

    return buff;
}

uint32_t get_MSB_array_crc(uint8_t* array, size_t size)
{
    E_assert(size % sizeof(uint32_t) == 0);
    rcu_periph_clock_enable(RCU_CRC);
    /* reset the CRC data register and calculate the CRC of the value */
    crc_data_register_reset();

    uint32_t valcrc = 0;
    uint32_t* pval32 = (uint32_t*)array;
    for (size_t i = 0; i < size / sizeof(uint32_t); i++) {
        valcrc = crc_single_data_calculate(BSWAP_32(*(pval32 + i)));
    }
    debug_printf("valcrc %#x\n", valcrc);
    return valcrc;
}

uint32_t get_LSB_array_crc(uint8_t* array, size_t size)
{
    E_assert(size % sizeof(uint32_t) == 0);
    rcu_periph_clock_enable(RCU_CRC);
    /* reset the CRC data register and calculate the CRC of the value */
    crc_data_register_reset();

    uint32_t valcrc = 0;
    uint32_t* pval32 = (uint32_t*)array;
    for (size_t i = 0; i < size / sizeof(uint32_t); i++) {
        valcrc = crc_single_data_calculate(*(pval32 + i));
    }
    return valcrc;
}
