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

static void m_hello(const char *cmd, ...);
static void m_getNumber(const char *cmd, ...);
static void m_getMutiNumber(const char *cmd, ...);
static void m_getIndefinitelengthNumber(const char *cmd, ...);

ICmdRegister("hello", m_hello);
ICmdRegister("getNumber", m_getNumber);
ICmdRegister("getMutiNumber", m_getMutiNumber);
ICmdRegister("getIndefinitelengthNumber", m_getIndefinitelengthNumber);

/**
                        __   ____                     __   _             
  _____ ____ ___   ____/ /  / __/__  __ ____   _____ / /_ (_)____   ____ 
 / ___// __ `__ \ / __  /  / /_ / / / // __ \ / ___// __// // __ \ / __ \
/ /__ / / / / / // /_/ /  / __// /_/ // / / // /__ / /_ / // /_/ // / / /
\___//_/ /_/ /_/ \__,_/  /_/   \__,_//_/ /_/ \___/ \__//_/ \____//_/ /_/ 
 */

static void m_hello(const char *cmd, ...)
{
    printf("hello\n");
}

static void m_getNumber(const char *cmd, ...)
{
    int data = 0;
    int cmdsize = m_CatchCmdSizeBeforeFlag(cmd, "=");
    sscanf(cmd + cmdsize, "%d", &data);
    printf("%d\n", data);
}

static void m_getMutiNumber(const char *cmd, ...)
{
    int data[8] = {0};
    int cmdsize = m_CatchCmdSizeBeforeFlag(cmd, "=");

    if (8 != sscanf(cmd + cmdsize, "%d,%d,%d,%d,%d,%d,%d,%d",
                    &data[0], &data[1], &data[2], &data[3],
                    &data[4], &data[5], &data[6], &data[7]))
    {
        printf("m_getMutiNumber cmd error!");
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(data); i++)
    {
        printf("%d \n", data[i]);
    }
}

static void m_getIndefinitelengthNumber(const char *cmd, ...)
{
    uint8_t i = 0;
    int data[MAX_WRITE_LEN] = {0};
    int cmdsize = m_CatchCmdSizeBeforeFlag(cmd, "=");

    char *token = NULL;
    token = strtok((char *)(cmd + cmdsize), ",");

    while (token)
    {
        sscanf((const char *)token, "%d", &data[i++]);
        token = strtok(NULL, ",");
    }

    int len = i;
    for (size_t i = 0; i < len; i++)
    {
        printf("%d \n", data[i]);
    }
}
