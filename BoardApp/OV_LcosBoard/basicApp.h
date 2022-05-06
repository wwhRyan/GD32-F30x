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

typedef enum color_current_t{
    RED,
    GREEN,
    BLUE,
}laser_color_t;

void laser_on(void);
void laser_off(void);
bool laser_set(int idx, float current);
float laser_get(int idx);

uint8_t get_idu_value(float current);
float get_current_value(uint8_t idu);

bool eeprom_write(uint8_t addr, uint8_t data);
uint8_t eeprom_read(uint8_t addr);

#endif
