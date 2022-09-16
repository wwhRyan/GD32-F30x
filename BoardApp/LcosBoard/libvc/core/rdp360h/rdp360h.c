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

#if defined(CFG_PANEL_RDP360H)

#define MAX_RDP360H_LED_CURRENT_VALUE	0x3F
#define MIN_RDP360H_LED_CURRENT_VALUE	0x00

//
#define LED_CTRL_STEP 8
#define LED_CTRL_MIN 0x0F
#define RDP360H_LED_CTRL_MAX 0x37


static int rdp360h_decrease_brightness(void)
{
	U8_T value;
	int ret = 0;

#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	/* Read RDP360H_0 Red LED Current */		
	value = RDP_REG_GET(0, 0xA6);
	/* Check MIN and then Apply */
	if ((value & 0x3F) >= (LED_CTRL_MIN + LED_CTRL_STEP)) 
		RDP_REG_SET(0, 0xA6, value - LED_CTRL_STEP);
	else 
		ret = -1; // BLU underflow

	/* Read RDP360H_0 Green LED Current */
	value = RDP_REG_GET(0, 0xA7);
	/* Check MIN and then Apply */
	if (value >= (LED_CTRL_MIN + LED_CTRL_STEP)) 
		RDP_REG_SET(0, 0xA7, value - LED_CTRL_STEP);
	else 
		ret = -2; // BLU underflow

	/* Read RDP360H_0 Blue LED Current */
	value = RDP_REG_GET(0, 0xA8);
	/* Check MIN and then Apply */
	if (value >= (LED_CTRL_MIN + LED_CTRL_STEP)) 
		RDP_REG_SET(0, 0xA8, value - LED_CTRL_STEP);
	else 
		ret = -3; // BLU underflow
#endif

#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	/* Read RDP360H_1 Red LED Current */		
	value = RDP_REG_GET(1, 0xA6);
	/* Check MIN and then Apply */
	if ((value & 0x3F) >= (LED_CTRL_MIN + LED_CTRL_STEP)) 
		RDP_REG_SET(1, 0xA6, value - LED_CTRL_STEP);
	else 
		ret = -4; // BLU underflow

	/* Read RDP360H_1 Green LED Current */
	value = RDP_REG_GET(1, 0xA7);
	/* Check MIN and then Apply */
	if ((value) >= (LED_CTRL_MIN + LED_CTRL_STEP)) 
		RDP_REG_SET(1, 0xA7, value - LED_CTRL_STEP);
	else 
		ret = -5; // BLU underflow

	/* Read RDP360H_1 Blue LED Current */
	value = RDP_REG_GET(1, 0xA8);
	/* Check MIN and then Apply */
	if ((value) >= (LED_CTRL_MIN + LED_CTRL_STEP)) 
		RDP_REG_SET(1, 0xA8, value - LED_CTRL_STEP);
	else 
		ret = -6; // BLU underflow
#endif

	return ret;
}

static int rdp360h_increase_brightness(void)
{
	U8_T value;
	int ret = 0;

#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	/* Read RDP360H_0 Red LED Current */
	value = RDP_REG_GET(0, 0xA6); 
	/* Check MAX and then Apply */
	if ((value & 0x3F) <= (RDP360H_LED_CTRL_MAX - LED_CTRL_STEP)) 
		RDP_REG_SET(0, 0xA6, value + LED_CTRL_STEP);
	else 
		ret = -1; // BLU overrflow

	/* Read RDP360H_0 Green LED Current */
	value = RDP_REG_GET(0, 0xA7);
	/* Check MAX and then Apply */
	if ((value) <= (RDP360H_LED_CTRL_MAX - LED_CTRL_STEP))
		RDP_REG_SET(0, 0xA7, value + LED_CTRL_STEP);
	else 
		ret = -2; // BLU overrflow

	/* Read RDP360H_0 Blue LED Current */
	value = RDP_REG_GET(0, 0xA8);
	/* Check MAX and then Apply */
	if (value <= (RDP360H_LED_CTRL_MAX - LED_CTRL_STEP))
		RDP_REG_SET(0, 0xA8,value + LED_CTRL_STEP);
	else 
		ret = -3; // BLU overrflow
#endif

#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	/* Read RDP360H_1 Red LED Current */
	value = RDP_REG_GET(1, 0xA6); 
	/* Check MAX and then Apply */
	if ((value & 0x3F) <= (RDP360H_LED_CTRL_MAX - LED_CTRL_STEP))
		RDP_REG_SET(1, 0xA6, value + LED_CTRL_STEP);
	else 
		ret = -4; // BLU overrflow

	/* Read RDP360H_1 Green LED Current */
	value = RDP_REG_GET(1, 0xA7);
	/* Check MAX and then Apply */
	if ((value) <= (RDP360H_LED_CTRL_MAX - LED_CTRL_STEP))
		RDP_REG_SET(1, 0xA7, value + LED_CTRL_STEP);
	else 
		ret = -5; // BLU overrflow

	/* Read RDP360H_1 Blue LED Current */
	value = RDP_REG_GET(1, 0xA8);
	/* Check MAX and then Apply */
	if ((value) <= (RDP360H_LED_CTRL_MAX - LED_CTRL_STEP))
		RDP_REG_SET(1, 0xA8,value + LED_CTRL_STEP);
	else 
		ret = -6; // BLU overrflow
#endif

	return ret;
}

static int rdp360h_get_brightness(void)
{
	return 0; // temp
}

static void rdp360h_set_brightness(int brightness)
{
}

#define MAX_LOOP_CNT 0x40
int rdp360h_time_sync (int panel_port)
{
	int ret = 0, inner_loop_err_cnt = 0;
    unsigned char read_data = 0x00;
    unsigned char loop_cnt = 0x00;
	U8_T rdc_0x40, rdp_0x12;

	rdc_0x40 = RDC_REG_GET(0x40);
	rdp_0x12 = RDP_REG_GET(panel_port, 0x12);

    RDC_REG_SET ( 0x40, 0x10); 

    RDC_REG_SET ( 0xe5, 0x04 ); // data fix
    RDC_REG_SET ( 0xe6, 0x02 ); // data 

    while ( read_data != 0x18 ) 
    {
        RDP_REG_SET ( panel_port, 0x64, 0x00 );  // 1C reg clear 
        RDP_REG_SET ( panel_port, 0x12, 0x03 );  // LVDS_write panel disable 

		while ( read_data != 0x02 ) // check data 
        {
            if (panel_port){
                RDC_REG_SET ( 0xF7, 0x6C ); // ldt test hs0 pd enable 
                RDC_REG_SET ( 0xF7, 0x64 ); // ldt test hs0 pd disable
            }
            else {
                RDC_REG_SET ( 0xF7, 0x74 ); // ldt test hs1 pd enable 
                RDC_REG_SET ( 0xF7, 0x64 ); // ldt test hs1 pd disable
            }
            read_data = RDP_REG_GET ( panel_port, 0x0b ); // channel data read
			if (loop_cnt == MAX_LOOP_CNT) {
				//EMSG("Panel time sync Timeout 0!\n");
				ret = -1;
				goto OUT_LOOP;
			}
			else {
				VC_M_DELAY(1);
				loop_cnt++;
				inner_loop_err_cnt++;
			}
        }

        RDP_REG_SET (panel_port, 0x12, 0x07 );  // LVDS_write panel enable 

        //rdp_write_int ( 0x64, 0x18 );
		{
		U8_T b[3] = {0x80, 0x64, 0x18};
		rtiVC_WriteBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0, vc_rdc_i2c_slave_addr, 
							b, 3);
		}

        read_data = RDP_REG_GET ( panel_port, 0x64 );
		if (loop_cnt == MAX_LOOP_CNT) {
			EMSG("Panel time sync Timeout 1!\n");
			ret = -2;
			break;
		}
        else
			VC_M_DELAY(1);
    }

OUT_LOOP:

    RDC_REG_SET ( 0xe5, 0x00 ); 

   // FLASH_4BYTE_CMD (CMD_READ , RDC_REG_ADDR_H, RDC_REG_ADDR_M, 0x40);
    //RDC_MASK_WRITE(0x40, 0x0F, FLASH_RXDATA_NP0_REG); 
	RDC_REG_MASK_SET(0x40, 0x0F, rdc_0x40);

   // FLASH_4BYTE_CMD (CMD_READ , RDP_REG_ADDR_H, RDP_REG_ADDR_M, 0x12);
    //RDP_REG_SET ( panel_port, 0x12, FLASH_RXDATA_NP0_REG ); // OFF_CTRL_MODE, CTRL_panel_SEL
	RDP_REG_SET (panel_port, 0x12, rdp_0x12); 

	return 0;
}

static int rdp360h_set_led_current(E_VC_PANEL_CTRL_PORT_T ctrl_port,
									E_VC_LED_CTRL_COLOR_T color,
									VC_LED_CURRENT_INFO_T new_current[MAX_NUM_VC_PANEL_PORT])
{
	int i;
	U8_T chk_port;
	VC_LED_CURRENT_INFO_T *current_ptr = NULL;

	// Calculate first!
	chk_port = (U8_T)ctrl_port;
	for (i = VC_PANEL_PORT_0; i < MAX_NUM_VC_PANEL_PORT; i++) {
		if (chk_port & 0x1) {
			current_ptr = &new_current[i];

			if (color & VC_LED_CTRL_COLOR_R) {
				if (current_ptr->R < MIN_RDP360H_LED_CURRENT_VALUE) {
					EMSG("R: Can't step to the down side anymore for port %d\n", i);
					//return -1;
					current_ptr->R = MIN_RDP360H_LED_CURRENT_VALUE;
				}

				if (current_ptr->R > MAX_RDP360H_LED_CURRENT_VALUE) {
					EMSG("R: Can't step to the up side anymore for port %d\n", i);
					current_ptr->R = MAX_RDP360H_LED_CURRENT_VALUE;
				}
			}

			if (color & VC_LED_CTRL_COLOR_G) {
				if (current_ptr->G < MIN_RDP360H_LED_CURRENT_VALUE) {
					EMSG("G: Can't step to the down side anymore for port %d\n", i);
					current_ptr->G = MIN_RDP360H_LED_CURRENT_VALUE;
				}

				if (current_ptr->G > MAX_RDP360H_LED_CURRENT_VALUE) {
					EMSG("G: Can't step to the up side anymore for port %d\n", i);
					current_ptr->G = MAX_RDP360H_LED_CURRENT_VALUE;
				}
			}

			if (color & VC_LED_CTRL_COLOR_B) {
				if (current_ptr->B < MIN_RDP360H_LED_CURRENT_VALUE) {
					EMSG("B: Can't step to the down side anymore for port %d\n", i);
					current_ptr->B = MIN_RDP360H_LED_CURRENT_VALUE;
				}

				if (current_ptr->B > MAX_RDP360H_LED_CURRENT_VALUE) {
					EMSG("B: Can't step to the up side anymore for port %d\n", i);
					current_ptr->B = MAX_RDP360H_LED_CURRENT_VALUE;
				}
			}
		}
		chk_port >>= 1;
	}

#if 0
	DMSG("0: 0xA6(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDC_RDP0, 0xA6) & 0x3F);
	DMSG("0: 0xA7(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDC_RDP0, 0xA7) & 0x3F);
	DMSG("0: 0xA8(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDC_RDP0, 0xA8) & 0x3F);
	DMSG("1: 0xA6(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDP1, 0xA6) & 0x3F);
	DMSG("1: 0xA7(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDP1, 0xA7) & 0x3F);
	DMSG("1: 0xA8(0x%02X)\n", RDP_REG_GET(VC_DEVICE_CH_RDP1, 0xA8) & 0x3F);
#endif

	if (color & VC_LED_CTRL_COLOR_R) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDC_RDP0, 0xA6, 0x3F, new_current[VC_PANEL_PORT_0].R);

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDP1, 0xA6, 0x3F, new_current[VC_PANEL_PORT_1].R);
	}	

	if (color & VC_LED_CTRL_COLOR_G) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDC_RDP0, 0xA7, 0x3F, new_current[VC_PANEL_PORT_0].G);

		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDP1, 0xA7, 0x3F, new_current[VC_PANEL_PORT_1].G);
	}
	
	if (color & VC_LED_CTRL_COLOR_B) {
		if (ctrl_port & VC_PANEL_CTRL_PORT_0)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDC_RDP0, 0xA8, 0x3F, new_current[VC_PANEL_PORT_0].B);
		
		if (ctrl_port & VC_PANEL_CTRL_PORT_1)
			RDP_REG_MASK_SET(VC_DEVICE_CH_RDP1, 0xA8, 0x3F, new_current[VC_PANEL_PORT_1].B);
	}

	return 0;
}

static void rdp360h_get_led_current_from_reg(E_VC_PANEL_PORT_T port,
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
		info->R = RDP_REG_GET(dev_ch, 0xA6) & 0x3F;
	
	if (color & VC_LED_CTRL_COLOR_G)
		info->G = RDP_REG_GET(dev_ch, 0xA7) & 0x3F;
	
	if (color & VC_LED_CTRL_COLOR_B)
		info->B = RDP_REG_GET(dev_ch, 0xA8) & 0x3F;	
}

static int rdp360h_get_led_current(struct _VC_PANEL_DEV_INFO_T *dev,
							E_VC_PANEL_CTRL_PORT_T ctrl_port,
							VC_LED_CURRENT_INFO_T info[MAX_NUM_VC_PANEL_PORT])
{
	if (ctrl_port & VC_PANEL_CTRL_PORT_0)
		info[VC_PANEL_PORT_0] = dev->led_current_info[VC_PANEL_PORT_0];

	if (ctrl_port & VC_PANEL_CTRL_PORT_1)
		info[VC_PANEL_PORT_1] = dev->led_current_info[VC_PANEL_PORT_1];

	return 0;
}

int rdp360h_get_led_level_range(U8_T *min, U8_T *max)
{
	*min = MIN_RDP360H_LED_CURRENT_VALUE;
	*max = MAX_RDP360H_LED_CURRENT_VALUE;

	return 0;
}


int rdp360h_get_device_id(E_VC_PANEL_PORT_T port, U16_T *chip_id, U16_T *rev_num)
{
	U8_T reg0x00, reg0x01;

	reg0x00 = RDP_REG_GET(port, 0x00);
	reg0x01 = RDP_REG_GET(port, 0x01);

	*chip_id = ((U16_T)reg0x00<<4) | (reg0x01>>4);
	*rev_num = reg0x01 & 0x0F;

	return 0;
}

static int rdp360h_prepare_panel(VC_PANEL_DEV_INFO_T *dev)
{
#ifdef _CFG_RDP_ATTACHED_TO_PORT_0
	rdp360h_get_led_current_from_reg(VC_PANEL_PORT_0, VC_LED_CTRL_COLOR_ALL,
									&dev->led_current_info[VC_PANEL_PORT_0]);
#endif
#ifdef _CFG_RDP_ATTACHED_TO_PORT_1
	rdp360h_get_led_current_from_reg(VC_PANEL_PORT_1, VC_LED_CTRL_COLOR_ALL,
									&dev->led_current_info[VC_PANEL_PORT_1]);
#endif

	return 0;
}

int RDP360H_PanelProbe(VC_PANEL_DEV_INFO_T *dev)
{
	VC_BACKLIGHT_INFO_T *bl = &dev->bl;

	dev->prepare_panel = rdp360h_prepare_panel;
	dev->get_device_id = rdp360h_get_device_id;

	dev->get_led_current = rdp360h_get_led_current;
	dev->set_led_current = rdp360h_set_led_current;
	dev->get_led_level_range = rdp360h_get_led_level_range;

	dev->set_brightness = rdp360h_set_brightness;
	dev->get_brightness = rdp360h_get_brightness;

	dev->increase_brightness = rdp360h_increase_brightness;
	dev->decrease_brightness = rdp360h_decrease_brightness;

	bl->max_brightness = RDP360H_LED_CTRL_MAX; // ????

	return 0;
}

#endif

