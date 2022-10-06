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
#include "gd32f30x_gpio.h"
#include "i2c.h"
#include "rti_vc_api.h"
#include "rti_vc_panel.h"
#include "rti_vc_rdc.h"
#include "rti_vc_regio.h"
#include "sort.h"
#include "ulog.h"
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
    .is_divided_voltage_R_pull_up = true, /* 分压电阻是上拉电阻？上拉：TRUE，下拉：false */
};

bool i2c_write(const i2c_sensor_t* model, uint16_t addr, uint8_t data)
{
    bool ret = true;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, addr,
        model->i2c_addr_type, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    vTaskDelay(model->write_delay_time);

    return ret;
}

uint8_t i2c_read(const i2c_sensor_t* model, uint8_t addr)
{
    uint8_t data;
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    return data;
}

bool i2c_muti_write(const i2c_sensor_t* model, uint16_t WriteAddr, uint8_t* data, uint16_t size)
{
    bool ret = true;
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

    Addr = WriteAddr % model->page_size;
    count = model->page_size - Addr; /* 差count个数据值，刚好可以对齐到页地址 */
    NumOfPage = size / model->page_size;
    NumOfSingle = size % model->page_size; /* mod运算求余，计算出剩余不满一页的字节数 */

    if (Addr == 0) {

        if (NumOfPage == 0) {
            xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
            ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, NumOfSingle, 0xFFFF);
            xSemaphoreGive(i2c_Semaphore);
            vTaskDelay(model->write_delay_time);
        }

        else {
            while (NumOfPage--) {
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, model->page_size, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);
                WriteAddr += model->page_size;
                data += model->page_size;
            }

            if (NumOfSingle != 0) {
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, NumOfSingle, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);
            }
        }
    }

    else {

        if (NumOfPage == 0) {
            if (count > NumOfSingle) {
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, NumOfSingle, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);
            } else {
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, count, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);

                WriteAddr += count;
                data += count;
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, NumOfSingle - count, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);
            }
        }

        else {
            size -= count;
            NumOfPage = size / model->page_size;
            NumOfSingle = size % model->page_size;

            if (count != 0) {
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, count, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);
                WriteAddr += count;
                data += count;
            }

            while (NumOfPage--) {
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, model->page_size, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);
                WriteAddr += model->page_size;
                data += model->page_size;
            }
            if (NumOfSingle != 0) {
                xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
                ret &= ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, WriteAddr, model->i2c_addr_type, data, NumOfSingle, 0xFFFF);
                xSemaphoreGive(i2c_Semaphore);
                vTaskDelay(model->write_delay_time);
            }
        }
    }
    return ret;
}

bool i2c_muti_read(const i2c_sensor_t* model, uint16_t addr, uint8_t* data, uint16_t size)
{
    bool ret = true;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type, data, size, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    vTaskDelay(model->write_delay_time);
    return ret;
}

uint8_t get_reg(uint8_t dev_addr, uint16_t reg_addr)
{
    uint8_t reg_val = 0;
    xSemaphoreTake(lcos_i2c_Semaphore, (TickType_t)0xFFFF);

    ISoftwareI2CRegRead(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, I2C_DELAY_TIME);
    xSemaphoreGive(lcos_i2c_Semaphore);

    return reg_val;
}

bool set_reg(uint8_t dev_addr, uint16_t reg_addr, uint8_t reg_val)
{
    bool ret;
    xSemaphoreTake(lcos_i2c_Semaphore, (TickType_t)0xFFFF);

    ret = ISoftwareI2CRegWrite(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, I2C_DELAY_TIME);
    xSemaphoreGive(lcos_i2c_Semaphore);
    return ret;
}

bool get_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;
    xSemaphoreTake(lcos_i2c_Semaphore, (TickType_t)0xFFFF);

    ret = ISoftwareI2CRegRead(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, I2C_DELAY_TIME);
    xSemaphoreGive(lcos_i2c_Semaphore);
    return ret;
}

bool set_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;
    xSemaphoreTake(lcos_i2c_Semaphore, (TickType_t)0xFFFF);

    ret = ISoftwareI2CRegWrite(&raontech_i2c, dev_addr, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, I2C_DELAY_TIME);
    xSemaphoreGive(lcos_i2c_Semaphore);
    return ret;
}

void set_panel_reg_block(uint16_t reg_addr, uint8_t* buff, size_t size)
{
    for (int i = 0; i < size; i++) {
        RDP_REG_SET(VC_PANEL_PORT_0, reg_addr + i, buff[i]);
    }
}

void get_panel_reg_block(uint16_t reg_addr, uint8_t* buff, size_t size)
{
    for (int i = 0; i < size; i++) {
        buff[i] = RDP_REG_GET(VC_PANEL_PORT_0, reg_addr + i);
    }
}

bool check_boot_done()
{
    for (int i = 0; i < 25; i++) {
        vTaskDelay(100);
        if (gpio_input_bit_get(RDC200A_BOOT_OUT_PORT, RDC200A_BOOT_OUT_PIN)) {
            return true;
        }
    }
    return false;
}

bool power_on()
{
    gpio_bit_set(RDC200A_VCC_EN_PORT, RDC200A_VCC_EN_PIN);
    vTaskDelay(1);
    gpio_bit_set(RDC200A_RESET_PORT, RDC200A_RESET_PIN);

    return check_boot_done();
}

void power_off()
{
    rtiVC_PowerOnPanel(false);
    vTaskDelay(10);

    gpio_bit_reset(RDC200A_RESET_PORT, RDC200A_RESET_PIN);
    vTaskDelay(10);

    gpio_bit_reset(RDC200A_VCC_EN_PORT, RDC200A_VCC_EN_PIN);
}

bool power_resume()
{
    if (gpio_output_bit_get(SYS_12V_ON_PORT, SYS_12V_ON_PIN) != true) {
        gpio_bit_reset(RDC200A_RESET_PORT, RDC200A_RESET_PIN);
        gpio_bit_set(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
        vTaskDelay(100);
        if (!power_on()) {
            power_off();
            gpio_bit_reset(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            return false;
        }
        ULOG_DEBUG("system resume\n");
    }
    return true;
}

char* get_rdc200a_version(char* buff, size_t size)
{
    uint16_t id;
    uint16_t rev;
    uint8_t major;
    uint8_t minor;

    rtiVC_GetRdcDeviceID(&id, &rev);
    major = (RDC_REG_GET(0x0002) & 0xf0) >> 4;
    minor = RDC_REG_GET(0x0003);
    snprintf(buff, size, "V%x_%x.%x", id, major, minor);
    return buff;
}

char* get_rdp250h_version(char* buff, size_t size)
{
    uint16_t id;
    uint16_t rev;

    rtiVC_GetPanelDeviceID(VC_PANEL_PORT_0, &id, &rev);
    snprintf(buff, size, "V%x_%x", id, rev);
    return buff;
}

char* get_eeprom_version(char* buff,size_t size){
    uint16_t major = (eeprom.version &0xffff0000)>>4;
    uint16_t minor = (eeprom.version &0x0000ffff);

    snprintf(buff, size, "V%x.%x",major,minor);
}

bool check_video_input(void)
{
    // unsigned short v_active = (((unsigned short)RDC_REG_GET(0x029B) << 8) | (unsigned short)RDC_REG_GET(0x029C));
    // unsigned short h_active = (((unsigned short)RDC_REG_GET(0x0295) << 8) | (unsigned short)RDC_REG_GET(0x0296));

    // if (v_active != VERTICAL_PIXEL || h_active != HORIZONTAL_PIXEL)
    //     return false;

    // unsigned short mrx_stability = ((unsigned short)RDC_REG_GET(0x0291)) & 0x03;
    // if (mrx_stability != 0x03)
    //     return false;

    return true;
}

bool h_v_flip_set(flip_t filp)
{
    unsigned short reg = (unsigned short)RDC_REG_GET(0x870);
    reg &= (unsigned short)0xFC;
    reg |= (unsigned short)filp;
    RDC_REG_SET(0x870, reg);
    return true;
}

flip_t h_v_flip_get(void)
{
    unsigned short reg = (unsigned short)RDC_REG_GET(0x870);
    // reg &= (unsigned short)v_1_h_1;
    reg &= (unsigned short)0x3;
    return (flip_t)reg;
}

void Border(uint8_t red, uint8_t green, uint8_t blue)
{
    rtiVC_EnableTestPattern(true);

    RDC_REG_SET(0x0280, 0x00);
    rtiVC_GenerateTestPattern(red, green, blue);
}

void check_box(void)
{
    rtiVC_EnableTestPattern(true);
    RDC_REG_SET(0x0280, 0x6A);
    RDC_REG_SET(0x0281, 0xF0);
    RDC_REG_SET(0x0282, 0xF0);
    RDC_REG_SET(0x0283, 0xF0);
}

void vertical_gradation(void)
{
    rtiVC_EnableTestPattern(true);
    RDC_REG_SET(0x0280, 0x23);
}

void horizontal_gradation(void)
{
    rtiVC_EnableTestPattern(true);
    RDC_REG_SET(0x0280, 0x12);
}

void off_testpattern(void)
{
    rtiVC_EnableTestPattern(false);
    RDC_REG_SET(0x0200, 0x02);
    RDC_REG_SET(0x0280, 0x00);
}

char* get_test_pattern(void)
{
    uint8_t rdc200a_mrx_config = RDC_REG_GET(0x0200);
    ;
    uint8_t rdc200a_pgen_config = RDC_REG_GET(0x0280);
    ;

    if (rdc200a_mrx_config != 0x04)
        return "Off";
    else {
        if (rdc200a_pgen_config == 0x00) {
            uint32_t raw = (uint32_t)(RDC_REG_GET(0x0281)) << 16 | (uint32_t)(RDC_REG_GET(0x0282)) << 8 | (uint32_t)(RDC_REG_GET(0x0283));
            switch (raw) {
            case 0x00FF0000:
                return "Red";
            case 0x0000FF00:
                return "Green";
            case 0x000000FF:
                return "Blue";
            case 0x00808080:
                return "Grey";
            case 0x00FF00FF:
                return "Magenta";
            case 0x0000FFFF:
                return "Cyan";
            case 0x00FFFF00:
                return "Yellow";
            case 0x00000000:
                return "Black";
            case 0x00FFFFFF:
                return "White";
            default:
                return "Unknow";
            }
        } else if (rdc200a_pgen_config == 0x6A) {
            return "Checkerboard";
        } else if (rdc200a_pgen_config == 0x12) {
            return "VerticalRamp";
        } else if (rdc200a_pgen_config == 0x23) {
            return "HorizontalRamp";
        } else {
            return "Unknow";
        }
    }
}

#define FLASH_SECTOR_SIZE (4 * 1024)
bool spi_flash_erase(size_t WriteAddr, size_t size)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    Addr = WriteAddr % FLASH_SECTOR_SIZE;
    count = FLASH_SECTOR_SIZE - Addr;
    NumOfPage = size / FLASH_SECTOR_SIZE; /* 包含多少个sector */
    NumOfSingle = size % FLASH_SECTOR_SIZE; /* 剩余不满sector字节数 */

    if (Addr == 0) {
        if (NumOfPage == 0) {
            if (rtiVC_EraseSectorFLASH(Addr) != 0)
                return false;
        } else {
            while (NumOfPage--) {
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;

                WriteAddr += FLASH_SECTOR_SIZE;
            }
            if (NumOfSingle != 0) {
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;
            }
        }
    } else {
        if (NumOfPage == 0) {
            if (count > NumOfSingle) {
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;
            } else {
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;
                WriteAddr += count;
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;
            }
        } else {
            size -= count;
            NumOfPage = size / FLASH_SECTOR_SIZE;
            NumOfSingle = size % FLASH_SECTOR_SIZE;

            if (count != 0) {
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;
                WriteAddr += count;
            }

            while (NumOfPage--) {
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;
                WriteAddr += FLASH_SECTOR_SIZE;
            }

            if (NumOfSingle != 0) {
                if (rtiVC_EraseSectorFLASH(WriteAddr) != 0)
                    return false;
            }
        }
    }
    return true;
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
    if (ntc->is_divided_voltage_R_pull_up == true)
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
    bool ret = i2c_muti_read(p_temp->p_i2c, 0x00, value, sizeof(value));

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

float get_rdp250h_register_temperature()
{
    float temperature[MAX_NUM_VC_PANEL_PORT] = { 0 };

    VC_PANEL_TEMPERATURE_INFO_T tinfo[MAX_NUM_VC_PANEL_PORT] = { 0 };

    rtiVC_prepare_panel();
    rtiVC_GetTemperature(VC_PANEL_CTRL_PORT_0, tinfo);
    temperature[0] = (float)(tinfo[0].temperature) / VC_TEMPERATURE_DEGREE_DIV;
    // temperature[1] = (float)(tinfo[1].temperature) / VC_TEMPERATURE_DEGREE_DIV;
    return temperature[0];
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

inline void B_to_R()
{
    laser_dac_set(R_CURRENT);

    gpio_bit_set(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_set(DISCHARGE2_PORT, DISCHARGE2_PIN);
    DelayUs(200);
    gpio_bit_reset(DISCHARGE_PORT, DISCHARGE_PIN);
    gpio_bit_reset(DISCHARGE2_PORT, DISCHARGE2_PIN);
}

// RGBG
color_t color_index[color_num] = {
    GREEN,
    BLUE,
    RED,
};

void spoke(color_t last_color, color_t next_color)
{
    switch (last_color) {
    case RED:
        if (next_color == GREEN)
            R_to_G();
        break;

    case GREEN:
        if (next_color == BLUE)
            G_to_B();
        break;

    case BLUE:
        if (next_color == RED)
            B_to_R();
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
        if (cnt[color] == 4) // It is time to switch different colors and change the current value of different colors
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
