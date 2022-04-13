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

extern const Uarter uart0_output;

void TaskSectionCurrent(void *pvParameters)
{
    while (1)
    {
#if 0
        debug_printf("TaskSectionCurrent\r\n");
        vTaskDelay(500);
#endif
#if 0
        if (0 != GetRxlen(&uart0_output))
        {
            debug_printf("%s\r\n", GetRxData(&uart0_output));
            ClearRxData(&uart0_output);
        }
#endif
        vTaskDelay(500);
        show_test_pattern();
    }
}
