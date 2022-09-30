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
#include "adc_mcu.h"
#include "eeprom.h"
#include "gd32f30x.h"
#include "i2c.h"
#include <stdint.h>


#define RDC200A_ADDR (0x4A << 1)
#define VERTICAL_PIXEL 1280
#define HORIZONTAL_PIXEL 720

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
    int normal_R;
    int divided_voltage_R;
    bool is_pull_up; // true: divided_voltage_R pull up, false: divided_voltage_R pull down
} ntc_t;

typedef struct temperature_t {
    const ntc_adc_config_t* p_ntc_adc_config;
    const ntc_t* p_ntc;
    int temperature;
    int buff[5]; /* store temperature * 10 for filtering */
} temperature_t;

typedef enum temperature_enum_t {
    blue_sensor,
    red_sensor,
    green_sensor,
    evn_sensor,
    lcos_sensor,
    sensor_num,
} temperature_enum_t;

typedef enum flip_t {
    v_0_h_0,
    v_0_h_1,
    v_1_h_0,
    v_1_h_1,
} flip_t;

typedef struct temperature_i2c_t {
    const i2c_sensor_t* p_i2c;
    int temperature;
    int buff[5]; /* store temperature * 10 for filtering */
} temperature_i2c_t;

bool i2c_write(const i2c_sensor_t* model, uint16_t addr, uint8_t data);
bool i2c_muti_write(const i2c_sensor_t* model, uint16_t WriteAddr, uint8_t* data, uint16_t size);
uint8_t i2c_read(const i2c_sensor_t* model, uint8_t addr);
bool i2c_muti_read(const i2c_sensor_t* model, uint16_t addr, uint8_t* data, uint16_t size);
uint8_t get_reg(uint8_t dev_addr, uint16_t reg_addr);
bool set_reg(uint8_t dev_addr, uint16_t reg_addr, uint8_t reg_val);
bool get_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size);
bool set_reg_block(uint8_t dev_addr, uint16_t reg_addr, uint8_t* reg_val, size_t size);
void set_panel_reg_block(uint16_t reg_addr, uint8_t* buff, size_t size);
void get_panel_reg_block(uint16_t reg_addr, uint8_t* buff, size_t size);
bool check_boot_done(void);
bool power_on(void);
void power_off(void);
bool power_resume(void);
bool spi_flash_erase(size_t WriteAddr, size_t size);

char* get_rdc200a_version(char* buff, size_t size);
char* get_rdp250h_version(char* buff, size_t size);
bool check_video_input(void);
bool h_v_flip_set(flip_t filp);
flip_t h_v_flip_get(void);
void Border(uint8_t red, uint8_t green, uint8_t blue);
void check_box(void);
void vertical_gradation(void);
void horizontal_gradation(void);
void off_testpattern(void);
char* get_test_pattern(void);

void laser_on(void);
void laser_off(void);
bool laser_set(int idx, float current);
void laser_dac_set(float current);

uint8_t get_idu_value(float current);
float get_current_value(uint8_t idu);
float get_temperature(temperature_t* p_temp);
bool get_i2c_temperature(temperature_i2c_t* p_temp);

void reload_idu_current(void);

void color_EN_EXIT_IRQ(color_t color);

char* get_sn(int number, char* buff);
uint32_t get_MSB_array_crc(uint8_t* array, size_t size);
uint32_t get_LSB_array_crc(uint8_t* array, size_t size);

extern const ntc_t NCP18WB473F10RB;

#endif
