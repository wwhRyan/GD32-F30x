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

#include "./../rti_vc_panel.h"

#if defined(CFG_PANEL_RDP501H)

#define MAX_RDP501H_LED_CURRENT_VALUE    0x1A
#define MIN_RDP501H_LED_CURRENT_VALUE    0x06

#define RDP501H_LED_CTRL_MIN	0x06
#define RDP501H_LED_CTRL_MAX	0x1A


//
#define RDP501H_LED_CTRL_STEP    4

#define RDP501H_CHECK_MIN_VAL\
	(RDP501H_LED_CTRL_MIN + RDP501H_LED_CTRL_STEP)

static int rdp501h_decrease_brightness(void)
{
	U8_T value;
	int ret = 0;

#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	/* Read RDP501H_0 Red LED Current */		
	value = RDC_REG_GET(0x24);
	/* Check MIN and then Apply */
	if( (value & 0x1F) >= (RDP501H_LED_CTRL_MIN + RDP501H_LED_CTRL_STEP) ) 
		RDC_REG_SET(0x24, value - RDP501H_LED_CTRL_STEP);
	else 
		ret = -1;

	/* Read RDP501H_0 Green LED Current */		
	value = RDC_REG_GET(0x25);
	/* Check MIN and then Apply */
	if( (value & 0x1F) >= (RDP501H_LED_CTRL_MIN + RDP501H_LED_CTRL_STEP) ) 
		RDC_REG_SET(0x25, value - RDP501H_LED_CTRL_STEP);
	else 
		ret = -2;

	/* Read RDP501H_0 Blue LED Current */		
	value = RDC_REG_GET(0x26);
	/* Check MIN and then Apply */
	if( (value & 0x1F) >= (RDP501H_LED_CTRL_MIN + RDP501H_LED_CTRL_STEP) ) 
		RDC_REG_SET(0x26, value - RDP501H_LED_CTRL_STEP);
	else 
		ret = -3;
#endif

#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	/* Read RDP501H_1 Red LED Current */
	value = RDC_REG_GET(0x85);
	/* Check MIN and then Apply */
	if( (value & 0x1F) >= (RDP501H_LED_CTRL_MIN + RDP501H_LED_CTRL_STEP) ) 
		RDC_REG_SET(0x85, value - RDP501H_LED_CTRL_STEP);
	else 
		ret = -4;

	/* Read RDP501H_1 Green LED Current */		
	value = RDC_REG_GET(0x86);
	/* Check MIN and then Apply */
	if( (value & 0x1F) >= (RDP501H_LED_CTRL_MIN + RDP501H_LED_CTRL_STEP) ) 
		RDC_REG_SET(0x86, value - RDP501H_LED_CTRL_STEP);
	else 
		ret = -5;

	/* Read RDP501H_1 Blue LED Current */		
	value = RDC_REG_GET(0x87);
	/* Check MIN and then Apply */
	if( (value & 0x1F) >= (RDP501H_LED_CTRL_MIN + RDP501H_LED_CTRL_STEP) ) 
		RDC_REG_SET(0x87, value - RDP501H_LED_CTRL_STEP);
	else 
		ret = -6;	
#endif

	return ret;
}

static int rdp501h_increase_brightness(void)
{
	U8_T value;
	int ret = 0;

#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	/* Read RDP501H_0 Red LED Current */
	value = RDC_REG_GET(0x24); 
	/* Check MAX and then Apply */
	if( (value & 0x1F) <= (RDP501H_LED_CTRL_MAX - RDP501H_LED_CTRL_STEP)) 
		RDC_REG_SET(0x24, value + RDP501H_LED_CTRL_STEP);
	else 
		ret = -1;

	/* Read RDP501H_0 Green LED Current */
	value = RDC_REG_GET(0x25); 
	/* Check MAX and then Apply */
	if( (value & 0x1F) <= (RDP501H_LED_CTRL_MAX - RDP501H_LED_CTRL_STEP)) 
		RDC_REG_SET(0x25, value + RDP501H_LED_CTRL_STEP);
	else 
		ret = -2;

	/* Read RDP501H_0 Blue LED Current */
	value = RDC_REG_GET(0x26); 
	/* Check MAX and then Apply */
	if( (value & 0x1F) <= (RDP501H_LED_CTRL_MAX - RDP501H_LED_CTRL_STEP)) 
		RDC_REG_SET(0x26, value + RDP501H_LED_CTRL_STEP);
	else 
		ret = -3;
#endif

#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	/* Read RDP501H_1 Red LED Current */
	value = RDC_REG_GET(0x85); 
	/* Check MAX and then Apply */
	if( (value & 0x1F) <= (RDP501H_LED_CTRL_MAX - RDP501H_LED_CTRL_STEP)) 
		RDC_REG_SET(0x85, value + RDP501H_LED_CTRL_STEP);
	else 
		ret = -4;

	/* Read RDP501H_1 Green LED Current */
	value = RDC_REG_GET(0x86); 
	/* Check MAX and then Apply */
	if( (value & 0x1F) <= (RDP501H_LED_CTRL_MAX - RDP501H_LED_CTRL_STEP)) 
		RDC_REG_SET(0x86, value + RDP501H_LED_CTRL_STEP);
	else 
		ret = -5;	

	/* Read RDP501H_1 Blue LED Current */
	value = RDC_REG_GET(0x87); 
	/* Check MAX and then Apply */
	if( (value & 0x1F) <= (RDP501H_LED_CTRL_MAX - RDP501H_LED_CTRL_STEP)) 
		RDC_REG_SET(0x87, value + RDP501H_LED_CTRL_STEP);
	else 
		ret = -6;	
#endif

	return ret;
}

#define RDP501H_ADDR6TO5(x)     ((((x)&(0x40)) + ((x)&(0x20))) >> 5)
#define RDP501H_ADDR4TO1(x)     ((((x)&(0x10)) + ((x)&(0x08)) + ((x)&(0x04)) + ((x)&(0x02))) >> 1)
#define RDP501H_ADDR0(x)        ((x)&(0x01))
#define RDP501H_DATA5TO3(x)     ((((x)&(0x20)) + ((x)&(0x10)) + ((x)&(0x08))) >> 3)
#define RDP501H_DATA2TO0(x)     (((x)&(0x04)) + ((x)&(0x02)) + ((x)&(0x01)))

void rtiVC_WriteRDP501H_1(U8_T reg, U8_T val)
{
#if defined(CFG_MDC_RDC100)
    U8_T dwAddrHigh;
    U8_T dwAddrMiddle;
    U8_T dwAdddrLow;
    U8_T dwDataHigh;
    U8_T dwDataLow;

    dwAddrHigh = RDP501H_ADDR6TO5(reg);
    dwAddrMiddle = RDP501H_ADDR4TO1(reg);
    dwAdddrLow = RDP501H_ADDR0(reg);
    dwDataHigh = RDP501H_DATA5TO3(val);
    dwDataLow = RDP501H_DATA2TO0(val);

    RDC_REG_SET(0x89, dwAddrHigh);
    RDC_REG_SET(0x8C, dwAddrMiddle);
    RDC_REG_SET(0x2E, dwAdddrLow);
    RDC_REG_SET(0x8A, dwDataHigh);
    RDC_REG_SET(0x88, dwDataLow);
    RDC_REG_SET(0x8B, 0x3);
    VC_M_DELAY(10);
    RDC_REG_SET(0x8B, 0x0);
#else
	//#error "Code not present"
	U8_T buf[3];

	buf[0] = 0x80;
	buf[1] = reg;
	buf[2] = val;	
	rtiVC_WriteBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0,
						vc_rdc_i2c_slave_addr, buf, 3);
#endif
}

void rtiVC_WriteRDP501H_0(U8_T reg, U8_T val)
{
#if defined(CFG_MDC_RDC100)
	U8_T dwAddrHigh;
    U8_T dwAddrMiddle;
    U8_T dwAdddrLow;
    U8_T dwDataHigh;
    U8_T dwDataLow;    
        
    dwAddrHigh = RDP501H_ADDR6TO5(reg);
    dwAddrMiddle = RDP501H_ADDR4TO1(reg);
    dwAdddrLow = RDP501H_ADDR0(reg);
    dwDataHigh = RDP501H_DATA5TO3(val);
    dwDataLow = RDP501H_DATA2TO0(val);

    RDC_REG_SET(0x28, dwAddrHigh);
    RDC_REG_SET(0x2B, dwAddrMiddle);
    RDC_REG_SET(0x2E, dwAdddrLow);
    RDC_REG_SET(0x29, dwDataHigh);
    RDC_REG_SET(0x27, dwDataLow);
    RDC_REG_SET(0x2A, 0x3);
    VC_M_DELAY(10);
    RDC_REG_SET(0x2A, 0x0);
#else
	U8_T buf[3];

	buf[0] = 0x80;
	buf[1] = reg;
	buf[2] = val;	
	rtiVC_WriteBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0,
						vc_rdc_i2c_slave_addr, buf, 3);
#endif
}

void rdp501h_set_brightness(int brightness)
{
}

int rdp501h_get_led_level_range(U8_T *min, U8_T *max)
{
	*min = MIN_RDP501H_LED_CURRENT_VALUE;
	*max = MAX_RDP501H_LED_CURRENT_VALUE;

	return 0;
}

static int rdp501h_set_led_current(E_VC_PANEL_CTRL_PORT_T ctrl_port,
							E_VC_LED_CTRL_COLOR_T color,
							VC_LED_CURRENT_INFO_T new_current[MAX_NUM_VC_PANEL_PORT])
{
	E_VC_PANEL_PORT_T port;
	U8_T reg_val;

	if (ctrl_port & VC_PANEL_CTRL_PORT_0) {
		port = VC_PANEL_PORT_0;
		if (color & VC_LED_CTRL_COLOR_R) {
			if (new_current[port].R < MIN_RDP501H_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the down side anymore for port 0\n");
				//return -1;
				new_current[port].R = MIN_RDP501H_LED_CURRENT_VALUE;
			}

			if (new_current[port].R > MAX_RDP501H_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the up side anymore for port 0\n");
				new_current[port].R = MAX_RDP501H_LED_CURRENT_VALUE;
			}
		}

		if (color & VC_LED_CTRL_COLOR_G) {
			if (new_current[port].G < MIN_RDP501H_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the down side anymore for port 0\n");
				new_current[port].G = MIN_RDP501H_LED_CURRENT_VALUE;
			}

			if (new_current[port].G > MAX_RDP501H_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the up side anymore for port 0\n");
				new_current[port].G = MAX_RDP501H_LED_CURRENT_VALUE;
			}
		}

		if (color & VC_LED_CTRL_COLOR_B) {
			if (new_current[port].B < MIN_RDP501H_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the down side anymore for port 0\n");
				new_current[port].B = MIN_RDP501H_LED_CURRENT_VALUE;
			}

			if (new_current[port].B > MAX_RDP501H_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the up side anymore for port 0\n");
				new_current[port].B = MAX_RDP501H_LED_CURRENT_VALUE;
			}
		}
	}

	if (ctrl_port & VC_PANEL_CTRL_PORT_1) {
		port = VC_PANEL_PORT_1;
		if (color & VC_LED_CTRL_COLOR_R) {
			if (new_current[port].R < MIN_RDP501H_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the down side anymore for port 1\n");
				new_current[port].R = MIN_RDP501H_LED_CURRENT_VALUE;
			}

			if (new_current[port].R > MAX_RDP501H_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the up side anymore for port 1\n");
				new_current[port].R = MAX_RDP501H_LED_CURRENT_VALUE;
			}
		}

		if (color & VC_LED_CTRL_COLOR_G) {
			if (new_current[port].G < MIN_RDP501H_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the down side anymore for port 1\n");
				new_current[port].G = MIN_RDP501H_LED_CURRENT_VALUE;
			}

			if (new_current[port].G > MAX_RDP501H_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the up side anymore for port 1\n");
				new_current[port].G = MAX_RDP501H_LED_CURRENT_VALUE;
			}
		}

		if (color & VC_LED_CTRL_COLOR_B) {
			if (new_current[port].B < MIN_RDP501H_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the down side anymore for port 1\n");
				new_current[port].B = MIN_RDP501H_LED_CURRENT_VALUE;
			}

			if (new_current[port].B > MAX_RDP501H_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the up side anymore for port 1\n");
				new_current[port].B = MAX_RDP501H_LED_CURRENT_VALUE;
			}
		}
	}

#if 0
	DMSG("0xA4(0x%02X)\n", ((RDC_REG_GET(0xA4) >> 3) & 0x1F));
#endif

	if (color & VC_LED_CTRL_COLOR_R) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			reg_val = (U8_T)new_current[VC_PANEL_PORT_0].R;

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			reg_val = (U8_T)new_current[VC_PANEL_PORT_1].R;
	}

	if (color & VC_LED_CTRL_COLOR_G) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			reg_val = (U8_T)new_current[VC_PANEL_PORT_0].G;

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			reg_val = (U8_T)new_current[VC_PANEL_PORT_1].G;
	}
	
	if (color & VC_LED_CTRL_COLOR_B) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			reg_val = (U8_T)new_current[VC_PANEL_PORT_0].B;

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			reg_val = (U8_T)new_current[VC_PANEL_PORT_1].B;
	}

	RDC_REG_MASK_SET(0xA4, 0xF8, reg_val<<3);

	return 0;
}

static void rdp501h_get_led_current_from_reg(E_VC_PANEL_PORT_T port,
										E_VC_LED_CTRL_COLOR_T color,
										VC_LED_CURRENT_INFO_T *info)
{
	info->R = info->G = info->B = RDC_REG_GET(0xA4) >> 3;
}

static int rdp501h_get_led_current(struct _VC_PANEL_DEV_INFO_T *dev,
							E_VC_PANEL_CTRL_PORT_T ctrl_port,
							VC_LED_CURRENT_INFO_T info[MAX_NUM_VC_PANEL_PORT])
{
	if (ctrl_port & VC_PANEL_CTRL_PORT_0)
		info[VC_PANEL_PORT_0] = dev->led_current_info[VC_PANEL_PORT_0];

	if (ctrl_port & VC_PANEL_CTRL_PORT_1)
		info[VC_PANEL_PORT_1] = dev->led_current_info[VC_PANEL_PORT_1];

	return 0;
}

static int rdp501h_prepare_panel(VC_PANEL_DEV_INFO_T *dev)
{
//#ifdef _CFG_RDP_ATTACHED_TO_PORT_0 // from firmware configuration?
	rdp501h_get_led_current_from_reg(VC_PANEL_PORT_0, VC_LED_CTRL_COLOR_ALL,
									&dev->led_current_info[VC_PANEL_PORT_0]);
//#endif
//#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	rdp501h_get_led_current_from_reg(VC_PANEL_PORT_1, VC_LED_CTRL_COLOR_ALL,
									&dev->led_current_info[VC_PANEL_PORT_1]);
//#endif

	return 0;
}

int RDP501H_PanelProbe(VC_PANEL_DEV_INFO_T *dev)
{
	dev->prepare_panel = rdp501h_prepare_panel;

	dev->get_led_current = rdp501h_get_led_current;
	dev->set_led_current = rdp501h_set_led_current;
	dev->get_led_level_range = rdp501h_get_led_level_range;

	dev->set_brightness = rdp501h_set_brightness;

	dev->increase_brightness = rdp501h_increase_brightness;
	dev->decrease_brightness = rdp501h_decrease_brightness;	

	return 0;
}
#endif

