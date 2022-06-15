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

    while (1)
    {
        // debug_printf("TaskIdle\r\n");
        // debug_printf("TaskIdle min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(500);
        if (get_ovp921_status() == true && get_sig(sys_sig, at_lightsource) && !gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN))
        {
            laser_on();
            debug_printf("laser on\r\n");
        }
        else if ((get_ovp921_status() == false || !get_sig(sys_sig, at_lightsource)) && gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN))
        {
            laser_off();
            debug_printf("laser off\r\n");
        }
    }
}
