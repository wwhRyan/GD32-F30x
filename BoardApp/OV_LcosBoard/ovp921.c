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
    .pattern_generator = {
        .addr = 0x0100,
        .reg.bits.pattern_generator_type = 1,
        .reg.bits.pattern_generator_en = 1,
    },
};

extern const SoftwareI2C ovp921_i2c;

void show_test_pattern()
{
    ISoftwareI2CRegWrite(&ovp921_i2c, OVP921_SCCB_ADDRESS_WRITE, ovp921.pattern_generator.addr,
                         REG_ADDR_2BYTE, (uint8_t *)&ovp921.pattern_generator.reg.raw, 1, SCCB_DELAY_TIME);
}
