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

#include "BoardInit.h"
#include "Cmdline.h"
#include "basicApp.h"
#include "gd32f30x_gpio.h"
#include "main.h"
#include "rti_vc_api.h"
#include "rti_vc_panel.h"
#include "rti_vc_rdc.h"
#include "rti_vc_regio.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int rdp250h_get_device_id(E_VC_PANEL_PORT_T port, U16_T* chip_id, U16_T* rev_num)
{
    U8_T reg0x00, reg0x01;
    reg0x00 = RDP_REG_GET(port, 0x0000);
    reg0x01 = RDP_REG_GET(port, 0x0001);
    *chip_id = ((U16_T)reg0x00 << 4) | (reg0x01 >> 4);
    *rev_num = reg0x01 & 0x0F;
    return 0;
}

static int rdc200a_get_device_id(U16_T* chip_id, U16_T* rev_num)
{
    U8_T chip_id_msb, chip_id_lsb;
    chip_id_msb = RDC_REG_GET(0x0000);
    chip_id_lsb = RDC_REG_GET(0x0001);
    *chip_id = ((U16_T)chip_id_msb << 8) | chip_id_lsb;
    *rev_num = 0;
    return 0;
}

void chipid(char argc, char* argv)
{
    uint16_t id;
    uint16_t rev;

    rdp250h_get_device_id(VC_PANEL_PORT_0, &id, &rev);
    cmd_printf("rdp250H id:%#X, rev:%#X\n", id, rev);

    rdc200a_get_device_id(&id, &rev);
    cmd_printf("rdc200A id:%#X, rev:%#X\n", id, rev);
}

void test(char argc, char* argv)
{
    uint16_t id;
    id = get_reg(0x4A << 1, 0x0000);
    cmd_printf("test %#X\n", id);

    cmd_printf("SDA IO = %d\n", gpio_input_bit_get(SCCB_SDA_PORT, SCCB_SDA_PIN));
}

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

    /* make sure the first read right. */
    float temperature[MAX_NUM_VC_PANEL_PORT] = { 0 };
    static float old_temperature[MAX_NUM_VC_PANEL_PORT] = { 0 };

    VC_PANEL_TEMPERATURE_INFO_T tinfo[MAX_NUM_VC_PANEL_PORT] = { 0 };
    rtiVC_GetTemperature(VC_PANEL_CTRL_PORT_ALL, tinfo);
    temperature[0] = (float)(tinfo[0].temperature) / VC_TEMPERATURE_DEGREE_DIV;
    temperature[1] = (float)(tinfo[1].temperature) / VC_TEMPERATURE_DEGREE_DIV;

    if (old_temperature[0] == 0 && old_temperature[1] == 0) {
        old_temperature[0] = temperature[0];
        old_temperature[1] = temperature[1];
    } else {

        for (int i = 0; i < 2; i++) {
            /* 只有温度在绝对值5以内，才会更新old_temperature */
            if (fabs(temperature[i] - old_temperature[i]) <= 5) {
                old_temperature[i] = temperature[i];
            }
        }
    }

    cmd_printf("lcos1=%f,lcos2=%f\r\n", old_temperature[0], old_temperature[1]);
    return;
}

ICmdRegister("test", test);
ICmdRegister("lcos", lcos); // ok
ICmdRegister("chipid", chipid);
