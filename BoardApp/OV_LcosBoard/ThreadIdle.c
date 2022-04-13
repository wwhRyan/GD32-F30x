/**
 * @file ThreadIdle.c
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

void TaskIdle(void *pvParameters)
{
    struct x
    {
        uint16_t a : 4;
        uint16_t b : 8;
        uint16_t c : 4;
    } test;

    test.a = 0xf;
    test.b = 0x0;
    test.c = 0xf;

    debug_printf("test is %#X\r\n", test);

    while (1)
    {
        // debug_printf("TaskIdle\r\n");
        // debug_printf("TaskIdle min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(500);
    }
}
