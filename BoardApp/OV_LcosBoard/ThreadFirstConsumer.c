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

#include "basicApp.h"
#include "gd32f30x.h"
#include "main.h"
#include "ovp921.h"

extern const SoftwareI2C ovp921_i2c;
extern const mem_t eeprom_mem[];

#define EEPROM_W_QUEUE_LENGTH 5
QueueHandle_t xQueue_eeprom = NULL;
int cw_pwm = 25;
int target_speed = 7200;

void ThreadFirstConsumer(void* pvParameters)
{
    ULOG_DEBUG("%s\n", __func__);
    set_sig(sys_sig, sig_lightsource, true);
    set_sig(sys_sig, sig_system, true);
    set_sig(sys_sig, sig_light_status, false);
    set_sig(sys_sig, sig_ovp921_status, false);
    clear_sig(sys_sig, sig_update_anf);
    clear_sig(sys_sig, sig_update_firmware);
    clear_sig(sys_sig, sig_eeprom_write);
    set_sig(sys_sig, sig_slient_async_msg, false);

    xQueue_eeprom = xQueueCreate(EEPROM_W_QUEUE_LENGTH, sizeof(mem_t));
    E_assert(xQueue_eeprom);
    ULOG_INFO("xQueue_eeprom create\n");
    while (1) {

        // ULOG_DEBUG("ThreadFirstConsumer\r\n");
        // ULOG_DEBUG("ThreadFirstConsumer min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(500);
        if (get_sig(sys_sig, sig_system))
            set_sig(sys_sig, sig_ovp921_status, get_ovp921_status());

        set_sig(sys_sig, sig_light_status, gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN));

        if (get_sig(sys_sig, sig_ovp921_status) == true && get_sig(sys_sig, sig_lightsource) && !get_sig(sys_sig, sig_light_status)) {
            laser_on();
            ULOG_INFO("laser on\r\n");
        } else if ((get_sig(sys_sig, sig_ovp921_status) == false || !get_sig(sys_sig, sig_lightsource)) && get_sig(sys_sig, sig_light_status)) {
            laser_off();
            ULOG_INFO("laser off\r\n");
        }

        if (gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN) && is_one_second() == true) {
            eeprom.light_source_time += 1;
            eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
            xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_light_source_time], (TickType_t)10);
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

        if (cw_wheel_fg.p_st_calc->idle_flag == true)
            cw_wheel_fg.p_st_calc->fre = 0;
        cw_wheel_fg.p_st_calc->idle_flag = true;

        while (get_sig(sys_sig, sig_system) == false) // system is off do nothing.
        {
            vTaskDelay(500);
        }
    }
}
