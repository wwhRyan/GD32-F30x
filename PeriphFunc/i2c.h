/**
 * @file i2c.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-10-14
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#ifndef __I2C_H
#define __I2C_H

#include "gd32f30x.h"
#include "stdbool.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXCUTE_ONCE(x)                     \
    do {                                   \
        static uint8_t once##__LINE__ = 0; \
        if (once##__LINE__ == 0) {         \
            once##__LINE__ = 1;            \
            x;                             \
        }                                  \
    } while (0)

#ifdef DEBUG
#define E_assert(condition)                                \
    do {                                                   \
        if (condition)                                     \
            ;                                              \
        else {                                             \
            printf("\nERROR %s:%d\n", __FILE__, __LINE__); \
            for (;;)                                       \
                ;                                          \
        }                                                  \
    } while (0)
#else
#define E_assert(condition)
#endif

#define REG_ADDR_1BYTE 1
#define REG_ADDR_2BYTE 2

#define I2C_DELAY_TIME 5 /* (((1000)/100)*2) = 5 100hz */

typedef struct __SoftwareI2C {
    uint32_t sda_port;
    uint32_t scl_port;
    uint32_t sda_pin;
    uint32_t scl_pin;
    uint32_t delay_time;
} SoftwareI2C;

void DelayUs(uint32_t nus);
void DelayMs(uint32_t nms);

void INewSoftwareI2C(const SoftwareI2C* psI2c);
bool ISoftwareI2CRegWrite(const SoftwareI2C* psI2c, uint16_t dev_addr, uint16_t reg_addr,
    uint8_t reg_addr_type, uint8_t* pData, uint16_t size, uint32_t timeout);
bool ISoftwareI2CRegRead(const SoftwareI2C* psI2c, uint16_t dev_addr, uint16_t reg_addr,
    uint8_t reg_addr_type, uint8_t* pData, uint16_t size, uint32_t timeout);

#endif // __I2C_H
