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
    uint32_t version;
    float red;
    float green;
    float blue;
    uint32_t light_source_time;
    char Sn_LightEngine[32];
    char Sn_SourceLight[32];
    char Sn_Projector[32];
} eeprom_t;

typedef enum eeprom_idx_t
{
    idx_check_sum,
    idx_magic_num,
    idx_version,
    idx_red,
    idx_green,
    idx_blue,
    idx_light_source_time,
    idx_Sn_LightEngine,
    idx_Sn_SourceLight,
    idx_Sn_Projector,
} eeprom_idx_t;

typedef struct unit_t
{
    uint8_t idx;
    void *pData;
    uint16_t addr;
    uint8_t size;
} unit_t;

void laser_on(void);
void laser_off(void);
bool laser_set(int idx, float current);
void laser_dac_set(float current);

uint8_t get_idu_value(float current);
float get_current_value(uint8_t idu);
float get_temperature(int adc_value);

extern eeprom_t eeprom;
void eeprom_lock(bool lock);
bool eeprom_write(uint16_t addr, uint8_t *data, uint16_t size);
uint8_t eeprom_read(uint8_t addr);
void init_eeprom(void);

#define EEPROM_SET(var, unit, byte_size)                                                                              \
    do                                                                                                                \
    {                                                                                                                 \
        if (eeprom.unit != var)                                                                                       \
        {                                                                                                             \
            eeprom_lock(UNLOCK);                                                                                      \
            xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);                                                        \
            eeprom.unit = var;                                                                                        \
            ISoftwareI2CRegWrite(&ovp921_i2c, EEPROM_WRITE, offsetof(eeprom_t, unit),                                 \
                                 REG_ADDR_1BYTE, ((uint8_t *)&eeprom) + offsetof(eeprom_t, unit), byte_size, 0xFFFF); \
            xSemaphoreGive(i2c_Semaphore);                                                                            \
            eeprom_lock(LOCK);                                                                                        \
        }                                                                                                             \
    } while (0)

void reload_idu_current(void);

void color_EN_EXIT_IRQ(color_t color);

char *get_sn(int number, char *buff);
uint32_t get_array_crc(uint8_t *array, size_t size);

#endif
