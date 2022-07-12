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

extern const SoftwareI2C ovp921_i2c;

void TaskIdle(void *pvParameters)
{
    set_sig(sys_sig, sig_lightsource, true);
    set_sig(sys_sig, sig_system, true);
    set_sig(sys_sig, sig_light_status, false);
    set_sig(sys_sig, sig_ovp921_status, false);
    while (1)
    {

        // debug_printf("TaskIdle\r\n");
        // debug_printf("TaskIdle min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(500);
        if (get_sig(sys_sig, sig_system))
            set_sig(sys_sig, sig_ovp921_status, get_ovp921_status());

        set_sig(sys_sig, sig_light_status, gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN));

        if (get_sig(sys_sig, sig_ovp921_status) == true && get_sig(sys_sig, sig_lightsource) && !get_sig(sys_sig, sig_light_status))
        {
            laser_on();
            debug_printf("laser on\r\n");
        }
        else if ((get_sig(sys_sig, sig_ovp921_status) == false || !get_sig(sys_sig, sig_lightsource)) && get_sig(sys_sig, sig_light_status))
        {
            laser_off();
            debug_printf("laser off\r\n");
        }

        if (gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN) &&is_one_second() == true)
        {
            EEPROM_SET(eeprom.light_source_time + 1, light_source_time, sizeof(uint32_t));
        }

        while (get_sig(sys_sig, sig_system) == false) // system is off do nothing.
        {
            vTaskDelay(500);
        }
    }
}
