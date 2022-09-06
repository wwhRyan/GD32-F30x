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

#include "BoardInit.h"
#include "basicApp.h"
#include "gd32f30x.h"
#include "main.h"
#include <stdint.h>

extern const SoftwareI2C raontech_i2c;
extern const mem_t eeprom_mem[];
int cw_speed = 25;

void ThreadFirstConsumer(void* pvParameters)
{
    xEventGroupWaitBits(sys_sig, (0x00000001 << sig_mcu_init_ok), pdFALSE, pdTRUE, 0xFFFF);
    ULOG_DEBUG("%s\n", __func__);

    while (1) {
#if 0
        // ULOG_DEBUG("ThreadFirstConsumer\r\n");
        // ULOG_DEBUG("ThreadFirstConsumer min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(500);
        if (get_sig(sys_sig, sig_system))
            set_sig(sys_sig, sig_rdc200a_status, get_ovp921_status());

        set_sig(sys_sig, sig_light_status, gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN));

        if (get_sig(sys_sig, sig_rdc200a_status) == true && get_sig(sys_sig, sig_lightsource) && !get_sig(sys_sig, sig_light_status)) {
            laser_on();
            ULOG_INFO("laser on\r\n");
        } else if ((get_sig(sys_sig, sig_rdc200a_status) == false || !get_sig(sys_sig, sig_lightsource)) && get_sig(sys_sig, sig_light_status)) {
            laser_off();
            ULOG_INFO("laser off\r\n");
        }

        if (gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN) && is_one_second() == true) {
            eeprom.light_source_time += 1;
            xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_light_source_time], (TickType_t)10);
        }

        extern const fan_timer_config_t cw_wheel_pwm;
        int error = (7200 - Get_fan_timer_FG(&cw_wheel_fg) * 30);
        cw_speed = cw_speed + error / 50;
        Set_fan_timer_pwm(&cw_wheel_pwm, cw_speed);

        while (get_sig(sys_sig, sig_system) == false) // system is off do nothing.
        {
            vTaskDelay(500);
        }
#endif
    }
}
