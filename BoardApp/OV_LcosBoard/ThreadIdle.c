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

    for (int i = 0; i < 10; i++)
    {
        debug_printf("%ds\r\n", i);
        vTaskDelay(1000);
    }

    uint8_t red_idu_value = eeprom_read((int)RED);
    uint8_t green_idu_value = eeprom_read((int)GREEN);
    uint8_t blue_idu_value = eeprom_read((int)BLUE);

    if (red_idu_value <= 127 && red_idu_value >= 21)
    {
        laser_set(RED, get_current_value(red_idu_value));
        debug_printf("RED: %f\r\n", get_current_value(red_idu_value));
    }
    else
    {
        laser_set(RED, 1.0);
        debug_printf("laser_set RED is %f\r\n", 1.0);
    }

    if (green_idu_value <= 127 && green_idu_value >= 21)
    {
        laser_set(GREEN, get_current_value(green_idu_value));
        debug_printf("GREEN: %f\r\n", get_current_value(green_idu_value));
    }
    else
    {
        laser_set(GREEN, 0.8);
        debug_printf("laser_set GREEN is %f\r\n", 0.8);
    }

    if (blue_idu_value <= 127 && blue_idu_value >= 21)
    {
        laser_set(BLUE, get_current_value(blue_idu_value));
        debug_printf("BLUE: %f\r\n", get_current_value(blue_idu_value));
    }
    else
    {
        laser_set(BLUE, 0.623);
        debug_printf("laser_set BLUE is %f\r\n", 0.623);
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
