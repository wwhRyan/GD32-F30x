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
extern const eeprom_model_t BL24C64A;

#define QUEUE_EEPROM_NUM 5
QueueHandle_t xQueue_eeprom = NULL;

void ThreadSecondConsumer(void *pvParameters)
{
    xQueue_eeprom = xQueueCreate(QUEUE_EEPROM_NUM, sizeof(unit_t));
    E_assert(xQueue_eeprom);

    unit_t xQueue_eeprom_recv = {0};
    while (1)
    {
        if (xQueueReceive(xQueue_eeprom, &(xQueue_eeprom_recv), (TickType_t)10) == pdPASS)
        {
            eeprom_block_write(&BL24C64A, xQueue_eeprom_recv.addr, (uint8_t *)xQueue_eeprom_recv.pData, xQueue_eeprom_recv.size);
        }
        vTaskDelay(200);
    }
}
