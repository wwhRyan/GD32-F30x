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

#include "Common.h"
#include "basicApp.h"
#include "i2c.h"
#include "eeprom.h"
#include "BoardInit.h"
#include "ulog.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void eeprom_lock(bool lock);

eeprom_t eeprom;
const uint32_t eeprom_magic_number = 0x12344321;

const mem_t eeprom_mem[] = {
    { .default_uint32 = 0, .type = uint32_type, .idx = idx_check_sum, .pData = &eeprom.check_sum, .addr = offsetof(eeprom_t, check_sum) + CONFIG_START_ADDR, .size = sizeof(uint32_t) },
    { .default_uint32 = eeprom_magic_number, .type = uint32_type, .idx = idx_magic_num, .pData = &eeprom.magic_num, .addr = offsetof(eeprom_t, magic_num) + CONFIG_START_ADDR, .size = sizeof(uint32_t) },
    { .default_uint32 = 0, .type = uint32_type, .idx = idx_version, .pData = &eeprom.version, .addr = offsetof(eeprom_t, version) + CONFIG_START_ADDR, .size = sizeof(uint32_t) },
    { .default_float = 1.0, .type = float_type, .idx = idx_red, .pData = &eeprom.red, .addr = offsetof(eeprom_t, red) + CONFIG_START_ADDR, .size = sizeof(float) },
    { .default_float = 1.0, .type = float_type, .idx = idx_green, .pData = &eeprom.green, .addr = offsetof(eeprom_t, green) + CONFIG_START_ADDR, .size = sizeof(float) },
    { .default_float = 1.0, .type = float_type, .idx = idx_blue, .pData = &eeprom.blue, .addr = offsetof(eeprom_t, blue) + CONFIG_START_ADDR, .size = sizeof(float) },
    { .default_uint32 = 0, .type = uint32_type, .idx = idx_light_source_time, .pData = &eeprom.light_source_time, .addr = offsetof(eeprom_t, light_source_time) + CONFIG_START_ADDR, .size = sizeof(uint32_t) },
    { .default_string = "NULL", .type = string_type, .idx = idx_Sn_LightEngine, .pData = &eeprom.Sn_LightEngine, .addr = offsetof(eeprom_t, Sn_LightEngine) + CONFIG_START_ADDR, .size = sizeof(char) * 32 },
    { .default_string = "NULL", .type = string_type, .idx = idx_Sn_SourceLight, .pData = &eeprom.Sn_SourceLight, .addr = offsetof(eeprom_t, Sn_SourceLight) + CONFIG_START_ADDR, .size = sizeof(char) * 32 },
    { .default_string = "NULL", .type = string_type, .idx = idx_Sn_Projector, .pData = &eeprom.Sn_Projector, .addr = offsetof(eeprom_t, Sn_Projector) + CONFIG_START_ADDR, .size = sizeof(char) * 32 },
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

bool eeprom_update_crc(const eeprom_model_t* model)
{
    bool ret = true;
    eeprom_t temp = { 0 };

    ret &= ISoftwareI2CRegRead(model->i2c, model->i2c_addr, CONFIG_START_ADDR,
        model->i2c_addr_type, ((uint8_t*)&temp), sizeof(eeprom_t), 0xFFFF);

    uint32_t calc_check_sum = get_LSB_array_crc((uint8_t*)&temp.magic_num, sizeof(eeprom_t) - sizeof(uint32_t));
    // if (calc_check_sum != eeprom.check_sum)
    //     ULOG_WARNING("EEPROM_had_been_external_update\n");

    ret &= eeprom_block_write(model, CONFIG_START_ADDR, ((uint8_t*)&calc_check_sum), sizeof(uint32_t));

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

bool eeprom_write(const eeprom_model_t* model, uint16_t addr, uint8_t data)
{
    bool ret = true;

    if (model->lock != NULL)
        model->lock(UNLOCK);

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, addr,
        model->i2c_addr_type, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    vTaskDelay(model->write_delay_time);

    if (model->lock != NULL)
        model->lock(LOCK);

    return ret;
}

uint8_t eeprom_read(const eeprom_model_t* model, uint8_t addr)
{
    uint8_t data;
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    return data;
}

bool eeprom_block_write(const eeprom_model_t* model, uint16_t WriteAddr, uint8_t* data, uint16_t size)
{
    bool ret = true;
    model->lock(UNLOCK);
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
            if (count < NumOfSingle) {
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
    model->lock(LOCK);
    return ret;
}

bool eeprom_block_read(const eeprom_model_t* model, uint16_t addr, uint8_t* data, uint16_t size)
{
    bool ret = true;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type, data, size, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    vTaskDelay(model->write_delay_time);
    return ret;
}

//TODO: eeprom 写入的防掉电操作
/**
 * @brief Call in rtos task
 */
void init_eeprom(const eeprom_model_t* model)
{

    ISoftwareI2CRegRead(model->i2c, model->i2c_addr, CONFIG_START_ADDR,
        model->i2c_addr_type, ((uint8_t*)&eeprom), sizeof(eeprom_t), 0xFFFF);

    uint32_t calc_check_sum = get_LSB_array_crc((uint8_t*)&eeprom.magic_num, sizeof(eeprom_t) - sizeof(uint32_t));

    if (eeprom.magic_num != eeprom_magic_number || calc_check_sum != eeprom.check_sum) {
        ULOG_ERROR("EEPROM DATA RESET!\n");
        ULOG_DEBUG("eeprom check_sum = %08X!\n", eeprom.check_sum);
        ULOG_DEBUG("calc_check_sum = %08X!\n", calc_check_sum);
        eeprom_memory_reset();
        eeprom.check_sum = calc_check_sum;
        eeprom_block_write(model, CONFIG_START_ADDR, ((uint8_t*)&eeprom), sizeof(eeprom_t));
        eeprom_update_crc(model);
    } else {
        ULOG_INFO("Ok eeprom.\n");
    }
}
