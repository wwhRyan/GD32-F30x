/*
 * @file: i2c.h
 * @author: gengliang (gengliang@appotronics.cn)
 * @brief:  
 * @date: 2021-06-04 
 */

#ifndef __I2C_H
#define __I2C_H

#include "gd32f30x.h"
#include "main.h"

#define REG_ADDR_1BYTE       1
#define REG_ADDR_2BYTE       2

typedef struct __SoftwareI2C{
    uint32_t sda_port;
    uint32_t scl_port;
    uint32_t sda_pin;
    uint32_t scl_pin;
    uint32_t delay_time;
}SoftwareI2C;

//  SoftwareI2C g_3555_SoftwareI2C;
//  SoftwareI2C g_A5931_SoftwareI2C;
//  SoftwareI2C g_31790_SoftwareI2C;
//  SoftwareI2C g_Eeprom_SoftwareI2C;

void DelayUs(uint32_t nus);

void INewSoftwareI2C(const SoftwareI2C *psI2c);
bool ISoftwareI2CRegWrite(const SoftwareI2C* psI2c, uint16_t dev_addr, uint16_t reg_addr, uint8_t reg_addr_type, uint8_t *pData, uint16_t size, uint32_t timeout);
bool ISoftwareI2CRegRead(const SoftwareI2C* psI2c, uint16_t dev_addr, uint16_t reg_addr, uint8_t reg_addr_type, uint8_t *pData, uint16_t size, uint32_t timeout);

#endif // __I2C_H

