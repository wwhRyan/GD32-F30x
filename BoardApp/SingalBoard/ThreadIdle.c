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

    test.a = 0b1111;
    test.b = 0b00000000;
    test.c = 0b1111;

    debug_printf("test is %#X\r\n", test);

    while (1)
    {
        // debug_printf("TaskIdle\r\n");
        // debug_printf("TaskIdle min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        if (gpio_output_bit_get(LED_PORT, LED_PIN) == RESET)
        {
            gpio_bit_set(LED_PORT, LED_PIN);
        }
        else
        {
            gpio_bit_reset(LED_PORT, LED_PIN);
        }
        vTaskDelay(1000);

        // gpio_bit_set(LED_PORT, LED_PIN);
    }
}
