/**
 * @file save_data.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-06-15
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#include "save_data.h"
#include "i2c.h"
#include "basicApp.h"
#include "main.h"

nv_data_t nv_data;
extern const SoftwareI2C ovp921_i2c;
const uint32_t magic_number = 0x12345678;

bool check_nv_data(nv_data_t *p_nv_data)
{
    /* reset the CRC data register and calculate the CRC of the value */
    crc_data_register_reset();
    uint32_t checksum = crc_block_data_calculate((const uint32_t *)&p_nv_data->magic_num, ((uint32_t)sizeof(nv_data_t) - sizeof(uint32_t)) / sizeof(uint32_t));
    // uint32_t checksum = crc_block_data_calculate((const uint32_t *)&p_nv_data->magic_num, 1);
    if (p_nv_data->check_sum == checksum)
        return true;
    else
    {
        p_nv_data->check_sum = checksum;
        printf("check sum no right.\n");
        return false;
    }
}

void save_nv_data_to_eeprom(nv_data_t *p_nv_data)
{
    eeprom_lock(UNLOCK);

    for (size_t i = 0; i < sizeof(nv_data_t); i++)
    {
        ISoftwareI2CRegWrite(&ovp921_i2c, EEPROM_WRITE, i,
                             REG_ADDR_1BYTE, ((uint8_t *)p_nv_data) + i, 1, 0xFFFF);
        DelayMs(10);
    }

    eeprom_lock(LOCK);
}

void read_eeprom_to_nv_data(nv_data_t *p_nv_data)
{
    eeprom_lock(UNLOCK);

    // for (size_t i = 0; i < sizeof(nv_data_t); i++)
    // {
    //     ISoftwareI2CRegRead(&ovp921_i2c, EEPROM_WRITE, i,
    //                         REG_ADDR_1BYTE, ((uint8_t *)p_nv_data) + i, 1, 0xFFFF);
    // }

    ISoftwareI2CRegRead(&ovp921_i2c, EEPROM_WRITE, 0x00,
                        REG_ADDR_1BYTE, ((uint8_t *)p_nv_data), sizeof(nv_data_t), 0xFFFF);

    eeprom_lock(LOCK);
}

void init_nv_data()
{
    read_eeprom_to_nv_data(&nv_data);
    printf("read nv_data checksum = %#x\n", nv_data.check_sum);
    if (!check_nv_data(&nv_data))
    {
        factary_reset_nv_data(&nv_data);
        save_nv_data_to_eeprom(&nv_data);

        printf("factary reset nv data\r\n");
    }
}

void update_nv_data()
{
    if (!check_nv_data(&nv_data))
    {
        save_nv_data_to_eeprom(&nv_data);
        debug_printf("nv_data updated %#x\r\n", nv_data.check_sum);
    }
}
