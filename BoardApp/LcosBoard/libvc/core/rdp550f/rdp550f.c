/******************************************************************************
* (c) COPYRIGHT 2017 RAONTECH, Inc. ALL RIGHTS RESERVED.                      *
*                                                                             *
* This software is the property of RAONTECH and is furnished under license    *
* by RAONTECH.                                                                *
* This software may be used only in accordance with the terms of said license.*
* This copyright noitce may not be removed, modified or obliterated without   *
* the prior written permission of RAONTECH, Inc.                              *
*                                                                             *
* This software may not be copied, transmitted, provided to or otherwise      *
* made available to any other person, company, corporation or other entity    *
* except as specified in the terms of said license.                           *
*                                                                             *
* No right, title, ownership or other interest in the software is hereby      *
* granted or transferred.                                                     *
*                                                                             *
* The information contained herein is subject to change without notice        *
* and should not be construed as a commitment by RAONTECH, Inc.               *
******************************************************************************/

#include <math.h>
#include "./../rti_vc_panel.h"

#if defined(CFG_PANEL_RDP550F) // 2018-05-15 RDP550F-F

/* LED current.
 *
 * RDP550F-e:
 *	MSB[5]: twice boot
 *	Range: (0x00 ~ 0x1F) or (0x20 ~ 0x3F)
 *
 * RDP550F-f:
 *	Range: 0x00 ~ 0xFF
 */

#define MAX_RDP550F_LED_CURRENT_VALUE	0x3F // Used only [7:2] to elminate unmoved step.
#define MIN_RDP550F_LED_CURRENT_VALUE	0x00

typedef enum {
	RDP550F_OTP_INDICATOR_GAMMA = 0x1,          // Gamma
	RDP550F_OTP_INDICATOR_COLDRV_REPAIR = 0x2,  // Column driver repair
	RDP550F_OTP_INDICATOR_LEDDRV_CURRENT = 0x4, // LED driver current
	RDP550F_OTP_INDICATOR_OSC = 0x8,            // Oscillator
	RDP550F_OTP_INDICATOR_LVDS_RX_TERM = 0x10   // LVDS Rx terminal
} E_RDP550F_OTP_INDICATOR_T;

typedef struct {
	BOOL_T led_apply;
	U8_T LED_CON; // POL0. Only R
	U8_T LED_RES_SEL;
} RDP550F_OTP_INFO_T;

static RDP550F_OTP_INFO_T rdp550f_otp[MAX_NUM_VC_PANEL_PORT];

/*
 Calculate 8-bit RDP LED_CON regisgter value.
 
 level: 8-bit UI level
*/
static U8_T rdp550f_calculate_led_otp(U8_T otp_LED_CON, U8_T level)
{
	U32_T result;
	U8_T ret;
	U8_T b;
	
	// Multiply 10 to eliminate floating math.
	result = (U32_T)((2550 - (U32_T)(otp_LED_CON*10)) * (U32_T)(level*10)) / 2550;

	ret = result / 10;
	b = result % 10;
	if (b >= 5)
		ret++;

	return ret;
}

/*
 Calculate 6-bit UI level.

 panel_LED_CON: 8-bit LED_CON in panel register
*/
static U8_T rdp550f_calculate_ui_level(U8_T otp_LED_CON, U8_T panel_LED_CON)
{
	U8_T level; // UI level (6-bit)
	U32_T temp;
	U8_T b;
	
	temp = ((U32_T)panel_LED_CON * (U32_T)2550 * 10) / (2550 - (U32_T)(otp_LED_CON*10));	
	temp >>= 2; // Divide by 4. (for 6-bit UI level)

	level = temp / 10;
	b = temp % 10;
	if (b >= 5)
		level++;

	return level;
}

int rdp550f_get_led_level_range(U8_T *min, U8_T *max)
{
	*min = MIN_RDP550F_LED_CURRENT_VALUE;
	*max = MAX_RDP550F_LED_CURRENT_VALUE;

	return 0;
}

static int rdp550f_set_led_current(E_VC_PANEL_CTRL_PORT_T ctrl_port,
							E_VC_LED_CTRL_COLOR_T color,
							VC_LED_CURRENT_INFO_T new_current[MAX_NUM_VC_PANEL_PORT])
{
	int i;
	U8_T val[MAX_NUM_VC_PANEL_PORT][3]; // Register value to be written.
	U8_T chk_port;
	VC_LED_CURRENT_INFO_T *current_ptr = NULL;
	
	// Calculate first!
	chk_port = (U8_T)ctrl_port;
	for (i = VC_PANEL_PORT_0; i < MAX_NUM_VC_PANEL_PORT; i++) {
		if (chk_port & 0x1) {
			current_ptr = &new_current[i];

			if (color & VC_LED_CTRL_COLOR_R) {
				if (current_ptr->R < MIN_RDP550F_LED_CURRENT_VALUE) {
					EMSG("R: Can't step to the down side anymore for port %d\n", i);
					//return -1;
					current_ptr->R = MIN_RDP550F_LED_CURRENT_VALUE;
				}

				if (current_ptr->R > MAX_RDP550F_LED_CURRENT_VALUE) {
					EMSG("R: Can't step to the up side anymore for port %d\n", i);
					current_ptr->R = MAX_RDP550F_LED_CURRENT_VALUE;
				}

				/* Make 8-bit level to write it into register. */
				val[i][0] = current_ptr->R << 2;

				if (rdp550f_otp[i].led_apply == TRUE)
					val[i][0] = rdp550f_calculate_led_otp(rdp550f_otp[i].LED_CON,
															val[i][0]);
			}

			if (color & VC_LED_CTRL_COLOR_G) {
				if (current_ptr->G < MIN_RDP550F_LED_CURRENT_VALUE) {
					EMSG("G: Can't step to the down side anymore for port %d\n", i);
					current_ptr->G = MIN_RDP550F_LED_CURRENT_VALUE;
				}

				if (current_ptr->G > MAX_RDP550F_LED_CURRENT_VALUE) {
					EMSG("G: Can't step to the up side anymore for port %d\n", i);
					current_ptr->G = MAX_RDP550F_LED_CURRENT_VALUE;
				}

				val[i][1] = current_ptr->G << 2;

				if (rdp550f_otp[i].led_apply == TRUE)
					val[i][1] = rdp550f_calculate_led_otp(rdp550f_otp[i].LED_CON,
															val[i][1]);
			}

			if (color & VC_LED_CTRL_COLOR_B) {
				if (current_ptr->B < MIN_RDP550F_LED_CURRENT_VALUE) {
					EMSG("B: Can't step to the down side anymore for port %d\n", i);
					current_ptr->B = MIN_RDP550F_LED_CURRENT_VALUE;
				}

				if (current_ptr->B > MAX_RDP550F_LED_CURRENT_VALUE) {
					EMSG("B: Can't step to the up side anymore for port %d\n", i);
					current_ptr->B = MAX_RDP550F_LED_CURRENT_VALUE;
				}

				val[i][2] = current_ptr->B << 2;

				if (rdp550f_otp[i].led_apply == TRUE)
					val[i][2] = rdp550f_calculate_led_otp(rdp550f_otp[i].LED_CON,
															val[i][2]);
			}
		}		
		chk_port >>= 1;
	}

	// Write register values.	
	if (color & VC_LED_CTRL_COLOR_R) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_SET(VC_DEVICE_CH_RDC_RDP0, 0xCF, val[VC_PANEL_PORT_0][0]);
		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_SET(VC_DEVICE_CH_RDP1, 0xCF, val[VC_PANEL_PORT_1][0]);
	}

	if (color & VC_LED_CTRL_COLOR_G) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_SET(VC_DEVICE_CH_RDC_RDP0, 0xD0, val[VC_PANEL_PORT_0][1]);

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_SET(VC_DEVICE_CH_RDP1, 0xD0, val[VC_PANEL_PORT_1][1]);
	}
	
	if (color & VC_LED_CTRL_COLOR_B) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_SET(VC_DEVICE_CH_RDC_RDP0, 0xD1, val[VC_PANEL_PORT_0][2]);
		
		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_SET(VC_DEVICE_CH_RDP1, 0xD1, val[VC_PANEL_PORT_1][2]);
	}
	
#if 0
	if (ctrl_port & VC_PANEL_CTRL_PORT_0) {
		DMSG("P0: 0xCF(0x%02X)\n", RDP_REG_GET(0, 0xCF));
		DMSG("P0: 0xD0(0x%02X)\n", RDP_REG_GET(0, 0xD0));
		DMSG("P0: 0xD1(0x%02X)\n", RDP_REG_GET(0, 0xD1));
	}

	if (ctrl_port & VC_PANEL_CTRL_PORT_1) {
		DMSG("P1: 0xCF(0x%02X)\n", RDP_REG_GET(1, 0xCF));
		DMSG("P1: 0xD0(0x%02X)\n", RDP_REG_GET(1, 0xD0));
		DMSG("P1: 0xD1(0x%02X)\n", RDP_REG_GET(1, 0xD1));
	}
#endif

	return 0;
}

static int rdp550f_get_led_current(struct _VC_PANEL_DEV_INFO_T *dev,
							E_VC_PANEL_CTRL_PORT_T ctrl_port,
							VC_LED_CURRENT_INFO_T info[MAX_NUM_VC_PANEL_PORT])
{
	if (ctrl_port & VC_PANEL_CTRL_PORT_0)
		info[VC_PANEL_PORT_0] = dev->led_current_info[VC_PANEL_PORT_0];

	if (ctrl_port & VC_PANEL_CTRL_PORT_1)
		info[VC_PANEL_PORT_1] = dev->led_current_info[VC_PANEL_PORT_1];

	return 0;
}

static void rdp550f_read_led_current_from_reg(E_VC_PANEL_PORT_T port,										
										VC_LED_CURRENT_INFO_T *info)
{
	U8_T R, G, B;
	
	R = RDP_REG_GET(port, 0xCF);
	G = RDP_REG_GET(port, 0xD0);
	B = RDP_REG_GET(port, 0xD1);

#if 0
	DMSG("P%d: 0xCF(0x%02X)\n", port, R);
	DMSG("P%d: 0xD0(0x%02X)\n", port, G);
	DMSG("P%d: 0xD1(0x%02X)\n", port, B);
#endif	

	if (rdp550f_otp[port].led_apply == TRUE) {
		info->R = rdp550f_calculate_ui_level(rdp550f_otp[port].LED_CON, R);
		info->G = rdp550f_calculate_ui_level(rdp550f_otp[port].LED_CON, G);
		info->B = rdp550f_calculate_ui_level(rdp550f_otp[port].LED_CON, B);
	}
	else {
		info->R = R >> 2; // Used only [7:2]. 4 step
		info->G = G >> 2; // Used only [7:2] 
		info->B = B >> 2; // Used only [7:2]
	}
}

static int rdp550f_read_otp(U8_T panel_port, U8_T *rdata, U8_T otp_addr)
{
	RDP_REG_SET(panel_port, 0x7B, 0x00);
	RDP_REG_SET(panel_port, 0x7C, 0x80);
	RDP_REG_SET(panel_port, 0x7C, 0x00);
	RDP_REG_SET(panel_port, 0x7C, otp_addr);
	RDP_REG_SET(panel_port, 0x7E, 0x01);
	VC_U_DELAY(100);
	*rdata = RDP_REG_GET(panel_port, 0x07);
	RDP_REG_SET(panel_port, 0x7C, 0x80);

	return 0;
}

static int rdp550f_prepare_led_otp(U8_T panel_port)
{
	int ret;
	U8_T otp_indicator, otp_data;
	RDP550F_OTP_INFO_T *otp = &rdp550f_otp[panel_port];

	ret = rdp550f_read_otp(panel_port, &otp_indicator, 0x00);
	if (ret)
		return ret;

	if (((otp_indicator & 0xC0) == 0x00) // Normal operation
		&& (otp_indicator & RDP550F_OTP_INDICATOR_LEDDRV_CURRENT)) {
		otp->led_apply = TRUE;

		rdp550f_read_otp(panel_port, &otp_data, 0x16);
		otp->LED_CON = otp_data;

		rdp550f_read_otp(panel_port, &otp_data, 0x1C);
		otp->LED_RES_SEL = otp_data & 0x7;
		if (otp->LED_RES_SEL == 0)
			otp->LED_RES_SEL = 2;
	}
		
	return 0;
}

static int rdp550f_prepare_panel(VC_PANEL_DEV_INFO_T *dev)
{
#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	#if (_CFG_RDP_SLAVE_ADDR == 0x18) // RDP550F-F
	rdp550f_prepare_led_otp(VC_PANEL_PORT_0);
	
	rdp550f_read_led_current_from_reg(VC_PANEL_PORT_0,
									&dev->led_current_info[VC_PANEL_PORT_0]);
	#endif
#endif

#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	#if (_CFG_RDP_SLAVE_ADDR == 0x18) // RDP550F-F
	rdp550f_prepare_led_otp(VC_PANEL_PORT_1);
	
	rdp550f_read_led_current_from_reg(VC_PANEL_PORT_1,
									&dev->led_current_info[VC_PANEL_PORT_1]);
	#endif
#endif


#if (_CFG_RDP_SLAVE_ADDR == 0x18)
	rdp550f_f_attached = 1;
#else
	rdp550f_f_attached = 0;
#endif

	return 0;
}

int rdp550f_set_vertical_pixel_shift(int panel_port, int center)
{
	U8_T swpd;
	U8_T shift_h, tmode_n_start_col;
	U16_T shift_h_tot;

	swpd = RDP_REG_GET(panel_port, 0x12);
	RDP_REG_SET(panel_port, 0x12, swpd|0x22); // SW_PD[7:0]

	RDP_REG_MASK_SET(panel_port, 0xE8, 0x40, 0x40); // OSC0_DIV_EN
	
	RDP_REG_SET(panel_port, 0x14, (U8_T)(0x2300>>8)); // SW_RESET[13:8]
	RDP_REG_SET(panel_port, 0x15, 0x2300&0xFF); // SW_RESET[7:0]

	RDP_REG_SET(panel_port, 0x14, (U8_T)(0x0000>>8)); // SW_RESET[13:8]
	RDP_REG_SET(panel_port, 0x15, 0x0000&0xFF); // SW_RESET[7:0]

	RDP_REG_MASK_SET(panel_port, 0x8C, 0x08, 0x00); // TMODE_EN
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 0); // TMODE_MODE

	shift_h = RDP_REG_GET(panel_port, 0x1D) & 0x1F; // SHIFT_H<4:0>
	tmode_n_start_col = RDP_REG_GET(panel_port, 0x8A); // TMODE_N_START_COL
	shift_h_tot = (tmode_n_start_col * 2 ) + (shift_h % 2);
	RDP_REG_MASK_SET(panel_port, 0x1D, 0x1F, (shift_h_tot% 8)); // SHIFT_H<4:0>

	RDP_REG_MASK_SET(panel_port, 0x8C, 0x08, 0x08); // TMODE_EN

	RDP_REG_SET(panel_port, 0x8A, (shift_h_tot / 2) + (shift_h_tot % 2)); // TMODE_N_START_COL
	RDP_REG_SET(panel_port, 0x8B, center); // TMODE_N_START_ROW
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 1); // TMODE_MODE
	VC_M_DELAY(10);

	RDP_REG_SET(panel_port, 0x8A, (shift_h_tot / 2)); // TMODE_N_START_COL
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 0x02); // TMODE_MODE
	VC_M_DELAY(10);

	RDP_REG_MASK_SET(panel_port, 0x8C, 0x08, 0); // TMODE_EN
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 0); // TMODE_MODE

	RDP_REG_SET(panel_port, 0x12, swpd); // SW_PD[7:0]

	return 0;
}

int rdp550f_set_horizontal_pixel_shift(int panel_port, int center)
{
	U8_T swpd;
	U8_T TMODE_N_START_COL;

	swpd = RDP_REG_GET(panel_port, 0x12);
	RDP_REG_SET(panel_port, 0x12, swpd|0x22); // SW_PD[7:0]

	RDP_REG_MASK_SET(panel_port, 0xE8, 0x40, 0x40); // OSC0_DIV_EN
	
	RDP_REG_SET(panel_port, 0x14, (U8_T)(0x2300>>8)); // SW_RESET[13:8]
	RDP_REG_SET(panel_port, 0x15, 0x2300&0xFF); // SW_RESET[7:0]

	RDP_REG_SET(panel_port, 0x14, (U8_T)(0x0000>>8)); // SW_RESET[13:8]
	RDP_REG_SET(panel_port, 0x15, 0x0000&0xFF); // SW_RESET[7:0]

	RDP_REG_MASK_SET(panel_port, 0x8C, 0x08, 0x00); // TMODE_EN
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 0); // TMODE_MODE

	RDP_REG_MASK_SET(panel_port, 0x1D, 0x1F, center%8); // SHIFT_H<4:0>

	RDP_REG_MASK_SET(panel_port, 0x8C, 0x08, 0x08); // TMODE_EN

	TMODE_N_START_COL = (center / 2) + (center % 2);
	RDP_REG_SET(panel_port, 0x8A, TMODE_N_START_COL); // TMODE_N_START_COL
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 1); // TMODE_MODE
	VC_M_DELAY(10);

	TMODE_N_START_COL = center / 2;
	RDP_REG_SET(panel_port, 0x8A, TMODE_N_START_COL); // TMODE_N_START_COL
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 2); // TMODE_MODE
	VC_M_DELAY(10);

	RDP_REG_MASK_SET(panel_port, 0x8C, 0x08, 0); // TMODE_EN
	RDP_REG_MASK_SET(panel_port, 0x8C, 0x07, 0); // TMODE_MODE

	RDP_REG_SET(panel_port, 0x12, swpd); // SW_PD[7:0]

	return 0;
}

int rdp550f_get_device_id(E_VC_PANEL_PORT_T port, U16_T *chip_id, U16_T *rev_num)
{
	U8_T reg0x00, reg0x01;

	if (rdp550f_f_attached == 1) {
		reg0x00 = RDP_REG_GET(port, 0x00);
		reg0x01 = RDP_REG_GET(port, 0x01);

		*chip_id = ((U16_T)reg0x00<<4) | (reg0x01>>4);
		*rev_num = reg0x01 & 0x0F;
	}
	else {
		reg0x00 = RDP_REG_GET(port, 0x00);
		*chip_id = reg0x00;
		*rev_num = 0;
	}

	return 0;
}

int RDP550F_PanelProbe(VC_PANEL_DEV_INFO_T *dev)
{
	unsigned int port;

	for (port = 0; port < MAX_NUM_VC_PANEL_PORT; port++)
		rdp550f_otp[port].led_apply = FALSE;
	
	dev->prepare_panel = rdp550f_prepare_panel;
	dev->get_device_id = rdp550f_get_device_id;

	dev->get_led_current = rdp550f_get_led_current;
	dev->set_led_current = rdp550f_set_led_current;
	dev->get_led_level_range = rdp550f_get_led_level_range;

	dev->set_horizontal_pixel_shift = rdp550f_set_horizontal_pixel_shift;
	dev->set_vertical_pixel_shift = rdp550f_set_vertical_pixel_shift;

	return 0;
}

#endif // #if defined(CFG_PANEL_RDP550F)

