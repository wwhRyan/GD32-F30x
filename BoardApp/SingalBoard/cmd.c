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

#define fan_max 2
extern const fan_timer_config_t fan1_pwm;
extern const fan_timer_config_t fan2_pwm;
extern const fan_timer_config_t fan1_FG;
extern const fan_timer_config_t fan2_FG;

void fanset(char argc, char *argv)
{
    int fan_idx;
    int fan_speed;
    if (argc == 3)
    {
        sscanf((const char *)&(argv[argv[1]]), "%d", &fan_idx);
        sscanf((const char *)&(argv[argv[2]]), "%d", &fan_speed);

        if (fan_idx > fan_max || fan_idx <= 0)
        {
            printf("%s param fan_idx error!\n", __func__);
            return;
        }

        if (fan_speed > 100 || fan_speed < 0)
        {
            printf("%s param fan_speed error!\n", __func__);
            return;
        }
        if (fan_idx == 1)
        {
            Set_fan_timer_pwm(&fan1_pwm, fan_speed);
            printf("%s fan1_pwm set %d\n", __func__, fan_speed);
        }
        else if (fan_idx == 2)
        {
            Set_fan_timer_pwm(&fan2_pwm, fan_speed);
            printf("%s fan2_pwm set %d\n", __func__, fan_speed);
        }
    }
    else
    {
        printf("%s param error!\n", __func__);
    }
}

void fanget(char argc, char *argv)
{
    if (argc == 2)
    {
        int fan_idx;
        sscanf((const char *)&(argv[argv[1]]), "%d", &fan_idx);
        if (fan_idx > fan_max || fan_idx <= 0)
        {
            printf("%s param fan_idx error!\n", __func__);
            return;
        }
        if (fan_idx == 1)
            printf("fan 1 speed %d RPM\n", Get_fan_timer_FG(&fan1_FG) * 60 / 2);
        else if (fan_idx == 2)
            printf("fan 2 speed %d RPM\n", Get_fan_timer_FG(&fan2_FG) * 60 / 2);
    }
    else
    {
        printf("%s param error!\n", __func__);
    }
}

ICmdRegister("fanset", fanset);
ICmdRegister("fanget", fanget);
