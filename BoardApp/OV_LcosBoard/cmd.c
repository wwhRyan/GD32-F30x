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
#include "ovp921.h"

#define FAN_MAX_NUM 1
#define ADC_MAX_NUM 3
#define DAC_MAX_NUM 1

extern const fan_timer_config_t cw_wheel_pwm;
extern const fan_timer_config_t cw_wheel_fg;
extern const laser_dac_config_t laser_dac;

extern const ntc_adc_config_t evn_ntc;
extern const ntc_adc_config_t green_led_ntc;
extern const ntc_adc_config_t ld_ntc;

extern struct ovp921_t ovp921;

void fanset(char argc, char *argv)
{
    int fan_idx;
    int fan_speed;
    if (argc == 2 + 1)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &fan_idx);
        sscanf((const char *)&(argv[argv[2]]), "%d", &fan_speed);

        if (fan_idx > FAN_MAX_NUM || fan_idx <= 0)
        {
            debug_printf("%s param fan_idx error!\n", __func__);
            return;
        }

        if (fan_speed > 100 || fan_speed < 0)
        {
            debug_printf("%s param fan_speed error!\n", __func__);
            return;
        }
        if (fan_idx == 1)
            Set_fan_timer_pwm(&cw_wheel_pwm, fan_speed);
    }
    else
    {
        debug_printf("%s param error!\n", __func__);
    }
}

void fanget(char argc, char *argv)
{
    if (argc == FAN_MAX_NUM + 1)
    {
        int fan_idx;
        sscanf((const char *)&(argv[argv[1]]), "%d", &fan_idx);
        if (fan_idx > FAN_MAX_NUM || fan_idx <= 0)
        {
            debug_printf("%s param fan_idx error!\n", __func__);
            return;
        }
        if (fan_idx == 1)
            debug_printf("%d\n", Get_fan_timer_FG(&cw_wheel_fg));
    }
    else
    {
        debug_printf("%s param error!\n", __func__);
    }
}

void dacset(char argc, char *argv)
{
    int dac_idx;
    int dac_value;
    if (argc == 1 + 1)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &dac_idx);
        sscanf((const char *)&(argv[argv[2]]), "%d", &dac_value);

        if (dac_idx > DAC_MAX_NUM || dac_idx <= 0)
        {
            debug_printf("%s param dac_idx error!\n", __func__);
            return;
        }

        if (dac_value > 100 || dac_value < 0)
        {
            debug_printf("%s param dac_value error!\n", __func__);
            return;
        }
        if (dac_idx == 1)
            laser_dac_set_value(&laser_dac, dac_value);
    }
    else
    {
        debug_printf("%s param error!\n", __func__);
    }
}

void adcget(char argc, char *argv)
{
    int adc_idx;
    if (argc == 1 + 1)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &adc_idx);
        if (adc_idx > ADC_MAX_NUM || adc_idx <= 0)
        {
            debug_printf("%s param adc_idx error!\n", __func__);
            return;
        }
        if (adc_idx == 1)
            debug_printf("%d\n", get_ntc_adc_sample(&ld_ntc));
        else if (adc_idx == 2)
            debug_printf("%d\n", get_ntc_adc_sample(&green_led_ntc));
        else if (adc_idx == 3)
            debug_printf("%d\n", get_ntc_adc_sample(&evn_ntc));
    }
    else
    {
        debug_printf("%s param error!\n", __func__);
    }
}

void gettime(char argc, char *argv)
{
    int val;
    if (argc == 2)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &val);
        GET_TIME(laser_dac_set_value, &laser_dac, val); //81Us
        GET_NOP_TIME;
    }
    else
    {
        debug_printf("%s param error!\n", __func__);
    }
}

void chipid(char argc, char *argv)
{
    get_chipid();
    debug_printf("ovp921 id:%#X\r\n", ovp921.chipid.reg.bits.chip_id);
    debug_printf("ovp921 mark version:%#X\r\n", ovp921.chipid.reg.bits.mask_version);
    debug_printf("ovp921 id2:%#X\r\n", ovp921.chipid2.reg.bits.chip_id2);
}

void testpattern(char argc, char *argv)
{
    if (argc == 1 + 1)
    {
        if (!strcmp("-h", &argv[argv[1]]))
        {
            cmd_printf("useage: ls [options]\r\n");
            cmd_printf("options: \r\n");
            cmd_printf("\t red \t: show red\r\n");
            cmd_printf("\t green \t: show green\r\n");
            cmd_printf("\t blue \t: show blue\r\n");
            cmd_printf("\t ramp \t: show ramp\r\n");
            cmd_printf("\t checkerboard \t: show checkerboard\r\n");
            cmd_printf("\t off \t: show off\r\n");
        }
        else if (!strcmp("ramp", &argv[argv[1]]))
        {
            gray_ramp_pattern();
        }
        else if (!strcmp("red", &argv[argv[1]]))
        {
            red_pattern();
        }
        else if (!strcmp("blue", &argv[argv[1]]))
        {
            blue_pattern();
        }
        else if (!strcmp("green", &argv[argv[1]]))
        {
            green_pattern();
        }
        else if (!strcmp("checkerboard", &argv[argv[1]]))
        {
            checkerboard_pattern();
        }
        else if (!strcmp("off", &argv[argv[1]]))
        {
            off_pattern();
        }
        else
        {
            debug_printf("%s param error!\n", __func__);
        }
    }
    else
    {
        debug_printf("%s param error!\n", __func__);
    }
}

ICmdRegister("fanset", fanset);
ICmdRegister("fanget", fanget);
ICmdRegister("dacset", dacset);
ICmdRegister("adcget", adcget);
ICmdRegister("chipid", chipid);
ICmdRegister("gettime", gettime);
