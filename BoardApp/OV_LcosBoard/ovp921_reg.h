/**
 * @file ovp921_reg.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-04-12
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef OVP921_REG_H
#define OVP921_REG_H

#include "main.h"

// SCCB device address 7-bit format:    000010(A0)
// A0 = 1, ID = 7'h05
// A0 = 0, ID = 7'h04
#define OVP921_SCCB_ADDRESS 0x05
#define OVP921_SCCB_ADDRESS_WRITE (uint8_t)(OVP921_SCCB_ADDRESS << 1)
#define OVP921_SCCB_ADDRESS_READ (uint8_t)(OVP921_SCCB_ADDRESS << 1 | 0x01)

union reg_scalar_Xdim_low
{
    uint8_t reg;
    struct
    {
        uint8_t scl_xdim : 8;
    } bits;
};

union reg_scalar_Ydim_low
{
    uint8_t reg;
    struct
    {
        uint8_t scl_ydim : 8;
    } bits;
};

union reg_scalar_XYdim_high
{
    uint8_t reg;
    struct
    {
        uint8_t reserved : 2;
        uint8_t scl_ydim : 2;
        uint8_t reserved2 : 1;
        uint8_t scl_xdim : 3;
    } bits;
};

union reg_keystone_scalar_control
{
    uint8_t reg;
    struct
    {
        uint8_t keystone_en : 1;
        uint8_t reserved : 2;
        uint8_t keystone_slope : 1;
        uint8_t reserved2 : 1;
        uint8_t scalar_bypass : 1;
        uint8_t reserved3 : 1;
        uint8_t scalar_en : 4;
    } bits;
};

union reg_keystone_offset
{
    uint8_t reg;
    struct
    {
        uint8_t keystone_offset : 8;
    } bits;
};

#endif
