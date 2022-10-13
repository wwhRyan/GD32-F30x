/**
 * @file ThreadSectionalCurrent.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-02-14
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#include "BoardInit.h"
#include "Common.h"
#include "basicApp.h"
#include "eeprom.h"
#include "gd32f30x.h"
#include "main.h"
#include "utils.h"

extern const Uarter uart0_output;
extern temperature_t temperature[];
extern const mem_t eeprom_mem[];

void ThreadSecondConsumer(void* pvParameters)
{
    xEventGroupWaitBits(sys_sig, (0x00000001 << sig_mcu_init_ok), pdFALSE, pdTRUE, 0xFFFF);
    ULOG_DEBUG("%s\n", __func__);

    mem_t recv_block = { 0 };
    while (1) {
        if (xQueueReceive(xQueue_eeprom, &(recv_block), (TickType_t)10) == pdPASS) {
            eeprom_lock(UNLOCK);
            // TODO: add checksum modify, write the recv_block.p_check 以防断电
            i2c_muti_write(&BL24C64A, recv_block.addr + EEPROM_0_ADDR, (uint8_t*)recv_block.pData, recv_block.size);
            i2c_muti_write(&BL24C64A, EEPROM_0_ADDR, (uint8_t*)recv_block.p_check, eeprom_mem[idx_check_sum].size);
            i2c_muti_write(&BL24C64A, recv_block.addr + EEPROM_1_ADDR, (uint8_t*)recv_block.pData, recv_block.size);
            i2c_muti_write(&BL24C64A, EEPROM_1_ADDR, (uint8_t*)recv_block.p_check, eeprom_mem[idx_check_sum].size);

            eeprom_lock(LOCK);

            if (recv_block.idx > idx_eeprom_number) {
                // reset eeprom crc and get data to memory.
                clear_sig(sys_sig, sig_eeprom_write);
                eeprom_block_update_crc(&BL24C64A, (const mem_t*)&recv_block);
            }
        }

        if (get_sig(sys_sig, sig_system) == true) {
            /* The adc cannot know whether the connection is abnormal */
            for (int i = 0; i < sensor_num; i++) {
                get_temperature(&temperature[i]);
                if (temperature[i].temperature < 0) {
                    switch (i) {
                    case blue_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("blue_sensor NTC not connect\n"));
                        break;
                    case red_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("red_sensor NTC not connect\n"));
                        break;
                    case green_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("green_sensor NTC not connect\n"));
                        break;
                    case lcos_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("lcos_sensor NTC not connect\n"));
                        break;
                    case evn_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("evn_sensor NTC not connect\n"));
                        break;
                    default:
                        E_assert(0);
                        break;
                    }
                }
            }
            for (int i = 0; i < sensor_num; i++) {
                if (get_i2c_temperature(&temperature_i2c[i]) == false) {
                    switch (i) {
                    case blue_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("blue_sensor I2C not connect\n"));
                        break;
                    case red_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("red_sensor I2C not connect\n"));
                        break;
                    case green_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("green_sensor I2C not connect\n"));
                        break;
                    case lcos_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("lcos_sensor I2C not connect\n"));
                        break;
                    case evn_sensor:
                        EXCUTE_ONCE(ULOG_ERROR("evn_sensor I2C not connect\n"));
                        break;
                    default:
                        E_assert(0);
                        break;
                    }
                }
            }
        }

        vTaskDelay(500);
    }
}
