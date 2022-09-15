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
#include "main.h"
#include "gd32f30x.h"
#include "BoardInit.h"
#include "Common.h"
#include "ovp921.h"

#define EEPROM_ADDRESS 0xA0 // 8'h 0xA0

#define EEPROM_PAGE_NUM 8    // write max number
#define EEPROM_WRITE_TIME 10 // unit is ms

#define UNLOCK false
#define LOCK true

#define CONFIG_START_ADDR 0x00
#define LOG_START_ADDR 0x4096

typedef struct eeprom_model_t
{
    uint16_t i2c_addr; // 8'h
    uint8_t i2c_addr_type;
    void (*lock)(bool status);
    uint8_t page_size;
    uint8_t write_delay_time; // ms
    const SoftwareI2C *i2c;
} eeprom_model_t;

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
    union{
        uint32_t default_uint32;
        float default_float;
        char * default_string;
    };
} mem_t;

extern eeprom_t eeprom;

void eeprom_lock(bool lock);
bool eeprom_write(const eeprom_model_t *model, uint16_t addr, uint8_t data);
bool eeprom_block_write(const eeprom_model_t* model, uint16_t WriteAddr, uint8_t* data, uint16_t size);
uint8_t eeprom_read(const eeprom_model_t *model, uint8_t addr);
bool eeprom_block_read(const eeprom_model_t *model, uint16_t addr, uint8_t *data, uint16_t size);
void init_eeprom(const eeprom_model_t *model);
void eeprom_reset(void);
bool eeprom_update_crc(const eeprom_model_t *model);

bool memory_endian_conversion(void* pointer, size_t size);
#endif
