/**
 * @file cmd.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-15
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "main.h"
#include "Cmdline.h"
#include "timer.h"

#define FAN_MAX_NUM 1
#define ADC_MAX_NUM 3
#define DAC_MAX_NUM 1

extern const fan_timer_config_t cw_wheel_pwm;
extern const fan_timer_config_t cw_wheel_fg;
extern const laser_dac_config_t laser_dac;

extern const ntc_adc_config_t evn_ntc;
extern const ntc_adc_config_t green_led_ntc;
extern const ntc_adc_config_t ld_ntc;

static void m_fan_set(const char *cmd, ...);
static void m_fan_get(const char *cmd, ...);
static void m_dac_set(const char *cmd, ...);
static void m_adc_get(const char *cmd, ...);

ICmdRegister("fanset", m_fan_set);
ICmdRegister("fanget", m_fan_get);
ICmdRegister("dacset", m_dac_set);
ICmdRegister("adcget", m_adc_get);

static void m_fan_set(const char *cmd, ...)
{
    int data[2] = {0};
    int cmdsize = m_CatchCmdSizeBeforeFlag(cmd, "=");

    if (2 != sscanf(cmd + cmdsize, "%d,%d",
                    &data[0], &data[1]))
    {
        printf("%s cmd error!\n", __func__);
        return;
    }

    printf("%s:%d,%d\n", __func__, data[0], data[1]);

    if (data[0] > FAN_MAX_NUM || data[0] <= 0)
    {
        printf("%s param data[0] error!\n", __func__);
        return;
    }

    if (data[1] > 100 || data[1] < 0)
    {
        printf("%s param data[1] error!\n", __func__);
        return;
    }
    if (data[0] == 1)
        Set_fan_timer_pwm(&cw_wheel_pwm, data[1]);
}

static void m_fan_get(const char *cmd, ...)
{
    int data[2] = {0};
    int cmdsize = m_CatchCmdSizeBeforeFlag(cmd, "=");

    if (2 != sscanf(cmd + cmdsize, "%d,%d",
                    &data[0], &data[1]))
    {
        printf("%s cmd error!\n", __func__);
        return;
    }

    printf("%s:%d,%d\n", __func__, data[0], data[1]);

    if (data[0] > FAN_MAX_NUM || data[0] <= 0)
    {
        printf("%s param data[0] error!\n", __func__);
        return;
    }

    if (data[1] > 100 || data[1] < 0)
    {
        printf("%s param data[1] error!\n", __func__);
        return;
    }
    if (data[0] == 1)
        printf("cw fre:%d\n", (int)Get_fan_timer_FG(&cw_wheel_fg));
}

static void m_dac_set(const char *cmd, ...)
{
    int data[2] = {0};
    int cmdsize = m_CatchCmdSizeBeforeFlag(cmd, "=");

    if (2 != sscanf(cmd + cmdsize, "%d,%d",
                    &data[0], &data[1]))
    {
        printf("%s cmd error!\n", __func__);
        return;
    }

    printf("%s:%d,%d\n", __func__, data[0], data[1]);

    if (data[0] > DAC_MAX_NUM || data[0] <= 0)
    {
        printf("%s param data[0] error!\n", __func__);
        return;
    }

    if (data[1] > 4096 || data[1] < 0)
    {
        printf("%s param data[1] error!\n", __func__);
        return;
    }
    if (data[0] == 1)
    {
        GET_TIME(laser_dac_set_value, &laser_dac, data[1]); //81Us
        GET_NOP_TIME;
    }
}

static void m_adc_get(const char *cmd, ...)
{
    int data[2] = {0};
    int cmdsize = m_CatchCmdSizeBeforeFlag(cmd, "=");

    if (2 != sscanf(cmd + cmdsize, "%d,%d",
                    &data[0], &data[1]))
    {
        printf("%s cmd error!\n", __func__);
        return;
    }

    printf("%s:%d,%d\n", __func__, data[0], data[1]);

    if (data[0] > ADC_MAX_NUM || data[0] <= 0)
    {
        printf("%s param data[0] error!\n", __func__);
        return;
    }

    if (data[1] > 100 || data[1] < 0)
    {
        printf("%s param data[1] error!\n", __func__);
        return;
    }
    if (data[0] == 1)
        printf("adc ld:%d\n", get_ntc_adc_sample(&ld_ntc));

    if (data[0] == 2)
        printf("adc green led:%d\n", get_ntc_adc_sample(&green_led_ntc));

    if (data[0] == 3)
        printf("adc EVN:%d\n", get_ntc_adc_sample(&evn_ntc));
}
