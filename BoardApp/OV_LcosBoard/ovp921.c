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

/**
 * @brief good thing is we no need to remember the register address and member address
 * @note bad thind is we have to enter lots of text.
 * @note if the register number is thousands, we have to transfer ovp921 struct to const,
 *      and make the union become pointer. addr become const value.
 *
 */
struct ovp921_t ovp921 = {
    // chip ID and serial port
    .chipid = {
        .addr = CHIP_ID_ADDR,
    },
    .microlcd_serial_port_address_low = {
        .addr = CHIP_ID_ADDR + 1,
    },
    .microlcd_serial_port_address_high = {
        .addr = CHIP_ID_ADDR + 2,
    },
    .microlcd_serial_port_data = {
        .addr = CHIP_ID_ADDR + 3,
    },
    .chipid2 = {
        .addr = MISCELLANEOUS_ADDR + 0x0f,
    },

    // pattern generator
    .pattern_generator = {
        .addr = PATTERN_GENERATOR_ADDR,
    },
    .pattern_gen_grayramp_step_low = {
        .addr = PATTERN_GENERATOR_ADDR + 0x01,
    },
    .pattern_gen_grayramp_step_high = {
        .addr = PATTERN_GENERATOR_ADDR + 0x02,
    },
    .pattern_gen_red_data = {
        .addr = PATTERN_GENERATOR_ADDR + 0x03,
    },
    .pattern_gen_green_data = {
        .addr = PATTERN_GENERATOR_ADDR + 0x04,
    },
    .pattern_gen_blue_data = {
        .addr = PATTERN_GENERATOR_ADDR + 0x05,
    },
    .pattern_gen_vsync_width = {
        .addr = PATTERN_GENERATOR_ADDR + 0x06,
    },
    .pattern_gen_vsync_front_porch = {
        .addr = PATTERN_GENERATOR_ADDR + 0x07,
    },
    .pattern_gen_vsync_back_porch = {
        .addr = PATTERN_GENERATOR_ADDR + 0x08,
    },
    .pattern_gen_hsync_width = {
        .addr = PATTERN_GENERATOR_ADDR + 0x09,
    },
    .pattern_gen_hsync_front_porch = {
        .addr = PATTERN_GENERATOR_ADDR + 0x0a,
    },
    .pattern_gen_hsync_back_porch = {
        .addr = PATTERN_GENERATOR_ADDR + 0x0b,
    },
    .pattern_gen_horizontal_cross_hatch_y_offset = {
        .addr = PATTERN_GENERATOR_ADDR + 0x0c,
    },
    .pattern_gen_horizontal_cross_hatch_y_on = {
        .addr = PATTERN_GENERATOR_ADDR + 0x0d,
    },
    .pattern_gen_horizontal_cross_hatch_y_off = {
        .addr = PATTERN_GENERATOR_ADDR + 0x0e,
    },
    .pattern_gen_horizontal_cross_hatch_x_offset = {
        .addr = PATTERN_GENERATOR_ADDR + 0x0f,
    },
    .pattern_gen_horizontal_cross_hatch_x_on = {
        .addr = PATTERN_GENERATOR_ADDR + 0x10,
    },
    .pattern_gen_horizontal_cross_hatch_x_off = {
        .addr = PATTERN_GENERATOR_ADDR + 0x11,
    },
    .pattern_gen_vertical_cross_hatch_y_offset = {
        .addr = PATTERN_GENERATOR_ADDR + 0x12,
    },
    .pattern_gen_vertical_cross_hatch_y_on = {
        .addr = PATTERN_GENERATOR_ADDR + 0x13,
    },
    .pattern_gen_vertical_cross_hatch_y_off = {
        .addr = PATTERN_GENERATOR_ADDR + 0x14,
    },
    .pattern_gen_vertical_cross_hatch_x_offset = {
        .addr = PATTERN_GENERATOR_ADDR + 0x15,
    },
    .pattern_gen_vertical_cross_hatch_x_on = {
        .addr = PATTERN_GENERATOR_ADDR + 0x16,
    },
    .pattern_gen_vertical_cross_hatch_x_off = {
        .addr = PATTERN_GENERATOR_ADDR + 0x17,
    },

    // light source control
    .led_control = {
        .addr = LIGHT_SOURCE_CONTROL_ADDR,
    },
};

extern const SoftwareI2C ovp921_i2c;

void show_solid_color_pattern(uint8_t red, uint8_t green, uint8_t blue)
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

void gray_ramp_pattern()
{
    show_solid_color_pattern(0xFF, 0xFF, 0xFF);

    ovp921.pattern_generator.reg.bits.pattern_generator_type = 1;
    ovp921.pattern_generator.reg.bits.pattern_generator_en = 1;
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_generator.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_generator.reg.raw, 1, SCCB_DELAY_TIME);
}

void checkerboard_pattern()
{
    ovp921.pattern_gen_horizontal_cross_hatch_y_offset.reg.raw = 0x00;
    ovp921.pattern_gen_horizontal_cross_hatch_y_on.reg.raw = 0x80;
    ovp921.pattern_gen_horizontal_cross_hatch_y_off.reg.raw = 0x80;
    ovp921.pattern_gen_horizontal_cross_hatch_x_offset.reg.raw = 0x00;
    ovp921.pattern_gen_horizontal_cross_hatch_x_on.reg.raw = 0x80;
    ovp921.pattern_gen_horizontal_cross_hatch_x_off.reg.raw = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_y_offset.reg.raw = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_y_on.reg.raw = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_y_off.reg.raw = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_x_offset.reg.raw = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_x_on.reg.raw = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_x_off.reg.raw = 0x80;

    show_solid_color_pattern(0xFF, 0xff, 0xff);

    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_horizontal_cross_hatch_y_offset.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_horizontal_cross_hatch_y_offset.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_horizontal_cross_hatch_y_on.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_horizontal_cross_hatch_y_on.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_horizontal_cross_hatch_y_off.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_horizontal_cross_hatch_y_off.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_horizontal_cross_hatch_x_offset.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_horizontal_cross_hatch_x_offset.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_horizontal_cross_hatch_x_on.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_horizontal_cross_hatch_x_on.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_horizontal_cross_hatch_x_off.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_horizontal_cross_hatch_x_off.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_vertical_cross_hatch_y_offset.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_vertical_cross_hatch_y_offset.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_vertical_cross_hatch_y_on.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_vertical_cross_hatch_y_on.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_vertical_cross_hatch_y_off.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_vertical_cross_hatch_y_off.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_vertical_cross_hatch_x_offset.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_vertical_cross_hatch_x_offset.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_vertical_cross_hatch_x_on.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_vertical_cross_hatch_x_on.reg.raw, 1, SCCB_DELAY_TIME);
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_gen_vertical_cross_hatch_x_off.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_gen_vertical_cross_hatch_x_off.reg.raw, 1, SCCB_DELAY_TIME);

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

uint8_t get_reg(uint16_t reg_addr)
{
    uint8_t reg_val = 0;
    ISoftwareI2CRegRead(&ovp921_i2c, OVP921_SCCB_ADDRESS_READ, reg_addr,
                        REG_ADDR_2BYTE, (uint8_t *)&reg_val, 1, SCCB_DELAY_TIME);
    return reg_val;
}

void set_reg(uint16_t reg_addr, uint8_t reg_val)
{
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, reg_addr,
                         REG_ADDR_2BYTE, (uint8_t *)&reg_val, 1, SCCB_DELAY_TIME);
}

void omnivision_lcos_init()
{
    delay_1ms(5);
    gpio_bit_reset(OVP921_RESET_PORT, OVP921_RESET_PIN);
}

void vertical_flip(bool enable)
{
    ovp921.microlcd_serial_port_address_low.reg.bits.dp_addr = 0x06;
    // ovp921.microlcd_serial_port_address_high.reg.bits.read_request = 1;
    // set_reg(ovp921.microlcd_serial_port_address_low.addr, ovp921.microlcd_serial_port_address_low.reg.raw);
    // set_reg(ovp921.microlcd_serial_port_address_high.addr, ovp921.microlcd_serial_port_address_high.reg.raw);

    // while ((0x80 & get_reg(ovp921.microlcd_serial_port_address_high.addr)) != 0x00)
    // {
    //     vTaskDelay(1);
    // }

    // dp_data = get_reg(ovp921.microlcd_serial_port_data.addr);

    ovp921.microlcd_serial_port_address_high.reg.raw = 0x00;

    if (enable)
    {
        set_reg(ovp921.microlcd_serial_port_address_low.addr, ovp921.microlcd_serial_port_address_low.reg.raw);
        set_reg(ovp921.microlcd_serial_port_address_high.addr, ovp921.microlcd_serial_port_address_high.reg.raw);
        set_reg(ovp921.microlcd_serial_port_data.addr, 0x40);
    }
    else
    {
        set_reg(ovp921.microlcd_serial_port_address_low.addr, ovp921.microlcd_serial_port_address_low.reg.raw);
        set_reg(ovp921.microlcd_serial_port_address_high.addr, ovp921.microlcd_serial_port_address_high.reg.raw);
        set_reg(ovp921.microlcd_serial_port_data.addr, 0x00);
    }
}

bool get_ovp921_status()
{
    if (get_reg(0x004C) == 0x17)
    {
        return true;
    }
    else
    {
        return false;
    }
}
