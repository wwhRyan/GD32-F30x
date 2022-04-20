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

    test.a = 0xf;
    test.b = 0x0;
    test.c = 0xf;

    debug_printf("test is %#X\r\n", test);

    laser_set(RED, 1.0);
    laser_set(GREEN, 0.8);
    laser_set(BLUE, 0.623);
    debug_printf("laser_set RED is %f\r\n", 1.0);
    debug_printf("laser_set GREEN is %f\r\n", 0.8);
    debug_printf("laser_set BLUE is %f\r\n", 0.623);

    for (int i = 0; i < 15; i++)
    {
        debug_printf("%ds\r\n", i);
        vTaskDelay(1000);
    }
    laser_on();
    debug_printf("laser on\r\n");

    while (1)
    {
        // debug_printf("TaskIdle\r\n");
        // debug_printf("TaskIdle min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(500);
    }
}
