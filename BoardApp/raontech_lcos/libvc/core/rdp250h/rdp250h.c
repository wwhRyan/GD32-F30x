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

#define _CFG_RDP_ATTACHED_TO_PORT_1
#define _CFG_RDP_ATTACHED_TO_PORT_0
#ifdef CFG_PANEL_RDP250H

#define RDP250H_DEFAULT_CP_TEMPERATURE		(25 * 1000/*To avoid floating*/)

#define MAX_RDP250H_LED_CURRENT_VALUE	0x3F // Used only [7:2] to elminate unmoved step.
//#define MAX_RDP250H_LED_CURRENT_VALUE	0xFF // Used only [7:2] to elminate unmoved step.
#define MIN_RDP250H_LED_CURRENT_VALUE	0x00

typedef struct {
	BOOL_T apply_temperature_otp;
	U16_T  ref_temperature;
	U16_T  cp_temperature;
} RDP250H_OTP_INFO_T;

static RDP250H_OTP_INFO_T rdp250h_otp[MAX_NUM_VC_PANEL_PORT];

/*
 * This function measures the temperature with the temperature sensor
 * on the panel and returns the calibrated code and the temperature in degrees celsius.
 *
 * code: temperature sensor code
 * temperature: temperature value in degree celsius.
 *
 * NOTE: The value of celsius temperature is calculated by multiplying by 1000
 *       to avoid floating point operations.
*/
static int rdp250h_get_temperature(int panel_port, int *tcode, S32_T *temperature)
{
	U16_T raw_code; /* Raw temperature sensor code */
	RDP250H_OTP_INFO_T *otp = &rdp250h_otp[panel_port];
		
	RDP_REG_SET(panel_port, 0x0169, 0x03); // TEMP code hold

	raw_code = (U16_T)(RDP_REG_GET(panel_port, 0x0167) & 0x3) << 8; // TEMPERATURE<9:8>
	raw_code |= (U16_T)RDP_REG_GET(panel_port, 0x0168);
	
	RDP_REG_SET(panel_port, 0x0169, 0x01); // TEMP code release(update)

	*temperature = otp->cp_temperature
					- ((otp->ref_temperature - raw_code) * (S32_T)(0.2369 * 1000));

	*tcode = raw_code;

    DMSG("%s\n", __func__);
    // DMSG("P%d: ref_temperature(%d) raw_code(%d) => temperature(%d)]\n",
    // 		panel_port, otp->ref_temperature, raw_code, *temperature);
    DMSG("P%d: cp_temperature(%d) ref_temperature(0x%X) raw_code(0x%X) temperature(%d)]\n",
        panel_port, otp->cp_temperature, otp->ref_temperature, raw_code, *temperature);

	return 0;
}

int rdp250h_get_vertical_pixel_shift(int panel_port, int *shift_val)
{
	*shift_val = RDP_REG_GET(panel_port, 0x0134);

	return 0;
}

int rdp250h_get_horizontal_pixel_shift(int panel_port, int *shift_val)
{
	*shift_val = RDP_REG_GET(panel_port, 0x0135);

	return 0;
}

int rdp250h_set_vertical_pixel_shift(int panel_port, int shift_val)
{
    if (shift_val > 255)
        DMSG("Vertical pixel cropped\n");

	RDP_REG_SET(panel_port, 0x0006, 0x08);  // PIXEL RESET
	RDP_REG_SET(panel_port, 0x0134, shift_val);
	RDP_REG_SET(panel_port, 0x0041, 0x01); // BPM_MODE	
	RDP_REG_SET(panel_port, 0x0040, 0x10); // BPM_STROBE
	VC_M_DELAY(1);
	RDP_REG_SET(panel_port, 0x0006, 0x00);  // PIXEL RESET

	return 0;
}

int rdp250h_set_horizontal_pixel_shift(int panel_port, int shift_val)
{
    if (shift_val > 175)
        DMSG("Horizotal pixel cropped\n");

	RDP_REG_SET(panel_port, 0x0006, 0x08);  // PIXEL RESET	
	RDP_REG_SET(panel_port, 0x0135, shift_val);
	RDP_REG_SET(panel_port, 0x0041, 0x01); // BPM_MODE	
	RDP_REG_SET(panel_port, 0x0040, 0x10); // BPM_STROBE
	VC_M_DELAY(1);
	RDP_REG_SET(panel_port, 0x0006, 0x00);  // PIXEL RESET

	return 0;
}

int rdp250h_get_otp_summary(int port, char *summary_buf)
{
	U8_T buf[16];
	int i, die_id, cp_applied;
	int cp_year, cp_month, cp_day, cp_rev;
	int ft_year, ft_month, ft_monthly_input_count;
	char cp_die_id_x_char, cp_die_id_y_char, cp_lot_str[9];
	char product_code_char, ft_die_id_x_char, ft_die_id_y_char;	
	const char *cp_month_str, *ft_month_str;
	static const char *month_str[12] = {
		"Jan",	"Feb",	"Mar",	"Apr",	"May",	"Jun",
		"Jul",	"Aug",	"Sep",	"Oct",	"Nov",	"Dec"
	};
	U16_T otp_base_addr = 0x0300;

	if (summary_buf == NULL) {
		EMSG("[RDP250H] Buffer is NULL!\n");
		return -1;
	}

	buf[0] = RDP_REG_GET(port, otp_base_addr + 0x77);
	switch (buf[0]) {
	case 0x33:
		product_code_char = 'X';
		break;
	case 0x27:
		product_code_char = 'R';
		break;
	default:
		product_code_char = '0'; /* No written */
		break;
	}	

	cp_applied = RDP_REG_GET(port, otp_base_addr + 0x7F);
	if (cp_applied == 0xFF) { /* CP */		
		for (i = 0; i < 16; i++)
			buf[i] = RDP_REG_GET(port, otp_base_addr + 0x00 + i);

		sprintf(cp_lot_str, "%c%c%c%c%c%c.%d",
				buf[0], buf[1], buf[2], buf[3],
				buf[4], buf[5], ((int)buf[6] << 8) | buf[7]);

		cp_month = buf[8] & 0xF;
		if (cp_month <= 12)
			cp_month_str = month_str[cp_month - 1];
		else
			cp_month_str = "000";

		cp_day = buf[9];
		cp_year = 2000 + (((buf[0xA] >> 4) & 0xF) * 10) + (buf[0xA] & 0xF);
		cp_rev = buf[0xB];
	
		die_id = ((int)buf[0xC] << 8) | buf[0xD];
		if (die_id <= 9)
			cp_die_id_x_char = '0' + die_id;
		else
			cp_die_id_x_char = (die_id - 10) + 'A';

		die_id = ((int)buf[0xE] << 8) | buf[0xF];
		if (die_id <= 9)
			cp_die_id_y_char = '0' + die_id;
		else
			cp_die_id_y_char = (die_id - 10) + 'A';
	} else {
		sprintf(cp_lot_str, "000000.0");
		cp_year = 0;
		cp_month_str = "000";
		cp_day = 0;
		cp_rev = 0;
		cp_die_id_x_char = '0';
		cp_die_id_y_char = '0';
	}

	for (i = 0; i < 5; i++)
		buf[i] = RDP_REG_GET(port, otp_base_addr + 0x78 + i);

	if (buf[0] != 0) { /* FT */
		ft_year = 2000 + (((buf[0] >> 4) & 0xF) * 10) + (buf[0] & 0xF);

		ft_month = buf[1] & 0xF;
		if (ft_month <= 12)
			ft_month_str = month_str[ft_month - 1];
		else
			ft_month_str = "000";

		ft_monthly_input_count = buf[2];

		die_id = buf[3];
		if (die_id <= 9)
			ft_die_id_x_char = '0' + die_id;
		else
			ft_die_id_x_char = (die_id - 10) + 'A';
		
		die_id = buf[4];
		if (die_id <= 9)
			ft_die_id_y_char = '0' + die_id;
		else
			ft_die_id_y_char = (die_id - 10) + 'A';
	} else {
		ft_year = 0;
		ft_month_str = "000";
		ft_monthly_input_count = 0;
		ft_die_id_x_char = '0';
		ft_die_id_y_char = '0';
	}

#if 1
	sprintf(summary_buf, "%c-%04d.%s-%03d-%c.%c",
			product_code_char, ft_year, ft_month_str, ft_monthly_input_count,
			ft_die_id_x_char, ft_die_id_y_char);

#else // Extra OTP info
	sprintf(summary_buf, "%c-%04d.%s-%03d-%c.%c %s-%s.%02d.%04d-Rev%d-%c.%c",
			product_code_char, ft_year, ft_month_str, ft_monthly_input_count,
			ft_die_id_x_char, ft_die_id_y_char,
			cp_lot_str, cp_month_str, cp_day, cp_year,
			cp_rev,	cp_die_id_x_char, cp_die_id_y_char);
#endif

	return 0;
}

int rdp250h_get_device_id(E_VC_PANEL_PORT_T port, U16_T *chip_id, U16_T *rev_num)
{
	U8_T reg0x00, reg0x01;

	reg0x00 = RDP_REG_GET(port, 0x0000);
	reg0x01 = RDP_REG_GET(port, 0x0001);

	*chip_id = ((U16_T)reg0x00<<4) | (reg0x01>>4);
	*rev_num = reg0x01 & 0x0F;

	return 0;
}

static int rdp250h_apply_tempsensor_otp(U8_T port)
{
	int ret = 0;
	U8_T TEMP_REF_lsb, TEMP_REF_msb, apply_otp_reg;
	RDP250H_OTP_INFO_T *otp = &rdp250h_otp[port];

	apply_otp_reg = RDP_REG_GET(port, 0x037F);
	if (apply_otp_reg == 0xFF) {
		otp->apply_temperature_otp = TRUE;

		TEMP_REF_msb = RDP_REG_GET(port, 0x031C) & 0x1F; // TEMPERATURE<9:5>
		TEMP_REF_lsb = RDP_REG_GET(port, 0x031D) & 0x1F; // TEMPERATURE<4:0>
		otp->ref_temperature = (U16_T)((U16_T)TEMP_REF_msb << 5)
								| (U16_T)TEMP_REF_lsb;

		otp->cp_temperature = RDP_REG_GET(port, 0x031E);
        if (otp->cp_temperature == 0) {
            otp->cp_temperature = 30;
        }
		otp->cp_temperature *= 1000;
	} else {
		otp->apply_temperature_otp = FALSE;

		otp->ref_temperature = 0x1C2;
		otp->cp_temperature = RDP250H_DEFAULT_CP_TEMPERATURE;
	}

    DMSG("temp: 0x%X\n", (U16_T)otp->ref_temperature);
    DMSG("%s\n", __func__);

	return 0;
}

static int rdp250h_prepare_otp(U8_T panel_port)
{
	DMSG("%s\n", __func__);

	rdp250h_apply_tempsensor_otp(panel_port);

	return 0;
}

static int rdp250h_prepare_temperature(VC_PANEL_DEV_INFO_T *dev, U8_T panel_port)
{
	VC_TEMPERATURE_STATUS_T *temp_status = &dev->temperature_status[panel_port];

	/* Get and store the previous data into device structure */
	rdp250h_get_temperature(panel_port,
								&temp_status->last_tsensor_code,
								&temp_status->last_temperature);

	temp_status->otp_applied = rdp250h_otp[panel_port].apply_temperature_otp;

	DMSG("%s\n", __func__);
	
	return 0;
}

static int rdp250h_prepare_panel(VC_PANEL_DEV_INFO_T *dev)
{
	DMSG("%s\n", __func__);

#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	rdp250h_prepare_otp(VC_PANEL_PORT_0);

	rdp250h_prepare_temperature(dev, VC_PANEL_PORT_0);
#endif

#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	rdp250h_prepare_otp(VC_PANEL_PORT_1);

	rdp250h_prepare_temperature(dev, VC_PANEL_PORT_1);
#endif

	return 0;
}

int RDP250H_PanelProbe(VC_PANEL_DEV_INFO_T *dev)
{
	memset(rdp250h_otp, 0x0, sizeof(rdp250h_otp));

	dev->prepare_panel = rdp250h_prepare_panel;
	dev->get_device_id = rdp250h_get_device_id;

	dev->set_horizontal_pixel_shift = rdp250h_set_horizontal_pixel_shift;
	dev->set_vertical_pixel_shift = rdp250h_set_vertical_pixel_shift;
	dev->get_horizontal_pixel_shift = rdp250h_get_horizontal_pixel_shift;
	dev->get_vertical_pixel_shift = rdp250h_get_vertical_pixel_shift;

	dev->get_temperature = rdp250h_get_temperature;

	dev->get_otp_summary = rdp250h_get_otp_summary;

	return 0;
}

#endif // #if defined(CFG_PANEL_RDP250H)

