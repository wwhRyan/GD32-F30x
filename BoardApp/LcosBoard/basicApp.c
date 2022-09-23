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
#include "rti_vc_panel.h"
#include "rti_vc_rdc.h"
#include "rti_vc_regio.h"
#include "sort.h"
#include "utils.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern const SoftwareI2C raontech_i2c;
extern const dac_t laser_dac;

const ntc_t NCP18WB473F10RB = {
    .B = 4108, /* NTC B常数 (25-85℃) */
    .normal_R = 47000, /* 电阻值 (25℃) ，单位Ω*/
    .divided_voltage_R = 100000, /* 分压电阻，单位Ω */
    .is_pull_up = true, /* 分压电阻是上拉电阻？上拉：TRUE，下拉：false */
};

uint8_t get_reg(uint8_t dev_addr, uint16_t reg_addr)
{
    uint8_t reg_val = 0;

    ISoftwareI2CRegRead(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, I2C_DELAY_TIME);
    return reg_val;
}

bool set_reg(uint8_t dev_addr, uint16_t reg_addr, uint8_t reg_val)
{
    bool ret;

    ret = ISoftwareI2CRegWrite(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, I2C_DELAY_TIME);
    return ret;
}

bool get_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;

    ret = ISoftwareI2CRegRead(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, I2C_DELAY_TIME);
    return ret;
}

bool set_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;

    ret = ISoftwareI2CRegWrite(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, I2C_DELAY_TIME);
    return ret;
}

char* get_rdc200a_version(char* buff, size_t size)
{
    uint16_t id;
    uint16_t rev;

    rtiVC_GetRdcDeviceID(&id, &rev);
    snprintf(buff, size, "%#x_%#x", id, rev);
    return buff;
}

char* get_rdp250h_version(char* buff, size_t size)
{
    uint16_t id;
    uint16_t rev;

    rtiVC_GetPanelDeviceID(VC_PANEL_PORT_0, &id, &rev);
    snprintf(buff, size, "%#x_%#x", id, rev);
    return buff;
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
        ntc_R = ((double)Voltage / (3.3 - (double)Voltage)) * ntc->divided_voltage_R;
    else
        ntc_R = ((3.3 - (double)Voltage) / 3.3) * ntc->divided_voltage_R;

    return (298.15 * ntc->B) / (298.15 * log(ntc_R / ntc->normal_R) + ntc->B) - 273.15;
}

float get_temperature(temperature_t* p_temp)
{
    uint16_t value = get_ntc_adc_sample(p_temp->p_ntc_adc_config);
    float voltage = (double)value * 3.3 / 4095;
    float measure_temp = get_ntc_temperature(p_temp->p_ntc, voltage);
    array_shift(p_temp->buff, sensor_num, 4);
    p_temp->buff[sensor_num - 1] = (int)(measure_temp * 10);
    quick_sort(p_temp->buff, 0, sensor_num - 1);
    p_temp->temperature = p_temp->buff[5 / 2 + 1];
    return (float)p_temp->temperature / 10;
}

bool get_i2c_temperature(temperature_i2c_t* p_temp)
{
    uint8_t value[2] = { 0 };
    bool ret = eeprom_block_read(p_temp->p_i2c, 0x00, value, sizeof(value));

    int temperature = (value[0] & 0x7f) * 10; // temperature 定点一个小数点
    temperature += (((value[1] >> 4) & 0x0f) * 10) / 16;
    if (GET_BIT(value[0], 7))
        temperature = -temperature;

    array_shift(p_temp->buff, sensor_num, 4);
    p_temp->buff[sensor_num - 1] = temperature;
    quick_sort(p_temp->buff, 0, sensor_num - 1);
    p_temp->temperature = p_temp->buff[5 / 2 + 1];
    return ret;
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
    DelayUs(250);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    // gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

inline void B_to_G()
{
    laser_dac_set(G_CURRENT);

    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(200);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
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
    ULOG_TRACE("valcrc %#x\n", valcrc);
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
