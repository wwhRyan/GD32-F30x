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

int rtiVC_OpenDevice(void)
{
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
    return -1;
}

int rtiVC_WriteBurstDeviceExt(E_VC_DEVICE_CH_T dev_ch, U8_T dev_slave_addr,
    U8_T* buf, U16_T size)
{
    return -1;
}
