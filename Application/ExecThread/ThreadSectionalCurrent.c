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

void TaskUartDecode(void *pvParameters)
{
    while (1)
    {
        debug_printf("TaskUartDecode\r\n");
        vTaskDelay(500);
    }
}
