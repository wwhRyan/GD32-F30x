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
#include "basicApp.h"
#include "gd32f30x.h"
#include "main.h"

extern const Uarter uart0_output;
extern temperature_t temperature[];

void ThreadSecondConsumer(void* pvParameters)
{
    xEventGroupWaitBits(sys_sig, (0x00000001 << sig_mcu_init_ok), pdFALSE, pdTRUE, 0xFFFF);
    ULOG_DEBUG("%s\n", __func__);

    // msg_t xQueue_eeprom_recv = { 0 };
    while (1) {
#if 0
        if (xQueueReceive(xQueue_eeprom, &(xQueue_eeprom_recv), (TickType_t)10) == pdPASS) {
            eeprom_block_write(&AT24C02D, xQueue_eeprom_recv.addr, (uint8_t *)xQueue_eeprom_recv.pData, xQueue_eeprom_recv.size);
            if (xQueue_eeprom_recv.idx == idx_eeprom_write) {
                // reset eeprom crc and get data to memory.
                clear_sig(sys_sig, sig_eeprom_write);
                eeprom_update_crc(&AT24C02D);
            }
        }
        vTaskDelay(200);
#endif
        for (int i = 0; i < sensor_num; i++) {
            get_temperature(&temperature[i]);
        }

        for (int i = 0; i < sensor_num; i++) {
            get_i2c_temperature(&temperature_i2c[i]);
        }

        vTaskDelay(500);
    }
}
