/**
 * @file eeprom.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-07-18
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "i2c.h"
#include "gd32f30x.h"
#include "Common.h"
#include <stdint.h>

#define UNLOCK false
#define LOCK true
#define EEPROM_SAVE_BACKUP_NUM 2
#define EEPROM_0_ADDR 0x000
#define EEPROM_1_ADDR 0x400
typedef struct i2c_sensor_t {
    uint16_t i2c_addr; // 8'h
    uint8_t i2c_addr_type;
    uint8_t write_delay_time; // ms
    const SoftwareI2C* i2c;
    uint8_t page_size;
} i2c_sensor_t;

typedef struct eeprom_t {
    uint32_t check_sum; /* addr 0x00 */
    uint32_t magic_num; /* addr 0x04 */
    uint32_t version; /* addr 0x08 */
    float red; /* addr 0x0c */
    float green; /* addr 0x10 */
    float blue; /* addr 0x14 */
    uint32_t light_source_time; /* addr 0x18 */
    char Sn_LightEngine[32]; /* addr 0x1c */
    char Sn_SourceLight[32]; /* addr 0x3C */
    char Sn_Projector[32]; /* addr 0x5C */
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
    idx_eeprom_number,
    /* for msg handle */
    idx_eeprom_write,
} eeprom_idx_t;

typedef enum type_t{
    uint32_type,
    float_type,
    string_type,
}type_t;

#pragma anon_unions
typedef struct mem_t
{
    uint8_t idx;
    void *pData;
    uint16_t addr;
    uint8_t size;
    type_t type;
    uint32_t* p_check;
    union{
        uint32_t default_uint32;
        float default_float;
        char * default_string;
    };
} mem_t;

extern eeprom_t eeprom;

void eeprom_lock(bool lock);
bool eeprom_block_write(const i2c_sensor_t* model, const mem_t* data, bool real_time);
void init_eeprom(const i2c_sensor_t *model);
void eeprom_block_clear(void);
void eeprom_reset(void);
bool eeprom_update_crc(const i2c_sensor_t *model);
bool eeprom_block_update_crc(const i2c_sensor_t* model, const mem_t* block);

bool memory_endian_conversion(void* pointer, size_t size);
#endif
