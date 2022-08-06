/**
 * @file cmd.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-08-06
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#include "Cmdline.h"
#include "basicApp.h"
#include "main.h"

void lcos(char argc, char* argv)
{
    /*
    if (argc == 1 + 1)
    {
        if (!strcmp("-h", &argv[argv[1]]))
        {
            cmd_printf("useage: %s get lcos temprature\r\n", __func__);
        }
    }*/
    float temperature;

    cmd_printf("lcos-temprature=%f\r\n", temperature);
}

ICmdRegister("lcos", lcos);
