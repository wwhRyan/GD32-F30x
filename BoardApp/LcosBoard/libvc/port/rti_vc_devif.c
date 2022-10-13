/******************************************************************************
 * (c) COPYRIGHT 2017 RAONTECH, Inc. ALL RIGHTS RESERVED.                      *
 *                                                                             *
 * This software is the property of RAONTECH and is furnished under license    *
 * by RAONTECH.                                                                *
 * This software may be used only in accordance with the terms of said license.*
 * This copyright noitce may not be removed, modified or obliterated without   *
 * the prior written permission of RAONTECH, Inc.                              *
 *                                                                             *
 * This software may not be copied, transmitted, provided to or otherwise      *
 * made available to any other person, company, corporation or other entity    *
 * except as specified in the terms of said license.                           *
 *                                                                             *
 * No right, title, ownership or other interest in the software is hereby      *
 * granted or transferred.                                                     *
 *                                                                             *
 * The information contained herein is subject to change without notice        *
 * and should not be construed as a commitment by RAONTECH, Inc.               *
 ******************************************************************************/

/* Device interface for RDC and RDP. */

#include "rti_vc_devif.h"
#include "basicApp.h"
#include "rti_vc_rdc.h"
#include <stdint.h>

#if (!defined(ECLIPSE_RCP) && !defined(__KERNEL__) && defined(__linux__) /* Linux application */)
U8_T vc_i2c_bus_number[MAX_NUM_VC_DEVICE_CH] = {
    _CFG_I2C_BUS_NR_RDC_RDP0, /* VC_DEVICE_CH_RDC_RDP0 */
    _CFG_I2C_BUS_NR_RDC_RDP1 /* VC_DEVICE_CH_RDP1 */
};

#ifdef _CFG_USE_RTIMD_KERNEL_DRIVER
static int rtimd_dev_fd;
#else
static int vc_dev_fd[MAX_NUM_VC_DEVICE_CH];
#endif // _CFG_USE_RTIMD_KERNEL_DRIVER
#else
U8_T vc_i2c_bus_number[MAX_NUM_VC_DEVICE_CH] = {
    VC_DEVICE_CH_RDC_RDP0, /* VC_DEVICE_CH_RDC_RDP0 */
    VC_DEVICE_CH_RDP1 /* VC_DEVICE_CH_RDP1 */
};
#endif /* #ifdef _CFG_USE_RTI_MD_LINUX_KERNEL_I2C_DRIVER */

/**
 * External functions
 */
#ifndef ECLIPSE_RCP
#ifdef __cplusplus
extern "C" {
#endif
#endif
int rtiVC_prepare_controller(void);
int rtiVC_prepare_panel(void);
#ifndef ECLIPSE_RCP
#ifdef __cplusplus
}
#endif
#endif

int rtiVC_OpenDevice(void)
{
#if !defined(ECLIPSE_RCP)
    if (rtiVC_prepare_controller() < 0)
        return -1;
    if (rtiVC_prepare_panel() < 0)
        return -1;
#endif
    return 0;
}

void rtiVC_DeinitDevice(void)
{
}

void rtiVC_InitDevice(void)
{
}

U8_T rtiVC_ReadDevice16(E_VC_DEVICE_CH_T dev_ch, U8_T dev_slave_addr, U16_T reg)
{
    return get_reg(dev_slave_addr, reg);
}
void rtiVC_WriteDevice16(E_VC_DEVICE_CH_T dev_ch, U8_T dev_slave_addr,
    U16_T reg, U8_T val)
{
    set_reg(dev_slave_addr, reg, val);
}

int rtiVC_ReadBurstDeviceExt(E_VC_DEVICE_CH_T dev_ch, U8_T dev_slave_addr,
    U8_T* w_buf, U16_T w_size,
    U8_T* r_buf, U16_T r_size)
{
    bool ret = true;
    uint16_t reg = (((uint16_t)w_buf[0] & 0x00ff) << 8 | ((uint16_t)w_buf[1] & 0x00ff));

    ret &= get_reg_block(dev_slave_addr, reg, r_buf, r_size);
    if (ret)
        return 0;
    else
        return -1;
}

int rtiVC_WriteBurstDeviceExt(E_VC_DEVICE_CH_T dev_ch, U8_T dev_slave_addr,
    U8_T* buf, U16_T size)
{
    bool ret = true;
    uint16_t reg = (((uint16_t)buf[0] & 0x00ff << 8) | ((uint16_t)buf[1] & 0x00ff));

    ret &= set_reg_block(dev_slave_addr, reg, buf + 2, size - 2);
    if (ret)
        return 0;
    else
        return -1;
}
