/**
 * @file basicApp.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-04-14
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#ifndef _BASICAPP_H_
#define _BASICAPP_H_

#include "Common.h"
#include "gd32f30x.h"
#include "i2c.h"
#include <stdint.h>
#include "eeprom.h"
#include "adc_mcu.h"

#define RDC200A_ADDR (0x4A << 1)

#if 1
#define R_CURRENT (eeprom.red)
#define G_CURRENT (eeprom.green)
#define B_CURRENT (eeprom.blue)
#else
#define R_CURRENT (2.5 + 0.3)
#define G_CURRENT (4.6 + 0.5)
#define B_CURRENT (4.6 + 0.5)
#endif

typedef enum color_t {
    RED,
    GREEN,
    BLUE,
    color_num,
} color_t;

typedef struct ntc_t {
    uint16_t B;
    uint16_t normal_R;
    uint16_t divided_voltage_R;
    bool is_pull_up; // true: divided_voltage_R pull up, false: divided_voltage_R pull down
} ntc_t;

typedef struct temperature_t{
    const ntc_adc_config_t * p_ntc_adc_config;
    const ntc_t * p_ntc;
    int temperature;
    int buff[5];/* store temperature * 10 for filtering */
}temperature_t;

typedef enum temperature_enum_t {
    blue_sensor,
    red_sensor,
    green_sensor,
    evn_sensor,
    lcos_sensor,
    sensor_num,
} temperature_enum_t;

typedef struct temperature_i2c_t{
    const eeprom_model_t * p_i2c;
    int temperature;
    int buff[5];/* store temperature * 10 for filtering */
}temperature_i2c_t;

uint8_t get_reg(uint8_t dev_addr, uint16_t reg_addr);
bool set_reg(uint8_t dev_addr, uint16_t reg_addr, uint8_t reg_val);
bool get_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size);
bool set_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size);
char* get_rdc200a_version(char* buff, size_t size);
char* get_rdp250h_version(char* buff, size_t size);

void laser_on(void);
void laser_off(void);
bool laser_set(int idx, float current);
void laser_dac_set(float current);

uint8_t get_idu_value(float current);
float get_current_value(uint8_t idu);
float get_temperature(temperature_t * p_temp);
bool get_i2c_temperature(temperature_i2c_t* p_temp);

void reload_idu_current(void);

void color_EN_EXIT_IRQ(color_t color);

char* get_sn(int number, char* buff);
uint32_t get_MSB_array_crc(uint8_t* array, size_t size);
uint32_t get_LSB_array_crc(uint8_t* array, size_t size);

extern const ntc_t NCP18WB473F10RB;

#endif
