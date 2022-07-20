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

#include "main.h"
#include "gd32f30x.h"
#include "BoardInit.h"
#include "ovp921.h"
#include "basicApp.h"

extern const Uarter uart0_output;

#define EEPROM_W_QUEUE_LENGTH 5
QueueHandle_t xQueue_eeprom = NULL;

void ThreadSecondConsumer(void *pvParameters)
{
    xQueue_eeprom = xQueueCreate(EEPROM_W_QUEUE_LENGTH, sizeof(msg_t));
    E_assert(xQueue_eeprom);

    msg_t xQueue_eeprom_recv = {0};
    while (1)
    {
        if (xQueueReceive(xQueue_eeprom, &(xQueue_eeprom_recv), (TickType_t)10) == pdPASS)
        {
            eeprom_block_write(&BL24C64A, xQueue_eeprom_recv.addr, (uint8_t *)xQueue_eeprom_recv.pData, xQueue_eeprom_recv.size);
            if (xQueue_eeprom_recv.idx == idx_eeprom_write)
            {
                // reset eeprom crc and get data to memory.
                clear_sig(sys_sig, sig_eeprom_write);
                eeprom_update_crc(&BL24C64A);
            }
        }
        vTaskDelay(200);
    }
}
