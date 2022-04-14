/**
 * @file ovp921.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-04-12
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "ovp921.h"

struct ovp921_t ovp921 = {
    // chip ID and serial port
    .chipid = {
        .addr = 0x0000,
        .reg.raw = 0x00,
    },
    .chipid2 = {
        .addr = 0x004F,
        .reg.raw = 0x00,
    },
    // pattern generator
    .pattern_generator = {
        .addr = 0x0100,
        .reg.raw = 0x00,
    },
    .pattern_gen_red_data = {
        .addr = 0x0103,
        .reg.raw = 0x00,
    },
    .pattern_gen_green_data = {
        .addr = 0x0104,
        .reg.raw = 0x00,
    },
    .pattern_gen_blue_data = {
        .addr = 0x0105,
        .reg.raw = 0x00,
    },
};

extern const SoftwareI2C ovp921_i2c;

static void show_solid_color_pattern(uint8_t red, uint8_t green, uint8_t blue)
{
    ovp921.pattern_gen_red_data.reg.bits.reg_pgen_data_r = red;
    ovp921.pattern_gen_green_data.reg.bits.reg_pgen_data_g = green;
    ovp921.pattern_gen_blue_data.reg.bits.reg_pgen_data_b = blue;

    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_red_data.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_red_data.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_green_data.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_green_data.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_blue_data.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_blue_data.reg.raw, 1, SCCB_DELAY_TIME);

    ovp921.pattern_generator.reg.bits.pattern_generator_type = 0;
    ovp921.pattern_generator.reg.bits.pattern_generator_en = 1;
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_generator.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_generator.reg.raw, 1, SCCB_DELAY_TIME);
}

void red_pattern()
{
    show_solid_color_pattern(0xFF, 0x00, 0x00);
}

void blue_pattern()
{
    show_solid_color_pattern(0x00, 0x00, 0xff);
}

void green_pattern()
{
    show_solid_color_pattern(0x00, 0xff, 0x00);
}

void gray_ramp_pattern()
{
    ovp921.pattern_generator.reg.bits.pattern_generator_type = 1;
    ovp921.pattern_generator.reg.bits.pattern_generator_en = 1;
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_generator.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_generator.reg.raw, 1, SCCB_DELAY_TIME);
}

void checkerboard_pattern()
{
    ovp921.pattern_generator.reg.bits.pattern_generator_type = 2;
    ovp921.pattern_generator.reg.bits.pattern_generator_en = 1;
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_generator.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_generator.reg.raw, 1, SCCB_DELAY_TIME);
}

void get_chipid()
{
    ISoftwareI2CRegRead(&ovp921_i2c, OVP921_SCCB_ADDRESS_READ, ovp921.chipid.addr,
                        REG_ADDR_2BYTE, (uint8_t *)&ovp921.chipid.reg.raw, 1, SCCB_DELAY_TIME);

    ISoftwareI2CRegRead(&ovp921_i2c, OVP921_SCCB_ADDRESS_READ, ovp921.chipid2.addr,
                        REG_ADDR_2BYTE, (uint8_t *)&ovp921.chipid2.reg.raw, 1, SCCB_DELAY_TIME);
}

void off_pattern()
{
    ovp921.pattern_generator.reg.bits.pattern_generator_en = 0;
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_generator.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_generator.reg.raw, 1, SCCB_DELAY_TIME);
}
