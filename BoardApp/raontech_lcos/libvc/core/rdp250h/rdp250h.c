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

//	DMSG("P%d: ref_temperature(%d) raw_code(%d) => temperature(%d)]\n",
//			panel_port, otp->ref_temperature, raw_code, *temperature);

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
	int i, otp_sel_idx = 0;
	U8_T apply_temperature_otp, lot[8], otp_sel[31];
	U32_T otp_sel_bits = 0x0;
	U16_T otp_base_addr = 0x0320;

	if (summary_buf == NULL) {
		EMSG("[RDP250H] Buffer is NULL!\n");
		return -1;
	}

	for (i = 0; i < 8; i++)
		lot[i] = RDP_REG_GET(port, 0x0300 + i);

	for (i = 0; i < 4; i++)
		otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0310 + i);

	otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0316);

	for (i = 0; i < 3; i++)
		otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0318 + i);

	for (i = 0; i < 4; i++)
		otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0320 + i);

	for (i = 0; i < 2; i++)
		otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0325 + i);

	for (i = 0; i < 4; i++)
		otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0328 + i);

	for (i = 0; i < 4; i++) {
		otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0330 + i);
		otp_sel[otp_sel_idx] = otp_sel[otp_sel_idx - 1] << 4;
		otp_sel_idx++;
	}

	otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x0334);

	for (i = 0; i < 4; i++)
		otp_sel[otp_sel_idx++] = RDP_REG_GET(port, 0x033C + i);

	for (i = 0; i < sizeof(otp_sel); i++) {
		if (otp_sel[i] & 0x80)
			otp_sel_bits |= (0x1 << i);
	}

	apply_temperature_otp = RDP_REG_GET(port, otp_base_addr + 0x7F);
	if (apply_temperature_otp == 0xFF)
		apply_temperature_otp = 1;
	else
		apply_temperature_otp = 0;

	sprintf(summary_buf, "%02X%02X%02X%02X%02X%02X%02X%02X.%08X.%d",
			lot[0], lot[1], lot[2], lot[3], lot[4], lot[5], lot[6], lot[7],
			otp_sel_bits, apply_temperature_otp);

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
		otp->cp_temperature *= 1000;
	} else {
		otp->apply_temperature_otp = FALSE;

		otp->ref_temperature = 0x1C2;
		otp->cp_temperature = RDP250H_DEFAULT_CP_TEMPERATURE;
	}

	//DMSG("temp: 0x%X\n", (U16_T)otp->ref_temperature);

	return 0;
}

static int rdp250h_prepare_otp(U8_T panel_port)
{
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

	return 0;
}

static int rdp250h_prepare_panel(VC_PANEL_DEV_INFO_T *dev)
{
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

