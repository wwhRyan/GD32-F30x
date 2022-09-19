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

void ThreadSecondConsumer(void* pvParameters)
{
    xEventGroupWaitBits(sys_sig, (0x00000001 << sig_mcu_init_ok), pdFALSE, pdTRUE, 0xFFFF);
    ULOG_DEBUG("%s\n", __func__);

    mem_t xQueue_eeprom_recv = { 0 };
    while (1) {
        if (xQueueReceive(xQueue_eeprom, &(xQueue_eeprom_recv), (TickType_t)10) == pdPASS) {
            eeprom_block_write(&BL24C64A, xQueue_eeprom_recv.addr, (uint8_t*)xQueue_eeprom_recv.pData, xQueue_eeprom_recv.size);
            if (xQueue_eeprom_recv.idx == idx_eeprom_write) {
                // reset eeprom crc and get data to memory.
                clear_sig(sys_sig, sig_eeprom_write);
            }
            eeprom_update_crc(&BL24C64A);
        }

        if (get_sig(sys_sig, sig_system) == true) {
            /* The adc cannot know whether the connection is abnormal */
            for (int i = 0; i < sensor_num; i++) {
                get_temperature(&temperature[i]);
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
