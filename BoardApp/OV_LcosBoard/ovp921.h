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

void red_pattern(void);
void blue_pattern(void);
void green_pattern(void);

void gray_ramp_pattern(void);
void checkerboard_pattern(void);
void get_chipid(void);

void off_pattern(void);

#endif
