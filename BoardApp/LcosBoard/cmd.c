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

void lcos_interrupt(char argc, char* argv)
{
    float temperature[MAX_NUM_VC_PANEL_PORT] = { 0 };
    static float old_temperature[MAX_NUM_VC_PANEL_PORT] = { 0 };
    static bool interrupt = false;

    uint16_t id;
    uint16_t rev;

    rdp250h_get_device_id(VC_PANEL_PORT_0, &id, &rev);
    if (id != 0x250) {
        if (interrupt == false) {
            old_temperature[0] = 0;
            old_temperature[1] = 0;
            cmd_printf("lcos1=%f,lcos2=%f\r\n", old_temperature[0], old_temperature[1]);
        }
        interrupt = true;
        return;
    }

    interrupt = false;

    VC_PANEL_TEMPERATURE_INFO_T tinfo[MAX_NUM_VC_PANEL_PORT] = { 0 };
    rtiVC_GetTemperature(VC_PANEL_CTRL_PORT_ALL, tinfo);
    temperature[0] = (float)(tinfo[0].temperature) / VC_TEMPERATURE_DEGREE_DIV;
    temperature[1] = (float)(tinfo[1].temperature) / VC_TEMPERATURE_DEGREE_DIV;

    if (old_temperature[0] == 0 && old_temperature[1] == 0) {
        old_temperature[0] = temperature[0];
        old_temperature[1] = temperature[1];
        cmd_printf("lcos1=%f,lcos2=%f\r\n", old_temperature[0], old_temperature[1]);
    }

    return;
}

uint32_t interval = 0;

void set_interval(char argc, char* argv)
{
    if (argc == 1 + 1) {
        if (!strcmp("-h", &argv[argv[1]])) {
            cmd_printf("useage: %s get lcos temprature\r\n", __func__);
        } else {
            sscanf((const char*)&(argv[argv[1]]), "%d", &interval);
            cmd_printf("%d s to feedback temperature!\n", interval);
        }
    }
}

void lcos_no_power(char argc, char* argv)
{
    float temperature[MAX_NUM_VC_PANEL_PORT] = { 0 };
    static float old_temperature[MAX_NUM_VC_PANEL_PORT] = { 0 };

    RDP_REG_SET(0, 0x02, 0x3);
    RDP_REG_SET(0, 0x03, 0x76);
    RDP_REG_SET(0, 0x04, 0xa6);

    RDP_REG_SET(1, 0x02, 0x3);
    RDP_REG_SET(1, 0x03, 0x76);
    RDP_REG_SET(1, 0x04, 0xa6);

    extern VC_RDC_CB_T vc_rdc_cb;
    VC_RDC_DEV_INFO_T* dev = &vc_rdc_cb.dev;
    dev->poweron_panel(TRUE);
    vTaskDelay(100);

    VC_PANEL_TEMPERATURE_INFO_T tinfo[MAX_NUM_VC_PANEL_PORT] = { 0 };
    rtiVC_GetTemperature(VC_PANEL_CTRL_PORT_ALL, tinfo);
    temperature[0] = (float)(tinfo[0].temperature) / VC_TEMPERATURE_DEGREE_DIV;
    temperature[1] = (float)(tinfo[1].temperature) / VC_TEMPERATURE_DEGREE_DIV;

    dev->poweron_panel(FALSE);
    vTaskDelay(10);

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

    debug_printf("lcos1=%f,lcos2=%f\r\n", old_temperature[0], old_temperature[1]);
}

uint32_t no_power_interval = 0;

void set_no_power_interval(char argc, char* argv)
{
    if (argc == 1 + 1) {
        if (sscanf((const char*)&(argv[argv[1]]), "%d", &no_power_interval) > 0) {
            debug_printf("no power temperature testing. \n");
            debug_printf("%d s to feedback temperature!\n", no_power_interval);

            /*
            0x02 = 0x3
            0x03 = 0x76
            0x04= 0xa6
             */

            RDP_REG_SET(0, 0x02, 0x3);
            RDP_REG_SET(0, 0x03, 0x76);
            RDP_REG_SET(0, 0x04, 0xa6);

            RDP_REG_SET(1, 0x02, 0x3);
            RDP_REG_SET(1, 0x03, 0x76);
            RDP_REG_SET(1, 0x04, 0xa6);

            extern VC_RDC_CB_T vc_rdc_cb;
            VC_RDC_DEV_INFO_T* dev = &vc_rdc_cb.dev;
            dev->poweron_panel(FALSE);
            vTaskDelay(10);
        }
    }
}

ICmdRegister("test", test);
ICmdRegister("lcos", lcos); // ok
ICmdRegister("lcos_no_power", lcos_no_power); // ok
ICmdRegister("lcos_interrupt", lcos_interrupt); // ok
ICmdRegister("chipid", chipid);
ICmdRegister("set_interval", set_interval); // ok
ICmdRegister("set_no_power_interval", set_no_power_interval); // ok
