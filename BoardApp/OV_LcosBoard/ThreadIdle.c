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
#include "ovp921.h"
#include "basicApp.h"

void TaskIdle(void *pvParameters)
{
    struct x
    {
        uint16_t a : 4;
        uint16_t b : 8;
        uint16_t c : 4;
    } test;

    test.a = 0xA;
    test.b = 0x0;
    test.c = 0xC;

    debug_printf("test is %#X\r\n", test);

    for (int i = 0; i < 5; i++)
    {
        debug_printf("%ds\r\n", i);
        vTaskDelay(1000);
    }

    reload_idu_current();

    while (1)
    {
        // debug_printf("TaskIdle\r\n");
        // debug_printf("TaskIdle min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(500);
        if (get_ovp921_status() == true)
        {
            laser_on();
            debug_printf("laser on\r\n");
        }
        else
        {
            laser_off();
            debug_printf("laser off\r\n");
        }
    }
}
