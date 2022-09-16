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
#include "BoardInit.h"
#include "Common.h"
#include "main.h"


extern const SoftwareI2C ovp921_i2c;
extern int OVP2200_921_pgen_v4_05_size;
extern const uint8_t OVP2200_921_pgen_v4_05[];

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
    // microlcd control and status
    .microlcd_control = {
        .addr = MICROLCD_CONTROL_ADDR,
    },
    .ulcd_xydim_h = {
        .addr = MICROLCD_CONTROL_ADDR + 1,
    },
    .horizontal_mode = {
        .addr = 0x000E,
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

void change_anf(uint8_t num)
{
    if (get_reg(0x0046) != num) {
        set_reg(0x0046, 0xA0 | num);
    } else
        return;
    vTaskDelay(100); // wait for the register to be updated in first change.
}

void show_solid_color_pattern(uint8_t red, uint8_t green, uint8_t blue)
{
    change_anf(2);
    ovp921.pattern_gen_red_data.reg_pgen_data_r = red;
    ovp921.pattern_gen_green_data.reg_pgen_data_g = green;
    ovp921.pattern_gen_blue_data.reg_pgen_data_b = blue;

    set_reg(ovp921.pattern_gen_red_data.addr,
        ovp921.pattern_gen_red_data.byte);
    set_reg(ovp921.pattern_gen_green_data.addr,
        ovp921.pattern_gen_green_data.byte);
    set_reg(ovp921.pattern_gen_blue_data.addr,
        ovp921.pattern_gen_blue_data.byte);

    ovp921.pattern_generator.pattern_generator_type = 0;
    ovp921.pattern_generator.pattern_generator_en = 1;
    set_reg(ovp921.pattern_generator.addr,
        ovp921.pattern_generator.byte);
    vTaskDelay(1);
    set_reg(0x4E, 0xAA);
}

void gray_ramp_pattern()
{
    change_anf(2);
    ovp921.pattern_generator.byte = get_reg(ovp921.pattern_generator.addr);
    if (ovp921.pattern_generator.pattern_generator_type == 1 && ovp921.pattern_generator.pattern_generator_en == 1)
        return;
    show_solid_color_pattern(0xFF, 0xFF, 0xFF);

    ovp921.pattern_generator.pattern_generator_type = 1;
    ovp921.pattern_generator.pattern_generator_en = 1;
    set_reg(ovp921.pattern_generator.addr,
        ovp921.pattern_generator.byte);
    vTaskDelay(1);
    set_reg(0x4E, 0xAA);
}

void checkerboard_pattern()
{
    change_anf(2);
    ovp921.pattern_generator.byte = get_reg(ovp921.pattern_generator.addr);
    if (ovp921.pattern_generator.pattern_generator_type == 2 && ovp921.pattern_generator.pattern_generator_en == 1)
        return;
    ovp921.pattern_gen_horizontal_cross_hatch_y_offset.byte = 0x00;
    ovp921.pattern_gen_horizontal_cross_hatch_y_on.byte = 0x80;
    ovp921.pattern_gen_horizontal_cross_hatch_y_off.byte = 0x80;
    ovp921.pattern_gen_horizontal_cross_hatch_x_offset.byte = 0x00;
    ovp921.pattern_gen_horizontal_cross_hatch_x_on.byte = 0x80;
    ovp921.pattern_gen_horizontal_cross_hatch_x_off.byte = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_y_offset.byte = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_y_on.byte = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_y_off.byte = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_x_offset.byte = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_x_on.byte = 0x80;
    ovp921.pattern_gen_vertical_cross_hatch_x_off.byte = 0x80;

    show_solid_color_pattern(0xFF, 0xff, 0xff);

    set_reg(ovp921.pattern_gen_horizontal_cross_hatch_y_offset.addr,
        ovp921.pattern_gen_horizontal_cross_hatch_y_offset.byte);
    set_reg(ovp921.pattern_gen_horizontal_cross_hatch_y_on.addr,
        ovp921.pattern_gen_horizontal_cross_hatch_y_on.byte);
    set_reg(ovp921.pattern_gen_horizontal_cross_hatch_y_off.addr,
        ovp921.pattern_gen_horizontal_cross_hatch_y_off.byte);
    set_reg(ovp921.pattern_gen_horizontal_cross_hatch_x_offset.addr,
        ovp921.pattern_gen_horizontal_cross_hatch_x_offset.byte);
    set_reg(ovp921.pattern_gen_horizontal_cross_hatch_x_on.addr,
        ovp921.pattern_gen_horizontal_cross_hatch_x_on.byte);
    set_reg(ovp921.pattern_gen_horizontal_cross_hatch_x_off.addr,
        ovp921.pattern_gen_horizontal_cross_hatch_x_off.byte);
    set_reg(ovp921.pattern_gen_vertical_cross_hatch_y_offset.addr,
        ovp921.pattern_gen_vertical_cross_hatch_y_offset.byte);
    set_reg(ovp921.pattern_gen_vertical_cross_hatch_y_on.addr,
        ovp921.pattern_gen_vertical_cross_hatch_y_on.byte);
    set_reg(ovp921.pattern_gen_vertical_cross_hatch_y_off.addr,
        ovp921.pattern_gen_vertical_cross_hatch_y_off.byte);
    set_reg(ovp921.pattern_gen_vertical_cross_hatch_x_offset.addr,
        ovp921.pattern_gen_vertical_cross_hatch_x_offset.byte);
    set_reg(ovp921.pattern_gen_vertical_cross_hatch_x_on.addr,
        ovp921.pattern_gen_vertical_cross_hatch_x_on.byte);
    set_reg(ovp921.pattern_gen_vertical_cross_hatch_x_off.addr,
        ovp921.pattern_gen_vertical_cross_hatch_x_off.byte);

    ovp921.pattern_generator.pattern_generator_type = 2;
    ovp921.pattern_generator.pattern_generator_en = 1;
    set_reg(ovp921.pattern_generator.addr,
        ovp921.pattern_generator.byte);
    vTaskDelay(1);
    set_reg(0x4E, 0xAA);
}

void get_chipid()
{
    ovp921.chipid.byte = get_reg(ovp921.chipid.addr);

    ovp921.chipid2.byte = get_reg(ovp921.chipid2.addr);
}

void off_pattern()
{
    ovp921.pattern_generator.pattern_generator_en = 0;
    set_reg(ovp921.pattern_generator.addr,
        ovp921.pattern_generator.byte);
    change_anf(1);
}

uint8_t get_reg(uint16_t reg_addr)
{
    uint8_t reg_val = 0;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ISoftwareI2CRegRead(&ovp921_i2c, OVP921_SCCB_ADDRESS_READ, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, SCCB_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return reg_val;
}

bool set_reg(uint16_t reg_addr, uint8_t reg_val)
{
    bool ret;
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);

    ret = ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, reg_addr,
        REG_ADDR_2BYTE, (uint8_t*)&reg_val, 1, SCCB_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return ret;
}

bool get_reg_block(uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;

    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);
    ret = ISoftwareI2CRegRead(&ovp921_i2c, OVP921_SCCB_ADDRESS_READ, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, SCCB_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return ret;
}

bool set_reg_block(uint16_t reg_addr, uint8_t* reg_val, size_t size)
{
    bool ret;
    xSemaphoreTake(i2c_Semaphore, (TickType_t)0xFFFF);

    ret = ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, reg_addr,
        REG_ADDR_2BYTE, reg_val, size, SCCB_DELAY_TIME);
    xSemaphoreGive(i2c_Semaphore);
    return ret;
}

void omnivision_lcos_init()
{
    DelayMs(10);
    gpio_bit_reset(OVP921_RESET_PORT, OVP921_RESET_PIN);
}

void vertical_flip(bool enable)
{
    // read vertical flip status
    ovp921.microlcd_serial_port_address_low.dp_addr = 0x06;
    ovp921.microlcd_serial_port_address_high.read_request = 1;
    set_reg(ovp921.microlcd_serial_port_address_low.addr, ovp921.microlcd_serial_port_address_low.byte);
    set_reg(ovp921.microlcd_serial_port_address_high.addr, ovp921.microlcd_serial_port_address_high.byte);

    while ((0x80 & get_reg(ovp921.microlcd_serial_port_address_high.addr)) != 0x00) {
        vTaskDelay(1);
    }

    uint8_t dp_data = get_reg(ovp921.microlcd_serial_port_data.addr);

    ovp921.microlcd_serial_port_address_low.dp_addr = 0x06;
    ovp921.microlcd_serial_port_address_high.byte = 0x00;

    if (enable) {
        set_reg(ovp921.microlcd_serial_port_address_low.addr, ovp921.microlcd_serial_port_address_low.byte);
        set_reg(ovp921.microlcd_serial_port_address_high.addr, ovp921.microlcd_serial_port_address_high.byte);
        set_reg(ovp921.microlcd_serial_port_data.addr, dp_data | 0x40);
    } else {
        set_reg(ovp921.microlcd_serial_port_address_low.addr, ovp921.microlcd_serial_port_address_low.byte);
        set_reg(ovp921.microlcd_serial_port_address_high.addr, ovp921.microlcd_serial_port_address_high.byte);
        set_reg(ovp921.microlcd_serial_port_data.addr, dp_data & (uint8_t)(~0x40));
    }
}

void horizontal_flip(bool enable)
{
    ovp921.horizontal_mode.horizontal_filp = enable;
    set_reg(ovp921.horizontal_mode.addr, (uint8_t)ovp921.horizontal_mode.byte);
}

bool get_ovp921_status()
{
    if (get_reg(0x004C) == 0x17) {
        return true;
    } else {
        return false;
    }
}

#define ANF_BASE_ADDR 0x8000
#define ANF_ADDR(anf_idx) (ANF_BASE_ADDR + 0x2000 * (anf_idx - 1))

bool halting_internal_mcu()
{
    bool ret = true;
    ret &= set_reg(0x01ff, 0x05);
    ret &= set_reg(0x7ffe, 0x01);
    ret &= set_reg(0x8000, 0x00);
    ret &= set_reg(0x8001, 0x75);
    ret &= set_reg(0x8002, 0x87);
    ret &= set_reg(0x8003, 0x02);
    ret &= set_reg(0x8004, 0x00);
    ret &= set_reg(0x8005, 0x00);
    ret &= set_reg(0x7ffe, 0x00);
    ret &= set_reg(0x7fc0, 0x03);
    return ret;
}

#define RETRY_TIMES 10

uint8_t get_reg_delay(uint16_t reg_addr)
{
    uint8_t reg = get_reg(reg_addr);
    DelayUs(400);
    return reg;
}

bool set_reg_delay(uint16_t reg_addr, uint8_t reg_val)
{
    bool ret = set_reg(reg_addr, reg_val);
    DelayUs(400);
    return ret;
}

bool ovp921_erase(uint32_t addr) // erase one page
{

    set_reg_delay(0x0210, 0x02);
    vTaskDelay(5 + 5);
    set_reg_delay(0x0210, 0x03);

    int times = 0;
    while (get_reg_delay(0x021d) != 0x00 && times < RETRY_TIMES) {
        times++;
        vTaskDelay(5 + 5);
    }
    if (times >= RETRY_TIMES) {
        return false;
    }

    set_reg_delay(0x0213, 0x00);
    set_reg_delay(0x0218, 0x06);
    vTaskDelay(15 + 5);
    set_reg_delay(0x0213, 0x01);
    set_reg_delay(0x0218, 0x01);
    set_reg_delay(0x0218, 0x00);
    vTaskDelay(30 + 5);
    set_reg_delay(0x0213, 0x00);
    set_reg_delay(0x0218, 0x06);
    vTaskDelay(15 + 5);

    uint8_t read_fifo_times = 0;
    read_fifo_times = get_reg_delay(0x021e);
    for (uint8_t i = 0; i < read_fifo_times; i++) {
        get_reg_delay(0x0218);
    }

    set_reg_delay(0x0213, 0x04);
    set_reg_delay(0x0218, 0x20);

    set_reg_delay(0x0218, (uint8_t)((addr >> 16) & 0xFF));
    set_reg_delay(0x0218, (uint8_t)((addr >> 8) & 0xFF));
    set_reg_delay(0x0218, (uint8_t)(addr & 0xFF));
    vTaskDelay(300 + 25);

    set_reg_delay(0x0213, 0x00);
    set_reg_delay(0x0218, 0x00);

    vTaskDelay(300 + 5);

    return true;
}

bool ovp921_write(uint32_t addr, const uint8_t* data, size_t size)
{
    bool ret = true;
    ret &= set_reg_delay(0x0213, 0x00);
    ret &= set_reg_delay(0x0218, 0x06);
    vTaskDelay(105 + 5);
    ret &= set_reg_delay(0x0213, 0x01);
    ret &= set_reg_delay(0x0218, 0x05);
    ret &= set_reg_delay(0x0218, 0x00);
    uint8_t read_fifo_times = 0;

    read_fifo_times = get_reg_delay(0x021e);
    for (uint8_t i = 0; i < read_fifo_times; i++) {
        get_reg_delay(0x0218);
    }

    ret &= set_reg_delay(0x0213, 0x04);
    ret &= set_reg_delay(0x0218, 0x02);

    ret &= set_reg_delay(0x0218, (uint8_t)((addr >> 16) & 0xFF));
    ret &= set_reg_delay(0x0218, (uint8_t)((addr >> 8) & 0xFF));
    ret &= set_reg_delay(0x0218, (uint8_t)(addr & 0xFF));

    for (size_t i = 0; i < size; i++) {
        ret &= set_reg_delay(0x0218, *(data + i));
        get_reg_delay(0x0218);
    }
    ret &= set_reg_delay(0x0213, 0x00);
    ret &= set_reg_delay(0x0218, 0x00);
    vTaskDelay(150 + 10);
    return ret;
}

#define FLASH_PAGE_SIZE 0x100
bool ovp921_write_page(uint32_t addr, const uint8_t* data)
{
    bool ret = true;
    ret &= set_reg_delay(0x0213, 0x00);
    ret &= set_reg_delay(0x0218, 0x06);
    vTaskDelay(105 + 5);
    ret &= set_reg_delay(0x0213, 0x01);
    ret &= set_reg_delay(0x0218, 0x05);
    ret &= set_reg_delay(0x0218, 0x00);
    uint8_t read_fifo_times = 0;

    read_fifo_times = get_reg_delay(0x021e);
    for (uint8_t i = 0; i < read_fifo_times; i++) {
        get_reg_delay(0x0218);
    }

    ret &= set_reg_delay(0x0213, 0x04);
    ret &= set_reg_delay(0x0218, 0x02);

    ret &= set_reg_delay(0x0218, (uint8_t)((addr >> 16) & 0xFF));
    ret &= set_reg_delay(0x0218, (uint8_t)((addr >> 8) & 0xFF));
    ret &= set_reg_delay(0x0218, (uint8_t)(addr & 0xFF));

    for (size_t i = 0; i < FLASH_PAGE_SIZE; i++) {
        ret &= set_reg_delay(0x0218, *(data + i));
        get_reg_delay(0x0218);
    }
    ret &= set_reg_delay(0x0213, 0x00);
    ret &= set_reg_delay(0x0218, 0x00);
    vTaskDelay(150 + 10);
    return ret;
}

bool update_anf(int idx, const uint8_t* p_anf, int anf_size)
{
    bool ret = true;
    static uint8_t idx_check;
    static uint8_t iteration = 0;
    E_assert(anf_size % 0x100 == 0); // must be multiple of 256
    if (!get_sig(sys_sig, sig_update_anf)) {
        ret &= halting_internal_mcu();
        if (ovp921_erase(ANF_ADDR(idx)) == false) {
            ULOG_ERROR("erase failed\n");
            return false;
        }
        if (ovp921_erase(ANF_ADDR(idx) + 0x1000) == false) {
            ULOG_ERROR("erase failed\n");
            return false;
        }
        set_sig(sys_sig, sig_update_anf, true);
        idx_check = idx;
        iteration = 0;
    }

    if (idx != idx_check)
        return false;

    ret &= ovp921_write_page(ANF_ADDR(idx) + iteration * 0x100, p_anf);
    ULOG_DEBUG(" %s %#x \n", __func__, ANF_ADDR(idx) + iteration * 0x100);
    iteration++;

    if (iteration == 0x2000 / 0x100) {
        clear_sig(sys_sig, sig_update_anf);
        ULOG_INFO("update anf success!\n");
    }
    return ret;
}

bool update_firmware(const uint8_t* p_data, int size)
{
    bool ret = true;
    static uint8_t iteration = 0;
    E_assert(size % 0x100 == 0); // must be multiple of 256
    if (!get_sig(sys_sig, sig_update_firmware)) {
        ret &= halting_internal_mcu();
        for (size_t i = 0; i < 8; i++) {
            if (ovp921_erase(i * 0x1000) == false) {
                ULOG_ERROR("erase failed\n");
                return false;
            }
        }

        /* Temporarily set to the firmware size */
        // TODO: update the size.
        uint16_t check_data = BSWAP_16(0x4000);
        ovp921_write(0x7ffe, (uint8_t*)&check_data, 2);

        set_sig(sys_sig, sig_update_firmware, true);
        iteration = 0;
    }

    ret &= ovp921_write_page(iteration * 0x100, p_data);
    ULOG_DEBUG(" %s %#x \n", __func__, iteration * 0x100);
    iteration++;

    ULOG_INFO("update firmware %04X.\n", iteration * 0x100);

    return ret;
}

uint8_t ovp921_read_flash(uint32_t addr)
{
    uint8_t ret = 0;
    set_reg(0x0210, 0x02);
    vTaskDelay(5 + 5);
    set_reg_delay(0x0210, 0x03);
    int times = 0;
    while (get_reg_delay(0x021d) != 0x00 && times < RETRY_TIMES) {
        times++;
        vTaskDelay(5 + 5);
    }
    if (times >= RETRY_TIMES) {
        return false;
    }

    set_reg_delay(0x0213, 0x04);
    set_reg_delay(0x0218, 0x03);
    set_reg_delay(0x0218, (uint8_t)((addr >> 16) & 0xFF));
    set_reg_delay(0x0218, (uint8_t)((addr >> 8) & 0xFF));
    set_reg_delay(0x0218, (uint8_t)(addr & 0xFF));

    uint8_t read_fifo_times = 0;

    read_fifo_times = get_reg_delay(0x021e);
    for (uint8_t i = 0; i < read_fifo_times; i++) {
        get_reg_delay(0x0218);
    }

    set_reg_delay(0x0218, 0x00);
    set_reg_delay(0x0218, 0x00);
    ret = get_reg_delay(0x0218);
    set_reg_delay(0x0213, 0x00);
    set_reg_delay(0x0218, 0x00);

    return ret;
}

void get_anf_version(char* p_version, int anf_idx)
{
    if (ovp921_read_flash(ANF_ADDR(anf_idx) + 0x100) != 0xff) {
        uint16_t year = ovp921_read_flash(ANF_ADDR(anf_idx) + 0x84) + 2000;
        uint8_t day = ovp921_read_flash(ANF_ADDR(anf_idx) + 0x83);
        uint8_t month = ovp921_read_flash(ANF_ADDR(anf_idx) + 0x82);
        uint8_t minor = ovp921_read_flash(ANF_ADDR(anf_idx) + 0x81);
        uint8_t major = ovp921_read_flash(ANF_ADDR(anf_idx) + 0x80);
        // sprintf(p_version, "%d-%02d-%02d.%d.%d", year, month, day, major, minor);
        sprintf(p_version, "%d.%d", major, minor);
    } else {
        strcpy(p_version, "NULL");
    }
}

void get_firmware_version(char* p_version)
{

    // uint16_t year = ovp921_read_flash(0x004d) + 2000;
    // uint8_t day = ovp921_read_flash(0x000b);
    // uint8_t month = ovp921_read_flash(0x000a);
    uint8_t major = ovp921_read_flash(0x004d);
    uint8_t minor = ovp921_read_flash(0x004e);
    // sprintf(p_version, "%d-%02d-%02d.%d.%d", year, month, day, major, minor);
    sprintf(p_version, "%d.%d", major, minor);
}
