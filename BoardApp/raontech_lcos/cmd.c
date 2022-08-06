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
#include "rti_vc_api.h"

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
    int ret = 0;
    VC_PANEL_TEMPERATURE_INFO_T tinfo[MAX_NUM_VC_PANEL_PORT] = { 0 };

    ret = rtiVC_GetTemperature(VC_PANEL_CTRL_PORT_0, tinfo);

    cmd_printf("lcos-temprature=%f\r\n", temperature);
}

ICmdRegister("lcos", lcos);
