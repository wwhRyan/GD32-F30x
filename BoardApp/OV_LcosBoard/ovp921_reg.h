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

#define CHIP_ID_ADDR 0x0000
#define MICROLCD_CONTROL_ADDR 0x000A
#define INCOMING_VIDEO_PROPERTIES_ADDR 0x0014
#define MISCELLANEOUS_ADDR 0x0040
#define SCALAR_ADDR 0x00B3
#define PLL_CONTROL_ADDR 0x00DF
#define PATTERN_GENERATOR_ADDR 0x0100
#define MIPI_RECEIVER_ADDR 0x0126
#define LIGHT_SOURCE_CONTROL_ADDR 0x0194
#define MIPI_TRANSMITTER_ADDR 0x0600
#define SPECIAL_CONTROL_SOURCE_ADDR 0x7FC0

#define SCCB_DELAY_TIME 5 // 5ms for i2c wait time

/**
 * @brief GD32 using arm cm4, it is little endian
 * @note union bit field order is from low to high
 * @note I know it is not good, but it is easy to understand
 * @note in the past, I know endian is using in bytes, but now, I see it is using in bit fields too.
 * @note even in a uint8_t, the bit fields order is from low to high
 */

/*
         __     _           _      __                     _         __                       __
  _____ / /_   (_)____     (_)____/ /  _____ ___   _____ (_)____ _ / /  ____   ____   _____ / /_
 / ___// __ \ / // __ \   / // __  /  / ___// _ \ / ___// // __ `// /  / __ \ / __ \ / ___// __/
/ /__ / / / // // /_/ /  / // /_/ /  (__  )/  __// /   / // /_/ // /  / /_/ // /_/ // /   / /_
\___//_/ /_//_// .___/  /_/ \__,_/  /____/ \___//_/   /_/ \__,_//_/  / .___/ \____//_/    \__/
              /_/                                                   /_/
*/
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

typedef struct reg_microlcd_serial_port_address_low_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t dp_addr : 8;
        } bits;
    } reg;
} reg_microlcd_serial_port_address_low_t;

typedef struct reg_microlcd_serial_port_address_high_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t dp_addr : 4;
            uint8_t : 3;
            uint8_t read_request : 1;
        } bits;
    } reg;
} reg_microlcd_serial_port_address_high_t;

typedef struct reg_microlcd_serial_port_data_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t dp_data : 8;
        } bits;
    } reg;
} reg_microlcd_serial_port_data_t;

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

/*
                       __
   _____ _____ ____ _ / /____ _ _____
  / ___// ___// __ `// // __ `// ___/
 (__  )/ /__ / /_/ // // /_/ // /
/____/ \___/ \__,_//_/ \__,_//_/

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

/*
    _                                 _                         _      __                                                       __   _
   (_)____   _____ ____   ____ ___   (_)____   ____ _   _   __ (_)____/ /___   ____     ____   _____ ____   ____   ___   _____ / /_ (_)___   _____
  / // __ \ / ___// __ \ / __ `__ \ / // __ \ / __ `/  | | / // // __  // _ \ / __ \   / __ \ / ___// __ \ / __ \ / _ \ / ___// __// // _ \ / ___/
 / // / / // /__ / /_/ // / / / / // // / / // /_/ /   | |/ // // /_/ //  __// /_/ /  / /_/ // /   / /_/ // /_/ //  __// /   / /_ / //  __/(__  )
/_//_/ /_/ \___/ \____//_/ /_/ /_//_//_/ /_/ \__, /    |___//_/ \__,_/ \___/ \____/  / .___//_/    \____// .___/ \___//_/    \__//_/ \___//____/
                                            /____/                                  /_/                 /_/
*/

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

/**
 * @brief conflicting with datasheet
 */
typedef struct reg_pattern_generator_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pattern_generator_en : 1;
            uint8_t : 1;
            uint8_t pattern_generator_type : 2;
            uint8_t : 4;
        } bits;
    } reg;
} reg_pattern_generator_t;

/*
                   __   __                                                               __
    ____   ____ _ / /_ / /_ ___   _____ ____     ____ _ ___   ____   ___   _____ ____ _ / /_ ____   _____
   / __ \ / __ `// __// __// _ \ / ___// __ \   / __ `// _ \ / __ \ / _ \ / ___// __ `// __// __ \ / ___/
  / /_/ // /_/ // /_ / /_ /  __// /   / / / /  / /_/ //  __// / / //  __// /   / /_/ // /_ / /_/ // /
 / .___/ \__,_/ \__/ \__/ \___//_/   /_/ /_/   \__, / \___//_/ /_/ \___//_/    \__,_/ \__/ \____//_/
/_/                                           /____/
*/

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

typedef struct reg_pattern_gen_vsync_back_porch_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_vbporch : 8;
        } bits;
    } reg;
} reg_pattern_gen_vsync_back_porch_t;

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

typedef struct reg_pattern_gen_hsync_front_porch_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_hfporch : 8;
        } bits;
    } reg;
} reg_pattern_gen_hsync_front_porch_t;

typedef struct reg_pattern_gen_hsync_back_porch_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_hbporch : 8;
        } bits;
    } reg;
} reg_pattern_gen_hsync_back_porch_t;

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

typedef struct reg_pattern_gen_grayramp_step_low_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_step_low : 8;
        } bits;
    } reg;
} reg_pattern_gen_grayramp_step_low_t;

typedef struct reg_pattern_gen_grayramp_step_high_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_step_high : 8;
        } bits;
    } reg;
} reg_pattern_gen_grayramp_step_high_t;

typedef struct reg_pattern_gen_horizontal_cross_hatch_y_offset_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_h_y_offset : 8;
        } bits;
    } reg;
} reg_pattern_gen_horizontal_cross_hatch_y_offset_t;

typedef struct reg_pattern_gen_horizontal_cross_hatch_y_on_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_h_y_on : 8;
        } bits;
    } reg;
} reg_pattern_gen_horizontal_cross_hatch_y_on_t;

typedef struct reg_pattern_gen_horizontal_cross_hatch_y_off_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_h_y_off : 8;
        } bits;
    } reg;
} reg_pattern_gen_horizontal_cross_hatch_y_off_t;

typedef struct reg_pattern_gen_horizontal_cross_hatch_x_offset_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_h_x_offset : 8;
        } bits;
    } reg;
} reg_pattern_gen_horizontal_cross_hatch_x_offset_t;

typedef struct reg_pattern_gen_horizontal_cross_hatch_x_on_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_h_x_on : 8;
        } bits;
    } reg;
} reg_pattern_gen_horizontal_cross_hatch_x_on_t;

typedef struct reg_pattern_gen_horizontal_cross_hatch_x_off_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_h_x_off : 8;
        } bits;
    } reg;
} reg_pattern_gen_horizontal_cross_hatch_x_off_t;

typedef struct reg_pattern_gen_vertical_cross_hatch_y_offset_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_v_y_offset : 8;
        } bits;
    } reg;
} reg_pattern_gen_vertical_cross_hatch_y_offset_t;

typedef struct reg_pattern_gen_vertical_cross_hatch_y_on_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_v_y_on : 8;
        } bits;
    } reg;
} reg_pattern_gen_vertical_cross_hatch_y_on_t;

typedef struct reg_pattern_gen_vertical_cross_hatch_y_off_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_v_y_off : 8;
        } bits;
    } reg;
} reg_pattern_gen_vertical_cross_hatch_y_off_t;

typedef struct reg_pattern_gen_vertical_cross_hatch_x_offset_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_v_x_offset : 8;
        } bits;
    } reg;
} reg_pattern_gen_vertical_cross_hatch_x_offset_t;

typedef struct reg_pattern_gen_vertical_cross_hatch_x_on_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_v_x_on : 8;
        } bits;
    } reg;
} reg_pattern_gen_vertical_cross_hatch_x_on_t;

typedef struct reg_pattern_gen_vertical_cross_hatch_x_off_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t pgen_v_x_off : 8;
        } bits;
    } reg;
} reg_pattern_gen_vertical_cross_hatch_x_off_t;

typedef struct reg_led_control_t
{
    uint16_t addr;
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t blue_LED_en : 1;
            uint8_t green_LED_en : 1;
            uint8_t red_LED_en : 1;
            uint8_t : 5;
        } bits;
    } reg;
} reg_led_control_t;

/*
                      ____  ___   ___      __
  ____  _   __ ____  / __ \|__ \ <  /     / /_
 / __ \| | / // __ \/ /_/ /__/ / / /     / __/
/ /_/ /| |/ // /_/ /\__, // __/ / /     / /_
\____/ |___// .___//____//____//_/______\__/
           /_/                   /_____/
*/

struct ovp921_t
{
    // chip ID and serial port
    reg_chipid_t chipid;
    reg_microlcd_serial_port_address_low_t microlcd_serial_port_address_low;
    reg_microlcd_serial_port_address_high_t microlcd_serial_port_address_high;
    reg_microlcd_serial_port_data_t microlcd_serial_port_data;
    
    reg_chipid2_t chipid2;
    // pattern generator
    reg_pattern_generator_t pattern_generator;
    reg_pattern_gen_grayramp_step_low_t pattern_gen_grayramp_step_low;
    reg_pattern_gen_grayramp_step_high_t pattern_gen_grayramp_step_high;
    reg_pattern_gen_red_data_t pattern_gen_red_data;
    reg_pattern_gen_green_data_t pattern_gen_green_data;
    reg_pattern_gen_blue_data_t pattern_gen_blue_data;
    reg_pattern_gen_vsync_width_t pattern_gen_vsync_width;
    reg_pattern_gen_vsync_front_porch_t pattern_gen_vsync_front_porch;
    reg_pattern_gen_vsync_back_porch_t pattern_gen_vsync_back_porch;
    reg_pattern_gen_hsync_width_t pattern_gen_hsync_width;
    reg_pattern_gen_hsync_front_porch_t pattern_gen_hsync_front_porch;
    reg_pattern_gen_hsync_back_porch_t pattern_gen_hsync_back_porch;
    reg_pattern_gen_horizontal_cross_hatch_y_offset_t pattern_gen_horizontal_cross_hatch_y_offset;
    reg_pattern_gen_horizontal_cross_hatch_y_on_t pattern_gen_horizontal_cross_hatch_y_on;
    reg_pattern_gen_horizontal_cross_hatch_y_off_t pattern_gen_horizontal_cross_hatch_y_off;
    reg_pattern_gen_horizontal_cross_hatch_x_offset_t pattern_gen_horizontal_cross_hatch_x_offset;
    reg_pattern_gen_horizontal_cross_hatch_x_on_t pattern_gen_horizontal_cross_hatch_x_on;
    reg_pattern_gen_horizontal_cross_hatch_x_off_t pattern_gen_horizontal_cross_hatch_x_off;
    reg_pattern_gen_vertical_cross_hatch_y_offset_t pattern_gen_vertical_cross_hatch_y_offset;
    reg_pattern_gen_vertical_cross_hatch_y_on_t pattern_gen_vertical_cross_hatch_y_on;
    reg_pattern_gen_vertical_cross_hatch_y_off_t pattern_gen_vertical_cross_hatch_y_off;
    reg_pattern_gen_vertical_cross_hatch_x_offset_t pattern_gen_vertical_cross_hatch_x_offset;
    reg_pattern_gen_vertical_cross_hatch_x_on_t pattern_gen_vertical_cross_hatch_x_on;
    reg_pattern_gen_vertical_cross_hatch_x_off_t pattern_gen_vertical_cross_hatch_x_off;

    // light source control
    reg_led_control_t led_control;
};

#endif
