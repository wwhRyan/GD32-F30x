/**
 * @file eeprom.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-07-18
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#include "eeprom.h"
#include "BoardInit.h"
#include "Common.h"
#include "basicApp.h"
#include "i2c.h"
#include "ulog.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void eeprom_lock(bool lock);

eeprom_t eeprom;
uint32_t eeprom_block_check_buf[idx_eeprom_number] = { 0 };
const uint32_t eeprom_magic_number = 0x12344321;

const mem_t eeprom_mem[] = {
    { .default_uint32 = 0, .type = uint32_type, .idx = idx_check_sum, .pData = &eeprom.check_sum, .addr = offsetof(eeprom_t, check_sum), .size = sizeof(uint32_t), .p_check = &eeprom_block_check_buf[idx_check_sum] },
    { .default_uint32 = eeprom_magic_number, .type = uint32_type, .idx = idx_magic_num, .pData = &eeprom.magic_num, .addr = offsetof(eeprom_t, magic_num), .size = sizeof(uint32_t), .p_check = &eeprom_block_check_buf[idx_magic_num] },
    { .default_uint32 = 0, .type = uint32_type, .idx = idx_version, .pData = &eeprom.version, .addr = offsetof(eeprom_t, version), .size = sizeof(uint32_t), .p_check = &eeprom_block_check_buf[idx_version] },
    { .default_float = 1.0, .type = float_type, .idx = idx_red, .pData = &eeprom.red, .addr = offsetof(eeprom_t, red), .size = sizeof(float), .p_check = &eeprom_block_check_buf[idx_red] },
    { .default_float = 1.0, .type = float_type, .idx = idx_green, .pData = &eeprom.green, .addr = offsetof(eeprom_t, green), .size = sizeof(float), .p_check = &eeprom_block_check_buf[idx_green] },
    { .default_float = 1.0, .type = float_type, .idx = idx_blue, .pData = &eeprom.blue, .addr = offsetof(eeprom_t, blue), .size = sizeof(float), .p_check = &eeprom_block_check_buf[idx_blue] },
    { .default_uint32 = 0, .type = uint32_type, .idx = idx_light_source_time, .pData = &eeprom.light_source_time, .addr = offsetof(eeprom_t, light_source_time), .size = sizeof(uint32_t), .p_check = &eeprom_block_check_buf[idx_light_source_time] },
    { .default_string = "NULL", .type = string_type, .idx = idx_Sn_LightEngine, .pData = &eeprom.Sn_LightEngine, .addr = offsetof(eeprom_t, Sn_LightEngine), .size = sizeof(char) * 32, .p_check = &eeprom_block_check_buf[idx_Sn_LightEngine] },
    { .default_string = "NULL", .type = string_type, .idx = idx_Sn_SourceLight, .pData = &eeprom.Sn_SourceLight, .addr = offsetof(eeprom_t, Sn_SourceLight), .size = sizeof(char) * 32, .p_check = &eeprom_block_check_buf[idx_Sn_SourceLight] },
    { .default_string = "NULL", .type = string_type, .idx = idx_Sn_Projector, .pData = &eeprom.Sn_Projector, .addr = offsetof(eeprom_t, Sn_Projector), .size = sizeof(char) * 32, .p_check = &eeprom_block_check_buf[idx_Sn_Projector] },
};

/**
 * @brief eeprom lock function
 *
 * @param lock true: lock, false: unlock
 */
void eeprom_lock(bool lock)
{
    if (lock == false) {
        gpio_bit_set(EE_WP_PORT, EE_WP_PIN); // set WP pin to high, can write
        DelayUs(10);
    } else {
        DelayUs(10);
        gpio_bit_reset(EE_WP_PORT, EE_WP_PIN); // set WP pin to low, can't write
    }
}

/**
 * @brief 如果EEPROM存储的内容的校验值不同于内存的校验值，将eeprom内存覆盖,并更新EEPROM储存的校验值
 *
 * @param model
 * @return true
 * @return false
 */
bool eeprom_update_crc(const i2c_sensor_t* model)
{
    bool ret = true;
    eeprom_t temp = { 0 };

    ret &= i2c_muti_read(model, EEPROM_0_ADDR, ((uint8_t*)&temp), sizeof(eeprom_t));

    uint32_t calc_check_sum = get_LSB_array_crc((uint8_t*)&temp.magic_num, sizeof(eeprom_t) - sizeof(uint32_t));

    if (calc_check_sum != eeprom.check_sum) {
        temp.check_sum = calc_check_sum;
        memcpy(&eeprom, &temp, sizeof(eeprom_t));
        eeprom_lock(UNLOCK);

        ret &= i2c_muti_write(model, EEPROM_0_ADDR, ((uint8_t*)&calc_check_sum), sizeof(uint32_t));
        ret &= i2c_muti_write(model, EEPROM_1_ADDR, ((uint8_t*)&calc_check_sum), sizeof(uint32_t));
        eeprom_lock(LOCK);

        ULOG_INFO("EEPROM_had_been_external_update\n");
    }

    return ret;
}

bool eeprom_block_update_crc(const i2c_sensor_t* model, const mem_t* block)
{
    bool ret = true;
    eeprom_t temp = { 0 };

    ret &= i2c_muti_read(model, EEPROM_0_ADDR, ((uint8_t*)&temp), sizeof(eeprom_t));

    uint32_t calc_check_sum = get_LSB_array_crc((uint8_t*)&temp.magic_num, sizeof(eeprom_t) - sizeof(uint32_t));

    if (calc_check_sum != *(block->p_check)) {
        temp.check_sum = calc_check_sum;
        memcpy(&eeprom, &temp, sizeof(eeprom_t));
        eeprom_lock(UNLOCK);

        ret &= i2c_muti_write(model, EEPROM_0_ADDR, ((uint8_t*)&calc_check_sum), sizeof(uint32_t));
        ret &= i2c_muti_write(model, EEPROM_1_ADDR, ((uint8_t*)&calc_check_sum), sizeof(uint32_t));
        eeprom_lock(LOCK);

        ULOG_INFO("EEPROM_had_been_external_update\n");
    }

    return ret;
}

void eeprom_memory_reset(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(eeprom_mem); i++) {
        switch (eeprom_mem[i].type) {
        case uint32_type:
            *((uint32_t*)eeprom_mem[i].pData) = eeprom_mem[i].default_uint32;
            break;

        case float_type:
            *((float*)eeprom_mem[i].pData) = eeprom_mem[i].default_float;
            break;

        case string_type:
            memset(eeprom_mem[i].pData, 0, eeprom_mem[i].size);
            strcpy((char*)eeprom_mem[i].pData, eeprom_mem[i].default_string);
            break;

        default:
            break;
        }
    }
}

bool eeprom_block_write(const i2c_sensor_t* model, const mem_t* data, bool real_time)
{
    bool ret = true;
    memcpy(((uint8_t*)&eeprom) + data->addr, data->pData, data->size);
    eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
    *(data->p_check) = eeprom.check_sum;
    if (real_time == true) {
        eeprom_lock(UNLOCK);
        ret &= i2c_muti_write(model, data->addr + EEPROM_0_ADDR, (uint8_t*)data->pData, data->size);
        ret &= i2c_muti_write(model, EEPROM_0_ADDR, (uint8_t*)&eeprom.check_sum, eeprom_mem[idx_check_sum].size);
        ret &= i2c_muti_write(model, data->addr + EEPROM_1_ADDR, (uint8_t*)data->pData, data->size);
        ret &= i2c_muti_write(model, EEPROM_1_ADDR, (uint8_t*)&eeprom.check_sum, eeprom_mem[idx_check_sum].size);

        eeprom_lock(LOCK);
        if (data->idx > idx_eeprom_number) {
            clear_sig(sys_sig, sig_eeprom_write);
            eeprom_block_update_crc(model, data);
        }
    } else {
        xQueueSend(xQueue_eeprom, data, 100);
    }

    return ret;
}

/**
 * @brief Call in rtos task
 */
void init_eeprom(const i2c_sensor_t* model)
{
    eeprom_t temp_buff[EEPROM_SAVE_BACKUP_NUM] = { 0 };
    uint32_t calc_check_sum[EEPROM_SAVE_BACKUP_NUM] = { 0 };
    for (int i = 0; i < EEPROM_SAVE_BACKUP_NUM; i++) {
        i2c_muti_read(model, EEPROM_0_ADDR + i * 0x400, ((uint8_t*)&temp_buff[i]), sizeof(eeprom_t));
        calc_check_sum[i] = get_LSB_array_crc((uint8_t*)&temp_buff[i].magic_num, sizeof(eeprom_t) - sizeof(uint32_t));
        if (temp_buff[i].magic_num == eeprom_magic_number && temp_buff[i].check_sum == calc_check_sum[i]) {
            memcpy(&eeprom, &temp_buff[i], sizeof(eeprom_t));
            ULOG_INFO("eeprom check ok! NUM = %d\n", i);
            return;
        }
    }

    ULOG_ERROR("EEPROM DATA RESET!\n");
    eeprom_memory_reset();
    eeprom_lock(UNLOCK);
    i2c_muti_write(model, EEPROM_0_ADDR, ((uint8_t*)&eeprom), sizeof(eeprom_t));
    i2c_muti_write(model, EEPROM_1_ADDR, ((uint8_t*)&eeprom), sizeof(eeprom_t));
    eeprom_lock(LOCK);
    eeprom_update_crc(model);
}

void eeprom_block_clear(void)
{
    eeprom_lock(UNLOCK);
    i2c_write(&BL24C64A, eeprom_mem[idx_check_sum].addr + EEPROM_0_ADDR, 0);
    i2c_write(&BL24C64A, eeprom_mem[idx_check_sum].addr + EEPROM_1_ADDR, 0);
    eeprom_lock(LOCK);

    init_eeprom(&BL24C64A);
}
