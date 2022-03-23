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

static void m_fan_set(const char *cmd, ...);
static void m_fan_get(const char *cmd, ...);

ICmdRegister("fanset", m_fan_set);
ICmdRegister("fanget", m_fan_get);

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

    if (data[0] > fan_max || data[0] <= 0)
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
        Set_fan_timer_pwm(&fan1_pwm, data[1]);

    if (data[0] == 2)
        Set_fan_timer_pwm(&fan2_pwm, data[1]);
}

extern __IO uint16_t fre;
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

    if (data[0] > fan_max || data[0] <= 0)
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
        printf("fan fre 1:%d\n", (int)fre);

    if (data[0] == 2)
        printf("fan 2:%d\n", (int)Get_fan_timer_FG(&fan2_FG));
}
