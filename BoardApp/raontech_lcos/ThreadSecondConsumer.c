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

void ThreadSecondConsumer(void* pvParameters)
{
    ULOG_DEBUG("%s\n", __func__);

    // msg_t xQueue_eeprom_recv = { 0 };
    while (1) {
#if 0
        if (xQueueReceive(xQueue_eeprom, &(xQueue_eeprom_recv), (TickType_t)10) == pdPASS) {
            // eeprom_block_write(&BL24C64A, xQueue_eeprom_recv.addr, (uint8_t *)xQueue_eeprom_recv.pData, xQueue_eeprom_recv.size);
            if (xQueue_eeprom_recv.idx == idx_eeprom_write) {
                // reset eeprom crc and get data to memory.
                clear_sig(sys_sig, sig_eeprom_write);
                eeprom_update_crc(&BL24C64A);
            }
        }
        vTaskDelay(200);
#endif
        vTaskDelay(2000);
    }
}
