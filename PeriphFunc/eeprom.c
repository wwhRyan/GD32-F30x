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

#include "basicApp.h"
#include "i2c.h"
#include "eeprom.h"

void eeprom_lock(bool lock);
extern const eeprom_model_t BL24C64A;

eeprom_t eeprom;
const uint32_t eeprom_magic_number = 0x12344321;

const unit_t eeprom_msg[] = {
    {.idx = idx_check_sum, .pData = &eeprom.check_sum, .addr = offsetof(eeprom_t, check_sum) + CONFIG_START_ADDR, .size = sizeof(uint32_t)},
    {.idx = idx_magic_num, .pData = &eeprom.magic_num, .addr = offsetof(eeprom_t, magic_num) + CONFIG_START_ADDR, .size = sizeof(uint32_t)},
    {.idx = idx_version, .pData = &eeprom.version, .addr = offsetof(eeprom_t, version) + CONFIG_START_ADDR, .size = sizeof(uint32_t)},
    {.idx = idx_red, .pData = &eeprom.red, .addr = offsetof(eeprom_t, red) + CONFIG_START_ADDR, .size = sizeof(float)},
    {.idx = idx_green, .pData = &eeprom.green, .addr = offsetof(eeprom_t, green) + CONFIG_START_ADDR, .size = sizeof(float)},
    {.idx = idx_blue, .pData = &eeprom.blue, .addr = offsetof(eeprom_t, blue) + CONFIG_START_ADDR, .size = sizeof(float)},
    {.idx = idx_light_source_time, .pData = &eeprom.light_source_time, .addr = offsetof(eeprom_t, light_source_time) + CONFIG_START_ADDR, .size = sizeof(uint32_t)},
    {.idx = idx_Sn_LightEngine, .pData = &eeprom.Sn_LightEngine, .addr = offsetof(eeprom_t, Sn_LightEngine) + CONFIG_START_ADDR, .size = sizeof(char) * 32},
    {.idx = idx_Sn_SourceLight, .pData = &eeprom.Sn_SourceLight, .addr = offsetof(eeprom_t, Sn_SourceLight) + CONFIG_START_ADDR, .size = sizeof(char) * 32},
    {.idx = idx_Sn_Projector, .pData = &eeprom.Sn_Projector, .addr = offsetof(eeprom_t, Sn_Projector) + CONFIG_START_ADDR, .size = sizeof(char) * 32},
};

/**
 * @brief eeprom lock function
 *
 * @param lock true: lock, false: unlock
 */
void eeprom_lock(bool lock)
{
    if (lock == false)
    {
        gpio_bit_set(EE_WP_PORT, EE_WP_PIN); // set WP pin to high, can write
        DelayUs(10);
    }
    else
    {
        DelayUs(10);
        gpio_bit_reset(EE_WP_PORT, EE_WP_PIN); // set WP pin to low, can't write
    }
}

/**
 * @brief Call before rtos task
 */
void init_eeprom(const eeprom_model_t *model)
{

    ISoftwareI2CRegRead(model->i2c, model->i2c_addr, CONFIG_START_ADDR,
                        model->i2c_addr_type, ((uint8_t *)&eeprom), sizeof(eeprom_t), 0xFFFF);

    if (eeprom.magic_num != eeprom_magic_number)
    {
        eeprom_lock(UNLOCK);

        printf("reset eeprom!\n");
        eeprom.magic_num = eeprom_magic_number;
        eeprom.red = 1.00;
        eeprom.green = 0.80;
        eeprom.blue = 0.623;
        eeprom.light_source_time = 0;

        // todo
        for (size_t i = 0; i < sizeof(eeprom_t); i++)
        {
            ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, i,
                                 model->i2c_addr_type, ((uint8_t *)&eeprom) + i, 1, 0xFFFF);
            DelayMs(10);
        }
        eeprom_lock(LOCK);
    }
    else
    {
        printf("Ok eeprom.\n");
    }
}

bool eeprom_write(const eeprom_model_t *model, uint16_t addr, uint8_t data)
{
    bool ret = true;
    model->lock(UNLOCK);

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegWrite(model->i2c, model->i2c_addr, addr,
                               model->i2c_addr_type, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    vTaskDelay(model->write_delay_time);

    model->lock(LOCK);

    return ret;
}

uint8_t eeprom_read(const eeprom_model_t *model, uint8_t addr)
{
    uint8_t data;
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type, &data, 1, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    return data;
}

bool eeprom_block_write(const eeprom_model_t *model, uint16_t addr, uint8_t *data, uint16_t size)
{
    bool ret = true;

    model->lock(UNLOCK);
    if (size > model->page_size)
    {
        int cnt = size / model->page_size;
        for (size_t i = 0; i < cnt; i++)
        {
            xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
            ret = ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type,
                                      data + i * model->page_size, model->page_size, 0xFFFF);
            xSemaphoreGive(i2c_Semaphore);
            vTaskDelay(model->write_delay_time);
        }
        int left_cnt = size % model->page_size;
        xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
        ret = ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type,
                                  data + cnt * model->page_size, left_cnt, 0xFFFF);
        xSemaphoreGive(i2c_Semaphore);
        vTaskDelay(model->write_delay_time);
    }
    else
    {
        xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
        ret = ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type, data, size, 0xFFFF);
        xSemaphoreGive(i2c_Semaphore);
        vTaskDelay(model->write_delay_time);
    }
    model->lock(LOCK);
    return ret;
}

bool eeprom_block_read(const eeprom_model_t *model, uint16_t addr, uint8_t *data, uint16_t size)
{
    bool ret = true;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegRead(model->i2c, model->i2c_addr, addr, model->i2c_addr_type, data, size, 0xFFFF);
    xSemaphoreGive(i2c_Semaphore);
    vTaskDelay(model->write_delay_time);
    return ret;
}
