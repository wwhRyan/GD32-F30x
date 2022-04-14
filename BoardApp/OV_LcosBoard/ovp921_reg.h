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

#define SCCB_DELAY_TIME 5 //5ms for i2c wait time

/**
 * @brief GD32 using arm cm4, it is little endian
 * @note union bit field order is from low to high
 * @note I know it is not good, but it is easy to understand
 * @note in the past, I know endian is using in bytes, but now, I see it is using in bit fields too.
 * @note even in a uint8_t, the bit fields order is from low to high
 */

typedef struct reg_scalar_Xdim_low_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t scl_xdim : 8;
        } bits;
    } reg;
} reg_scalar_Xdim_low_t;

typedef struct reg_scalar_Ydim_low_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t scl_ydim : 8;
        } bits;
    } reg;
} reg_scalar_Ydim_low_t;

typedef struct reg_scalar_XYdim_high_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t scl_xdim : 3;
            uint8_t : 1;
            uint8_t scl_ydim : 2;
            uint8_t : 2;
        } bits;
    } reg;
} reg_scalar_XYdim_high_t;

typedef struct reg_keystone_scalar_control_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t scalar_en : 1;
            uint8_t : 1;
            uint8_t scalar_bypass : 1;
            uint8_t : 1;
            uint8_t keystone_slope : 1;
            uint8_t : 2;
            uint8_t keystone_en : 1;
        } bits;
    } reg;
} reg_keystone_scalar_control_t;

typedef struct reg_keystone_offset_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t keystone_offset : 8;
        } bits;
    } reg;
} reg_keystone_offset_t;

typedef struct reg_video_input_bus_byte_swap_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t swap_for_red : 2;
            uint8_t swap_for_green : 2;
            uint8_t swap_for_blue : 2;
            uint8_t : 2;
        } bits;
    } reg;
} reg_video_input_bus_byte_swap_t;

typedef struct reg_video_input_bus_bit_swap_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t swap_for_red : 1;
            uint8_t swap_for_green : 1;
            uint8_t swap_for_blue : 1;
            uint8_t reserved : 5;
        } bits;
    } reg;
} reg_video_input_bus_bit_swap_t;

typedef struct reg_XPan_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t horizontal_pan_value : 7;
            uint8_t Xpan_direction : 1;
        } bits;
    } reg;
} reg_XPan_t;

typedef struct reg_YPan_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t vertical_pan_value : 7;
            uint8_t Ypan_direction : 1;
        } bits;
    } reg;
} reg_YPan_t;

typedef struct reg_horizontal_mode_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t : 7;
            uint8_t horizontal_filp : 1;
        } bits;
    } reg;
} reg_horizontal_mode_t;

typedef struct reg_pattern_generator_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pattern_generator_en : 1;
            uint8_t pattern_generator_type : 2;
            uint8_t : 5;
        } bits;
    } reg;
} reg_pattern_generator_t;

typedef struct reg_video_Xdim_manuall_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t video_Xdim_manual : 8;
        } bits;
    } reg;
} reg_video_Xdim_manuall_t;

typedef struct reg_video_Ydim_manuall_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t video_Ydim_manual : 8;
        } bits;
    } reg;
} reg_video_Ydim_manuall_t;

typedef struct reg_video_XYdim_manuall_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t video_Ydim_manual : 2;
            uint8_t : 2;
            uint8_t video_Xdim_manual : 2;
            uint8_t : 1;
            uint8_t video_dim_useManual : 1;
        } bits;
    } reg;
} reg_video_XYdim_manuall_t;

typedef struct reg_pattern_gen_vsync_width_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pattern_gen_vsync_width : 4;
            uint8_t : 4;
        } bits;
    } reg;
} reg_pattern_gen_vsync_width_t;

typedef struct reg_pattern_gen_vsync_front_porch_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_vfporch : 8;
        } bits;
    } reg;
} reg_pattern_gen_vsync_front_porch_t;

typedef struct reg_pattern_gen_hsync_width_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pattern_gen_hsync_width : 4;
            uint8_t : 4;
        } bits;
    } reg;
} reg_pattern_gen_hsync_width_t;

typedef struct reg_pattern_gen_red_data_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t reg_pgen_data_r : 8;
        } bits;
    } reg;
} reg_pattern_gen_red_data_t;

typedef struct reg_pattern_gen_green_data_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t reg_pgen_data_g : 8;
        } bits;
    } reg;
} reg_pattern_gen_green_data_t;

typedef struct reg_pattern_gen_blue_data_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t reg_pgen_data_b : 8;
        } bits;
    } reg;
} reg_pattern_gen_blue_data_t;

typedef struct reg_chipid_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t chip_id : 4;
            uint8_t mask_version : 4;
        } bits;
    } reg;
} reg_chipid_t;

typedef struct reg_chipid2_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t chip_id2 : 8;
        } bits;
    } reg;
} reg_chipid2_t;

struct ovp921_t
{
    reg_pattern_generator_t pattern_generator;
    reg_chipid_t chipid;
    reg_chipid2_t chipid2;
    reg_pattern_gen_red_data_t pattern_gen_red_data;
    reg_pattern_gen_green_data_t pattern_gen_green_data;
    reg_pattern_gen_blue_data_t pattern_gen_blue_data;
};

#endif
