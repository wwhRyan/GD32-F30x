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
#include "ulog.h"
#include "utils.h"
#include <stdint.h>

extern const SoftwareI2C raontech_i2c;
extern const mem_t eeprom_mem[];
int cw_pwm = 25;
int target_speed = 7200;

void ThreadFirstConsumer(void* pvParameters)
{
    xEventGroupWaitBits(sys_sig, (0x00000001 << sig_mcu_init_ok), pdFALSE, pdTRUE, 0xFFFF);
    ULOG_DEBUG("%s\n", __func__);

    if (!check_boot_done()) {
        EXCUTE_ONCE(ULOG_ERROR("error to power on"));
    } else {
        gpio_bit_set(MCU_GPIO_INT_PORT, MCU_GPIO_INT_PIN);
    }

    while (1) {
        vTaskDelay(500);

        EXCUTE_ONCE(check_panel_connect());

        if (get_sig(sys_sig, sig_system)) {
            set_sig(sys_sig, sig_rdc200a_status, (gpio_input_bit_get(RDC200A_BOOT_OUT_PORT, RDC200A_BOOT_OUT_PIN) == RESET));
            /* check video active pixel */
            // if (check_video_input()) {
            //     EXCUTE_ONCE(ULOG_INFO("rdc200a decode MIPI success!\n"));
            // }
        }

        set_sig(sys_sig, sig_light_status, gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN));
        // set_sig(sys_sig, sig_light_status, true);/* for test */

        if (get_sig(sys_sig, sig_rdc200a_status) == true && get_sig(sys_sig, sig_lightsource) && !get_sig(sys_sig, sig_light_status)) {
            laser_on();
            ULOG_INFO("laser on\r\n");
        } else if ((get_sig(sys_sig, sig_rdc200a_status) == false || !get_sig(sys_sig, sig_lightsource)) && get_sig(sys_sig, sig_light_status)) {
            laser_off();
            ULOG_INFO("laser off\r\n");
        }

        if (get_sig(sys_sig, sig_light_status) == true && is_one_second() == true) {
            eeprom.light_source_time += 1;
            eeprom_block_write(&BL24C64A, (const mem_t*)&eeprom_mem[idx_light_source_time], true);
        }

        extern const fan_timer_config_t cw_wheel_pwm;
        int error = (target_speed - Get_fan_timer_FG(&cw_wheel_fg) * 30);
        cw_pwm = cw_pwm + error / 50;
        if (cw_pwm < 10)
            cw_pwm = 10;
        if (cw_pwm > 100)
            cw_pwm = 100;
        Set_fan_timer_pwm(&cw_wheel_pwm, cw_pwm);
        // output_printf("speed=%d\r\n", Get_fan_timer_FG(&cw_wheel_fg) * 30);

        if (cw_wheel_fg.p_st_calc->idle_flag == true) {
            /* 若为true，色轮未触发IO中断 */
            cw_wheel_fg.p_st_calc->fre = 0;
            EXCUTE_ONCE(ULOG_ERROR("CW wheel speed is zero\n"));
        }
        cw_wheel_fg.p_st_calc->idle_flag = true; /* 主循环中置位为true，中断置位为false */

        while (get_sig(sys_sig, sig_system) == false) // system is off do nothing.
        {
            vTaskDelay(500);
        }
    }
}
