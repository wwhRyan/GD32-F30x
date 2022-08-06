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

#if defined(CFG_PANEL_RDP700Q)

#if 0
#define MAX_RDP700Q_LED_CURRENT_VALUE	0x3F
#define MIN_RDP700Q_LED_CURRENT_VALUE	0x20

static S32_T rdp700q_step_down_current_value(S32_T curr_val)
{
	S32_T new_curr;

	if (curr_val < 0x20) // for RDP550F-e. Forcely twice boot always enabled!
		curr_val += 0x20;

	new_curr = curr_val - _CFG_LED_CURRENT_CTRL_STEP;

	return new_curr;
}

static S32_T rdp700q_step_up_current_value(S32_T curr_val)
{
	S32_T new_curr;

	if (curr_val < 0x20) // for RDP550F-e. Forcely twice boot always enabled!
		curr_val += 0x20;

	new_curr = curr_val + _CFG_LED_CURRENT_CTRL_STEP;

	return new_curr;
}

static void rdp700q_get_led_current_from_reg(E_VC_PANEL_PORT_T port,
										E_VC_LED_CTRL_COLOR_T color,
										VC_LED_CURRENT_INFO_T *info)
{
	E_VC_DEVICE_CH_T dev_ch;

	switch (port) {
	case VC_PANEL_PORT_0:
		dev_ch = VC_DEVICE_CH_RDC_RDP0;
		break;

	case VC_PANEL_PORT_1:
		dev_ch = VC_DEVICE_CH_RDP1;
		break;

	default:
		EMSG("Invalid panel port\n");
		return;
	}

	if (color & VC_LED_CTRL_COLOR_R)
		info->R = RDP_REG_GET(dev_ch, 0x73) & 0x3F;
	
	if (color & VC_LED_CTRL_COLOR_G)
		info->G = RDP_REG_GET(dev_ch, 0x74) & 0x3F;
	
	if (color & VC_LED_CTRL_COLOR_B)
		info->B = RDP_REG_GET(dev_ch, 0x75) & 0x3F;	
}

static int rdp700q_set_led_current(E_VC_PANEL_CTRL_PORT_T ctrl_port,
							E_VC_LED_CTRL_COLOR_T color,
							VC_LED_CURRENT_INFO_T new_current[MAX_NUM_VC_PANEL_PORT])
{
	E_VC_PANEL_PORT_T port;

	if (ctrl_port & VC_PANEL_CTRL_PORT_0) {
		port = VC_PANEL_PORT_0;
		if (color & VC_LED_CTRL_COLOR_R) {
			if (new_current[port].R < MIN_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the down side anymore for port 0\n");
				return -1;
			}

			if (new_current[port].R > MAX_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the up side anymore for port 0\n");
				return -2;
			}
		}

		if (color & VC_LED_CTRL_COLOR_G) {
			if (new_current[port].G < MIN_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the down side anymore for port 0\n");
				return -3;
			}

			if (new_current[port].G > MAX_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the up side anymore for port 0\n");
				return -4;
			}
		}

		if (color & VC_LED_CTRL_COLOR_B) {
			if (new_current[port].B < MIN_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the down side anymore for port 0\n");
				return -5;
			}

			if (new_current[port].B > MAX_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the up side anymore for port 0\n");
				return -6;
			}
		}
	}

	if (ctrl_port & VC_PANEL_CTRL_PORT_1) {
		port = VC_PANEL_PORT_1;
		if (color & VC_LED_CTRL_COLOR_R) {
			if (new_current[port].R < MIN_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the down side anymore for port 1\n");
				return -7;
			}

			if (new_current[port].R > MAX_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("R: Can't step to the up side anymore for port 1\n");
				return -8;
			}
		}

		if (color & VC_LED_CTRL_COLOR_G) {
			if (new_current[port].G < MIN_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the down side anymore for port 1\n");
				return -9;
			}

			if (new_current[port].G > MAX_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("G: Can't step to the up side anymore for port 1\n");
				return -10;
			}
		}

		if (color & VC_LED_CTRL_COLOR_B) {
			if (new_current[port].B < MIN_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the down side anymore for port 1\n");
				return -11;
			}

			if (new_current[port].B > MAX_RDP700Q_LED_CURRENT_VALUE) {
				EMSG("B: Can't step to the up side anymore for port 1\n");
				return -12;
			}
		}
	}

#if 0
	DMSG("0: 0x73(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDC_RDP0, 0x73) & 0x3F);
	DMSG("0: 0x74(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDC_RDP0, 0x74) & 0x3F);
	DMSG("0: 0x75(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDC_RDP0, 0x75) & 0x3F);
	DMSG("1: 0x73(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDP1, 0x73) & 0x3F);
	DMSG("1: 0x74(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDP1, 0x74) & 0x3F);
	DMSG("1: 0x75(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDP1, 0x75) & 0x3F);
#endif

	if (color & VC_LED_CTRL_COLOR_R) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDC_RDP0, 0x73, 0x3F, new_current[VC_PANEL_PORT_0].R);

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDP1, 0x73, 0x3F, new_current[VC_PANEL_PORT_1].R);
	}	

	if (color & VC_LED_CTRL_COLOR_G) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDC_RDP0, 0x74, 0x3F, new_current[VC_PANEL_PORT_0].G);

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDP1, 0x74, 0x3F, new_current[VC_PANEL_PORT_1].G);
	}
	
	if (color & VC_LED_CTRL_COLOR_B) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDC_RDP0, 0x75, 0x3F, new_current[VC_PANEL_PORT_0].B);
		
		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDP1, 0x75, 0x3F, new_current[VC_PANEL_PORT_1].B);
	}

	return 0;
}

static int rdp700q_get_led_current(struct _VC_PANEL_DEV_INFO_T *dev,
							E_VC_PANEL_CTRL_PORT_T ctrl_port,
							VC_LED_CURRENT_INFO_T info[MAX_NUM_VC_PANEL_PORT])
{
	if (ctrl_port & VC_PANEL_CTRL_PORT_0)
		info[VC_PANEL_PORT_0] = dev->led_current_info[VC_PANEL_PORT_0];

	if (ctrl_port & VC_PANEL_CTRL_PORT_1)
		info[VC_PANEL_PORT_1] = dev->led_current_info[VC_PANEL_PORT_1];

	return 0;
}
#endif

static int rdp700q_prepare_panel(VC_PANEL_DEV_INFO_T *dev)
{
#if 0
#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	rdp550f_get_led_current_from_reg(VC_PANEL_PORT_0, VC_LED_CTRL_COLOR_ALL,
									&dev->led_current_info[VC_PANEL_PORT_0]);
#endif
#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	rdp550f_get_led_current_from_reg(VC_PANEL_PORT_1, VC_LED_CTRL_COLOR_ALL,
									&dev->led_current_info[VC_PANEL_PORT_1]);
#endif
#endif

	return 0;
}


int RDP700Q_PanelProbe(VC_PANEL_DEV_INFO_T *dev)
{
	dev->prepare_panel = rdp700q_prepare_panel;
	
#if 0
	dev->get_led_current = rdp700q_get_led_current;
	dev->set_led_current = rdp700q_set_led_current;

	//dev->set_brightness = rdp700q_set_brightness;
#endif

	return 0;
}

#endif // #if defined(CFG_PANEL_RDP700Q)

