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

#include "main.h"
#include "gd32f30x.h"
#include "BoardInit.h"
#include "Common.h"

typedef enum color_t
{
    RED,
    GREEN,
    BLUE,
    color_num,
} color_t;

typedef struct ntc_t
{
    uint16_t B;
    uint16_t normal_R;
    uint16_t divided_voltage_R;
    bool is_pull_up; // true: divided_voltage_R pull up, false: divided_voltage_R pull down
} ntc_t;

void laser_on(void);
void laser_off(void);
bool laser_set(int idx, float current);
float laser_get(int idx);
void laser_dac_set(float current);

uint8_t get_idu_value(float current);
float get_current_value(uint8_t idu);
float get_temperature(int adc_value);

bool eeprom_write(uint8_t addr, uint8_t data);
uint8_t eeprom_read(uint8_t addr);

void color_EN_EXIT_IRQ(color_t color);

#endif
