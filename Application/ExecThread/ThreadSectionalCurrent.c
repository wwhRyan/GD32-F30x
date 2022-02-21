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
#include "ExecThread.h"

extern const Uarter uart0_debug;

void TaskSectionCurrent(void *pvParameters)
{
    while (1)
    {
        // debug_printf("TaskSectionCurrent\r\n");
        // vTaskDelay(500);
        if (0 != GetRxlen(&uart0_debug))
        {
            debug_printf("%s\r\n", GetRxData(&uart0_debug));
            ClearRxData(&uart0_debug);
        }
        vTaskDelay(10);
    }
}
