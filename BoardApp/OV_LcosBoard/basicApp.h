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

#include "i2c.h"
#include "main.h"
#include "gd32f30x.h"
#include "BoardInit.h"
#include "Common.h"
#include "ovp921.h"

#define EEPROM_ADDRESS 0xA0 // 8'h 0xA0
#define EEPROM_WRITE (EEPROM_ADDRESS | 0x00)
#define EEPROM_READ (EEPROM_ADDRESS | 0x01)

#define UNLOCK false
#define LOCK true

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

typedef struct eeprom_t
{
    uint32_t check_sum;
    uint32_t magic_num;
    float red;
    float green;
    float blue;
    uint32_t light_source_time;
} eeprom_t;

void laser_on(void);
void laser_off(void);
bool laser_set(int idx, float current);
float laser_get(int idx);
void laser_dac_set(float current);

uint8_t get_idu_value(float current);
float get_current_value(uint8_t idu);
float get_temperature(int adc_value);

extern eeprom_t eeprom;
void eeprom_lock(bool lock);
bool eeprom_write(uint8_t addr, uint8_t data);
uint8_t eeprom_read(uint8_t addr);
void init_eeprom(void);

#define EEPROM_SET(var, unit, byte_size)                                                                         \
    do                                                                                                           \
    {                                                                                                            \
        eeprom_lock(UNLOCK);                                                                                     \
                                                                                                                 \
        eeprom.unit = var;                                                                                       \
        ISoftwareI2CRegWrite(&ovp921_i2c, EEPROM_WRITE, offsetof(eeprom_t, unit),                                \
                             REG_ADDR_1BYTE, ((uint8_t *)&eeprom) + offsetof(eeprom_t, unit), byte_size, 0xFFFF); \
        eeprom_lock(LOCK);                                                                                       \
    } while (0)

void reload_idu_current(void);

void color_EN_EXIT_IRQ(color_t color);

#endif
