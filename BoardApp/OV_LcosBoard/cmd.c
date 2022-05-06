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
#include "basicApp.h"

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
    if (argc == 1 + 2)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &fan_idx);
        sscanf((const char *)&(argv[argv[2]]), "%d", &fan_speed);

        if (fan_idx > FAN_MAX_NUM || fan_idx <= 0)
        {
            cmd_printf("%s param fan_idx error!\n", __func__);
            return;
        }

        if (fan_speed > 100 || fan_speed < 0)
        {
            cmd_printf("%s param fan_speed error!\n", __func__);
            return;
        }
        if (fan_idx == 1)
            Set_fan_timer_pwm(&cw_wheel_pwm, fan_speed);
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

void fanget(char argc, char *argv)
{
    if (argc == 1 + 1)
    {
        int fan_idx;
        sscanf((const char *)&(argv[argv[1]]), "%d", &fan_idx);
        if (fan_idx > FAN_MAX_NUM || fan_idx <= 0)
        {
            cmd_printf("%s param fan_idx error!\n", __func__);
            return;
        }
        if (fan_idx == 1)
            cmd_printf("%d\n", Get_fan_timer_FG(&cw_wheel_fg));
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

void dacset(char argc, char *argv)
{
    int dac_idx;
    int dac_value;
    if (argc == 1 + 2)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &dac_idx);
        sscanf((const char *)&(argv[argv[2]]), "%d", &dac_value);

        if (dac_idx > DAC_MAX_NUM || dac_idx <= 0)
        {
            cmd_printf("%s param dac_idx error!\n", __func__);
            return;
        }

        if (dac_value > 100 || dac_value < 0)
        {
            cmd_printf("%s param dac_value error!\n", __func__);
            return;
        }
        if (dac_idx == 1)
            laser_dac_set_value(&laser_dac, dac_value * 4095 / 100);
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
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
            cmd_printf("%s param adc_idx error!\n", __func__);
            return;
        }
        if (adc_idx == 1)
            cmd_printf("%d\n", get_ntc_adc_sample(&ld_ntc));
        else if (adc_idx == 2)
            cmd_printf("%d\n", get_ntc_adc_sample(&green_led_ntc));
        else if (adc_idx == 3)
            cmd_printf("%d\n", get_ntc_adc_sample(&evn_ntc));
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

void gettime(char argc, char *argv)
{
    int val;
    if (argc == 1 + 1)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &val);
        cmd_printf("set laser val = %d\n", val);
        GET_TIME(laser_dac_set_value, &laser_dac, val); //81Us
        GET_NOP_TIME;
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

void chipid(char argc, char *argv)
{
    get_chipid();
    cmd_printf("ovp921 id:%04X\r\n", ovp921.chipid.reg.bits.chip_id);
    cmd_printf("ovp921 mark version:%04X\r\n", ovp921.chipid.reg.bits.mask_version);
    cmd_printf("ovp921 id2:%04X\r\n", ovp921.chipid2.reg.bits.chip_id2);
}

void testpattern(char argc, char *argv)
{
    if (argc == 1 + 1)
    {
        if (!strcmp("-h", &argv[argv[1]]))
        {
            cmd_printf("useage: %s [options]\r\n", __func__);
            cmd_printf("options: \r\n");
            cmd_printf("\t red \t: show red\r\n");
            cmd_printf("\t green \t: show green\r\n");
            cmd_printf("\t blue \t: show blue\r\n");
            cmd_printf("\t white \t: show white\r\n");
            cmd_printf("\t black \t: show black\r\n");
            cmd_printf("\t yellow \t: show yellow\r\n");
            cmd_printf("\t cyan \t: show cyan\r\n");
            cmd_printf("\t magenta \t: show magenta\r\n");
            cmd_printf("\t gray \t: show gray\r\n");
            cmd_printf("\t ramp \t: show ramp\r\n");
            cmd_printf("\t checkerboard \t: show checkerboard\r\n");
            cmd_printf("\t off \t: show off\r\n");
        }
        else if (!strcmp("ramp", &argv[argv[1]]))
        {
            gray_ramp_pattern();
            cmd_printf("show ramp\r\n");
        }
        else if (!strcmp("red", &argv[argv[1]]))
        {
            show_solid_color_pattern(0xFF, 0x00, 0x00);
            cmd_printf("show red\r\n");
        }
        else if (!strcmp("blue", &argv[argv[1]]))
        {
            show_solid_color_pattern(0x00, 0x00, 0xFF);
            cmd_printf("show blue\r\n");
        }
        else if (!strcmp("green", &argv[argv[1]]))
        {
            show_solid_color_pattern(0x00, 0xFF, 0x00);
            cmd_printf("show green\r\n");
        }
        else if (!strcmp("white", &argv[argv[1]]))
        {
            show_solid_color_pattern(0xFF, 0xFF, 0xFF);
            cmd_printf("show white\r\n");
        }
        else if (!strcmp("black", &argv[argv[1]]))
        {
            show_solid_color_pattern(0x00, 0x00, 0x00);
            cmd_printf("show black\r\n");
        }
        else if (!strcmp("yellow", &argv[argv[1]]))
        {
            show_solid_color_pattern(0xFF, 0xFF, 0x00);
            cmd_printf("show yellow\r\n");
        }
        else if (!strcmp("cyan", &argv[argv[1]]))
        {
            show_solid_color_pattern(0x00, 0xFF, 0xFF);
            cmd_printf("show cyan\r\n");
        }
        else if (!strcmp("magenta", &argv[argv[1]]))
        {
            show_solid_color_pattern(0xFF, 0x00, 0xFF);
            cmd_printf("show magenta\r\n");
        }
        else if (!strcmp("gray", &argv[argv[1]]))
        {
            show_solid_color_pattern(0x80, 0x80, 0x80);
            cmd_printf("show gray\r\n");
        }
        else if (!strcmp("checkerboard", &argv[argv[1]]))
        {
            checkerboard_pattern();
            cmd_printf("show checkerboard\r\n");
        }
        else if (!strcmp("off", &argv[argv[1]]))
        {
            off_pattern();
            cmd_printf("show off\r\n");
        }
        else
        {
            cmd_printf("%s param error!\n", __func__);
        }
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

int get_color_idx(char *color)
{
    if (!strcmp("r", color) || !strcmp("R", color))
        return (int)RED;
    else if (!strcmp("g", color) || !strcmp("G", color))
        return (int)GREEN;
    else if (!strcmp("b", color) || !strcmp("B", color))
        return (int)BLUE;
    else
        return -1;
}

void laser(char argc, char *argv)
{
    if (argc == 1 + 1)
    {
        if (!strcmp("-h", &argv[argv[1]]))
        {
            cmd_printf("useage: %s [options]\r\n", __func__);
            cmd_printf("options: \r\n");
            cmd_printf("\t on \t: turn on laser\r\n");
            cmd_printf("\t off \t: turn off laser\r\n");
            cmd_printf("\t R 0.7 \t: set laser Red current\r\n");
        }
        else if (!strcmp("on", &argv[argv[1]]))
        {
            laser_on();
            cmd_printf("laser on\r\n");
        }
        else if (!strcmp("off", &argv[argv[1]]))
        {
            laser_off();
            cmd_printf("laser off\r\n");
        }
        else
        {
            cmd_printf("%s param error!\n", __func__);
        }
    }
    else if (argc == 1 + 2)
    {
        float current;
        int idx = get_color_idx(&argv[argv[1]]);
        if (idx < 0)
        {
            cmd_printf("%s color can`t resolve!\n", __func__);
            return;
        }

        printf("old current: %f\n", laser_get(idx));

        sscanf((const char *)&(argv[argv[2]]), "%f", &current);
        if (true == laser_set(idx, current))
            cmd_printf("laser idx %d current set to %f\r\n", idx, current);
        else
            cmd_printf("laser idx %d current set failed!\r\n", idx);
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

void write(char argc, char *argv)
{
    uint32_t addr;
    uint32_t val;
    if (argc >= 2 + 1)
    {
        sscanf((const char *)&(argv[argv[1]]), "%x", &addr);
        for (size_t i = 0; i < argc - 2; i++)
        {
            sscanf((const char *)&(argv[argv[i + 2]]), "%x", &val);
            set_reg((uint16_t)addr, (uint8_t)val);
            cmd_printf("write addr:%04x val:%02x ", addr, val);
            cmd_printf("read addr:%04x val:%02x\r\n", addr, get_reg((uint16_t)addr));
            addr++;
            val = 0x00;
        }
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

void read(char argc, char *argv)
{
    uint32_t addr;
    if (argc == 1 + 1)
    {
        sscanf((const char *)&(argv[argv[1]]), "%x", &addr);
        cmd_printf("read addr:%04x val:%02x\r\n", addr, get_reg((uint16_t)addr));
    }
    else if (argc == 1 + 2)
    {
        int num;
        sscanf((const char *)&(argv[argv[1]]), "%x", &addr);
        sscanf((const char *)&(argv[argv[2]]), "%d", &num);

        if (num > 256)
        {
            cmd_printf("%s num > 256!\n", __func__);
            return;
        }
        else
        {
            for (int i = 0; i < num; i++)
            {
                cmd_printf("addr:%04x val:%02x \r\n", addr, get_reg((uint16_t)addr));
                addr += 1;
            }
            cmd_printf("\r\n");
        }
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

void reset(char argc, char *argv)
{
    if (argc == 1 + 1)
    {
        if (!strcmp("-h", &argv[argv[1]]))
        {
            cmd_printf("useage: %s [options]\r\n", __func__);
            cmd_printf("options: \r\n");
            cmd_printf("\t ov \t: ovp921 reset\r\n");
            cmd_printf("\t sys \t: sys 12V reset\r\n");
        }
        else if (!strcmp("ov", &argv[argv[1]]))
        {
            gpio_bit_set(OVP921_RESET_PORT, OVP921_RESET_PIN);
            vTaskDelay(1000);
            gpio_bit_reset(OVP921_RESET_PORT, OVP921_RESET_PIN);
            cmd_printf("reset ovp921 reset pin.\r\n");
        }
        else if (!strcmp("sys", &argv[argv[1]]))
        {
            gpio_bit_set(OVP921_RESET_PORT, OVP921_RESET_PIN);
            gpio_bit_reset(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            vTaskDelay(1000);
            gpio_bit_set(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            cmd_printf("reset sys 12V reset pin.\r\n");
            vTaskDelay(1000);
            gpio_bit_reset(OVP921_RESET_PORT, OVP921_RESET_PIN);
            cmd_printf("reset ovp921 reset pin.\r\n");
        }
        else
        {
            cmd_printf("%s param error!\n", __func__);
        }
    }
    else
    {
        cmd_printf("%s Please input right param!\n", __func__);
    }
}

ICmdRegister("write", write);
ICmdRegister("read", read);
ICmdRegister("fanset", fanset);
ICmdRegister("fanget", fanget);
ICmdRegister("dacset", dacset);
ICmdRegister("adcget", adcget);
ICmdRegister("chipid", chipid);
ICmdRegister("gettime", gettime);
ICmdRegister("laser", laser);
ICmdRegister("testpattern", testpattern);
ICmdRegister("reset", reset);
