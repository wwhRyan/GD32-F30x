/**
 * @file save_data.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-06-15
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#ifndef _SAVE_DATA_H_
#define _SAVE_DATA_H_

#include "main.h"

//round with uint32_t for crc calculation
// typedef struct nv_data_t
// {
//     uint32_t check_sum;
//     uint32_t magic_num;
//     float red;
//     float green;
//     float blue;
//     uint32_t light_source_time;
// } nv_data_t;

extern nv_data_t nv_data;
extern const uint32_t magic_number;

bool check_nv_data(nv_data_t *p_nv_data);
void save_nv_data_to_eeprom(nv_data_t *p_nv_data);
void read_eeprom_to_nv_data(nv_data_t *p_nv_data);
void factary_reset_nv_data(nv_data_t *p_nv_data);
void init_nv_data(void);
void update_nv_data(void);

#endif
