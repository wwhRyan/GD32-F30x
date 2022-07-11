/**
 * @file ovp921.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-04-12
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#ifndef OVP921_H
#define OVP921_H

#include "ovp921_reg.h"
#include "main.h"
#include "i2c.h"

void show_solid_color_pattern(uint8_t red, uint8_t green, uint8_t blue);

void gray_ramp_pattern(void);
void checkerboard_pattern(void);
void get_chipid(void);

void off_pattern(void);
uint8_t get_reg(uint16_t reg_addr);
void set_reg(uint16_t reg_addr, uint8_t reg_val);
void omnivision_lcos_init(void);
bool get_ovp921_status(void);
void vertical_flip(bool enable);
void horizontal_flip(bool enable);

bool update_anf(int idx, const uint8_t *p_anf, int anf_size);
bool ovp921_erase(uint32_t addr);
void get_anf_version(char *p_version, int anf_idx);
void get_firmware_version(char *p_version);

#endif
