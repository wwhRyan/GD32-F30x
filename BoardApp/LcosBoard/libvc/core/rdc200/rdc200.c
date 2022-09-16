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
#include "./../rti_vc_rdc.h"
#include "./../rti_vc_panel.h"

#if defined(CFG_MDC_RDC200)
#include "rdc200_flash_partitions.h"

#define RDC_XTAL_FREQ_MHz	32 //32000000

#define RDC200_STEP_DOWN_LED_DUTY_VALUE(led_duty)\
	(S32_T)((led_duty) - (_CFG_LED_DUTY_CTRL_STEP * VC_LED_DUTY_DIVIDER))

#define RDC200_STEP_UP_LED_DUTY_VALUE(led_duty)\
	(S32_T)((led_duty) + (_CFG_LED_DUTY_CTRL_STEP * VC_LED_DUTY_DIVIDER))


#define RDC200_STEP_DOWN_FRAME_RATE_VALUE(frame_rate)\
	((frame_rate) - (_CFG_FRAME_RATE_CTRL_STEP * VC_FRAME_RATE_DIVIDER))

#define RDC200_STEP_UP_FRAME_RATE_VALUE(frame_rate)\
	((frame_rate) + (_CFG_FRAME_RATE_CTRL_STEP * VC_FRAME_RATE_DIVIDER))

//
extern int rdp360h_time_sync(int panel_port);
extern int rdp550f_time_sync(int panel_port);

#define RDC200_REG_BURST_SET(buf, size)\
	rtiVC_WriteBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0,\
						vc_rdc_i2c_slave_addr, buf, size)

#define RDC200_REG_BURST_GET(w_buf, w_size, r_buf, r_size)\
	rtiVC_ReadBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0, vc_rdc_i2c_slave_addr,\
							w_buf, w_size, r_buf, r_size)

///
#define RDC200_XTAL_FREQ_Hz	(32 *  1000000)

#define RDC200_MAX_NUM_FONT_CHAR_H	32
#define RDC200_MAX_NUM_FONT_CHAR_V	8
#define RDC200_MAX_NUM_FONT_CHAR_PER_BOX\
		(RDC200_MAX_NUM_FONT_CHAR_H*RDC200_MAX_NUM_FONT_CHAR_V)

#define RDC200_FONT_BOX_WITDH	256
#define RDC200_FONT_BOX_HEGIHT	128

#define RDC200_BMP_RES_WITDH	160
#define RDC200_BMP_RES_HEGIHT	96


#define LED_CTRL_MASK_PORT_0		0x01
#define LED_CTRL_MASK_PORT_1		0x02

#define LED_CTRL_MASK_BRIGHTNESS	0x08

#define LED_CTRL_MASK_RED			0x10
#define LED_CTRL_MASK_GREEN			0x20
#define LED_CTRL_MASK_BLUE			0x40

#define LED_DONE_FLAG_MASK			0x01
#define LED_ERR_FLAG_MASK			0x02
static U8_T led_ctrl_intr_flag[2];

// Output resolution of register
typedef struct {
	U16_T h_size;
	U16_T v_size;
	U8_T lvdx_line;
} RDC200_OUTPUT_RES_REG_T;

static const RDC200_OUTPUT_RES_REG_T rdc200_output_res_reg[] = 
{
	{0, 0, 0}, // control using output_h_size, output_v_size register
	{1280, 720, 4}, // HD, 1280 x 720
	{1920, 1080, 8}, // FHD, 1920 x 1080
	{2048, 1024, 8}, // 2K1K, 2048 x 1024
	{2560, 1440, 8}, // QHD, 2560 x 1440
	{2048, 2048, 8}, // 2K2K, 2048 x 2048
	{2560, 1600, 8}, // WQXGA, 2560 x 1600
	{2560, 720, 8} // HD3D, 2560 x 720
};

#define MAX_DEV_ACCESS_RETRY_CNT		240

#define RDC200_FLASH_SR_WIP	    0x01
#define RDC200_FLASH_SR_WEL     0x02
#define RDC200_FLASH_SR_BP0     0x04
#define RDC200_FLASH_SR_BP1     0x08
#define RDC200_FLASH_SR_BP2     0x10
#define RDC200_FLASH_SR_BP3     0x20
#define RDC200_FLASH_SR_QE      0x40
#define RDC200_FLASH_SR_SRWD    0x80
#define RDC200_FLASH_SR_BP3_0   (RDC200_FLASH_SR_BP0 | RDC200_FLASH_SR_BP1 | RDC200_FLASH_SR_BP2 | RDC200_FLASH_SR_BP3)

// RDC200 spec. 8 bytes per 1 operation
#define RDC200_FLASH_READ_SIZE		8

#define RDC200_FLASH_CMDB_SIZE	6

/**
 FLASH access code
*/
#define RDC200_FLASH_ACC_REG 	0x81

typedef enum {
    CMD_WRITE_STATUS_REG = 0x01,
    CMD_PROGRAM_PAGE = 0x02,
    CMD_READ = 0x03,
    CMD_WRITE_DISABLE = 0x04,

    CMD_READ_STATUS_REG1 = 0x05,

	CMD_WRITE_ENABLE = 0x06,

	CMD_SECTOR_ERASE_4K = 0x20,
	
	CMD_BLOCK_ERASE1_32K = 0x52,
	CMD_BLOCK_ERASE2_32K = 0x53,
	CMD_BLOCK_ERASE_64K = 0xD8,
    CMD_BULK_ERASE_0 = 0x60,

	CMD_READ_DEVICE_ID = 0x9F,
    CMD_RESUME_FROM_PWRDOWN = 0xAB,
    CMD_POWER_DOWN = 0xB9,
	CMD_BULK_ERASE_1 = 0xC7
} FLASH_COMMAND;

/**
 Command Buffers
*/
static U8_T cmdb_RDID[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_READ_DEVICE_ID, 0, 0, 0, 0};
static U8_T cmdb_RDSR[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_READ_STATUS_REG1, 0, 0, 0, 0};
static U8_T cmdb_WREN[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_WRITE_ENABLE, 0, 0, 0, 0};
static U8_T cmdb_SE[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_SECTOR_ERASE_4K, 0, 0, 0, 0};
static U8_T cmdb_CE[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_BULK_ERASE_0, 0, 0, 0, 0};
static U8_T cmdb_PP[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_PROGRAM_PAGE, 0, 0, 0, 0};
static U8_T cmdb_READ[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_READ, 0, 0, 0, 0};
static U8_T cmdb_WRSR[RDC200_FLASH_CMDB_SIZE] = {RDC200_FLASH_ACC_REG, CMD_WRITE_STATUS_REG, 0, 0, 0, 0};

static const U8_T rdc200_palette_drawer_sram_id[NUM_RDC_TEXT_DRAWER] = 
	{TSID_FONT_BG_COLOR_0, TSID_FONT_BG_COLOR_1, TSID_FONT_BG_COLOR_2, TSID_FONT_BG_COLOR_3};

static const U8_T rdc200_font_drawer_sram_id[NUM_RDC_TEXT_DRAWER] = 
	{TSID_FONT_BMP_0, TSID_FONT_BMP_1, TSID_FONT_BMP_2, TSID_FONT_BMP_3};

static const U8_T rdc200_bmp_drawer_sram_id[NUM_VC_BMP_DRAWER] = 
	{TSID_BMP_DRAWER_0, TSID_BMP_DRAWER_1, TSID_BMP_DRAWER_2, TSID_BMP_DRAWER_3};

static U8_T rdc200_read_SR(void)
{
	U8_T sr_buf[RDC200_FLASH_READ_SIZE];

	RDC200_REG_BURST_GET(cmdb_RDSR, RDC200_FLASH_CMDB_SIZE,
						sr_buf, RDC200_FLASH_READ_SIZE);

	//DMSG("RDSR: 0x%02X\n", sr_buf[0]);

	return sr_buf[0];
}

static U8_T rdc200_read_Flash_Device_ID(void)
{
    U8_T sr_buf[RDC200_FLASH_READ_SIZE];
    
    RDC200_REG_BURST_GET(cmdb_RDID, RDC200_FLASH_CMDB_SIZE,
                         sr_buf, RDC200_FLASH_READ_SIZE);
    
    DMSG("Flash Device ID : 0x%02X 0x%02X 0x%02X\n",
                        sr_buf[0],sr_buf[1],sr_buf[2]);
    
    return sr_buf[0];
}

static int rdc200_flash_read(U32_T addr, U8_T *buf, UINT_T len)
{
	int i, num_chunk = len >> 3; // divide by 8
	U16_T remain_bytes = len & (RDC200_FLASH_READ_SIZE -1); // mod by 8
	U8_T remain_buf[RDC200_FLASH_READ_SIZE];

	if (buf == NULL || len == 0) {
		EMSG("Read: Invalid parameters\n");
		return -1;
	}

	for (i = 0; i < num_chunk; i++) {
		// READ
		cmdb_READ[2] = (addr >> 16) & 0xFF;
		cmdb_READ[3] = (addr >> 8) & 0xFF; 
		cmdb_READ[4] = (addr & 0xFF);
		RDC200_REG_BURST_GET(cmdb_READ, RDC200_FLASH_CMDB_SIZE,
							buf, RDC200_FLASH_READ_SIZE);
		
		buf += RDC200_FLASH_READ_SIZE;
		addr += RDC200_FLASH_READ_SIZE;
	}
	
	if (remain_bytes) {
		// READ
		cmdb_READ[2] = (addr >> 16) & 0xFF;
		cmdb_READ[3] = (addr >> 8) & 0xFF; 
		cmdb_READ[4] = (addr & 0xFF);

		RDC200_REG_BURST_GET(cmdb_READ, RDC200_FLASH_CMDB_SIZE,
							remain_buf, RDC200_FLASH_READ_SIZE);

		/* Copy data into output buffer. */
		for (i = 0; i < remain_bytes; i++)
			buf[i] = remain_buf[i];
	}

	return 0;
}

static int rdc200_flash_program(U32_T addr, const U8_T *buf, UINT_T len)
{
	UINT_T i, retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;
	
	for (i = 0; i < len; i++) {
		while (1) {
			// WREN (Write ENable)
			RDC200_REG_BURST_SET(cmdb_WREN, RDC200_FLASH_CMDB_SIZE);

			if (rdc200_read_SR() & RDC200_FLASH_SR_WEL) {
				//DMSG("OK");
				break;
			}

			if (--retry_cnt == 0) {
				EMSG("Program: WREN Timeout!\n"); 
				return -1;
			}
		}

		// Program
		cmdb_PP[2] = (addr >> 16) & 0xFF;
		cmdb_PP[3] = (addr >> 8) & 0xFF; 
		cmdb_PP[4] = (addr & 0xFF);
		cmdb_PP[5] = buf[i];
		RDC200_REG_BURST_SET(cmdb_PP, RDC200_FLASH_CMDB_SIZE);

		// Check Status Register
		while (1) {
			if ((rdc200_read_SR() & RDC200_FLASH_SR_WIP) == 0x0)
				break;

			if (--retry_cnt == 0) {
				EMSG("Program: Program Timeout!\n"); 
				return -2;
			}
		}

		addr += 1;
	}

	return 0;
}

static int rdc200_flash_erase_sector(U32_T sector_addr)
{
	int retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;

	if (rdc200_read_SR() & (RDC200_FLASH_SR_SRWD | RDC200_FLASH_SR_BP3_0)) {
		EMSG("[FLASH] Protect enabled 0x%02X\n", rdc200_read_SR());
		return -1;
	}

	while (1) {
		// WREN (Write ENable)
		RDC200_REG_BURST_SET(cmdb_WREN, RDC200_FLASH_CMDB_SIZE);

		if (rdc200_read_SR() & RDC200_FLASH_SR_WEL)	{
			//DMSG("OK");
			break;
		}

		if (--retry_cnt == 0) {
			EMSG("Sector erase: WREN Timeout!\n"); 
			return -1;
		}

		VC_M_DELAY(1);
	}
	
	// SE (Sector Erase)
	cmdb_SE[2] = (sector_addr >> 16) & 0xFF;
	cmdb_SE[3] = (sector_addr >> 8) & 0xFF; 
	cmdb_SE[4] = (sector_addr & 0xFF);
	RDC200_REG_BURST_SET(cmdb_SE, RDC200_FLASH_CMDB_SIZE);

	// Check Status Register
	retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;
	while (1) {
		if ((rdc200_read_SR() & RDC200_FLASH_SR_WIP) == 0x0)
			break;

		if (--retry_cnt == 0) {
			EMSG("Sector erase: CMD Timeout!\n"); 
			return -2;
		}

		VC_M_DELAY(1);
	}

	return 0;
}

static int rdc200_flash_erase_chip(void)
{
	int retry_cnt;

	// WREN (Write ENable)
	RDC200_REG_BURST_SET(cmdb_WREN, RDC200_FLASH_CMDB_SIZE);

	// Check Status Register
	retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;
	while (1) {
		if (rdc200_read_SR() & RDC200_FLASH_SR_WEL)	{
			//DMSG("OK");
			break;
		}

		if (--retry_cnt == 0) {
			EMSG("Chip Erase: WREN Timeout!\n"); 
			return -1;
		}

		VC_M_DELAY(1);
	}

	RDC200_REG_BURST_SET(cmdb_CE, RDC200_FLASH_CMDB_SIZE);

	// Check Status Register
	retry_cnt = 60000;
	while (1) {
		if ((rdc200_read_SR() & RDC200_FLASH_SR_WIP) == 0x0)
			break;

		if (--retry_cnt == 0) {
			EMSG("Chip Erase: CMD Timeout!\n"); 
			return -2;
		}

		VC_M_DELAY(1);
	}

#if 0 // check erased data
{
	int i;
	static U8_T tmp[100];

	if (rdc200_flash_read(0x0, tmp, 100) == 0) {
		DMSG("Chip erase result\n");

		for (i = 0; i < 100; i++) {
			if (((i+1) % 16) == 0)
				DMSG("\n");
			else
				DMSG("0x%02X ", tmp[i]);
		}
	}
}
#endif

	return 0;
}

static int rdc200_flash_disable_protect(void)
{
	int retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;
    
    DMSG("rdc200_flash_disable_protect : 0x%02X\n", rdc200_read_SR());
    
	while (1) {
		// WREN (Write ENable)
		RDC200_REG_BURST_SET(cmdb_WREN, RDC200_FLASH_CMDB_SIZE);

		if (rdc200_read_SR() & RDC200_FLASH_SR_WEL)	{
			//DMSG("OK");
			break;
		}

		if (--retry_cnt == 0) {
			EMSG("rdc200_flash_disable_protect : WREN Timeout!\n");
			return -1;
		}

		VC_M_DELAY(1);
	}

	cmdb_WRSR[5] = 0x00;
	RDC200_REG_BURST_SET(cmdb_WRSR, RDC200_FLASH_CMDB_SIZE);

	// Check Status Register
	retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;
	while (1) {
		if ((rdc200_read_SR() & RDC200_FLASH_SR_WIP) == 0x0)
			break;

		if (--retry_cnt == 0) {
			EMSG("rdc200_flash_disable_protect : WIP CMD Timeout!\n");
			return -2;
		}

		VC_M_DELAY(1);
	}

	return 0;
}

static int rdc200_flash_enable_protect(void)
{
	int retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;

	while (1) {
		// WREN (Write ENable)
		RDC200_REG_BURST_SET(cmdb_WREN, RDC200_FLASH_CMDB_SIZE);

		if (rdc200_read_SR() & RDC200_FLASH_SR_WEL) {
			//DMSG("OK");
			break;
		}
        
		if (--retry_cnt == 0) {
			EMSG("rdc200_flash_enable_protect : WREN Timeout!\n");
			return -1;
		}
        
        VC_M_DELAY(1);
    }

    cmdb_WRSR[5] = (RDC200_FLASH_SR_SRWD | RDC200_FLASH_SR_BP3_0);
    RDC200_REG_BURST_SET(cmdb_WRSR, RDC200_FLASH_CMDB_SIZE);

    // Check Status Register
    retry_cnt = MAX_DEV_ACCESS_RETRY_CNT;
    while (1) {
        if ((rdc200_read_SR() & RDC200_FLASH_SR_WIP) == 0x0)
            break;

        if (--retry_cnt == 0) {
            EMSG("rdc200_flash_enable_protect : WIP CMD Timeout!\n");
            return -2;
        }
        
        VC_M_DELAY(1);
    }
    
    
    DMSG("rdc200_flash_enable_protect : 0x%02X\n", rdc200_read_SR());
    
	return 0;
}

static int rdc200_flash_prepare(void)
{
    rdc200_read_Flash_Device_ID();
	rdc200_flash_disable_protect();

	return 0;
}

static int rdc200_flash_finish(void)
{
	rdc200_flash_enable_protect();

	return 0;
}

static int rdc200_drive_panel(BOOL_T on)
{
	int ret = 0;

	if (on == TRUE) {
		RDC_REG_SET(0x22, 0x00);
		VC_M_DELAY(1);	
		RDC_REG_SET(0x22, 0x02);
		VC_M_DELAY(1);
		RDC_REG_SET(0x22, 0x00);
		VC_M_DELAY(1);
		RDC_REG_SET(0x22, 0x01);
		VC_M_DELAY(20);
	}
	else {
		RDC_REG_SET(0x22, 0x01);
		VC_M_DELAY(1);	
		RDC_REG_SET(0x22, 0x02);
		VC_M_DELAY(1);
		RDC_REG_SET(0x22, 0x00);
		VC_M_DELAY(1);
	}

	return ret;
}

int rdc200_poweron_panel(BOOL_T on)
{
	int ret = 0;

	if (on == TRUE) {
		rdc200_drive_panel(TRUE);
		RDC_REG_SET(0x29, (RDC_REG_GET(0x29) ^ 0x01)); // dcdc_enable
	}
	else {
		RDC_REG_SET(0x29, (RDC_REG_GET(0x29) ^ 0x01)); // dcdc disable
		rdc200_drive_panel(FALSE);
	}

	return ret;
}

static int rdc200_update_lut(U32_T addr, U32_T size, U8_T id)
{
	int ret = 0;
	U16_T timeout = 10000; // us: Max 10ms
	U8_T read_data = 0x00;

	if (!size) {
		EMSG("Invalid LUT size\n");
		ret = -1;
		goto up_lut_err;
	}
	
	size--;

	RDC_REG_SET(0x11, 0x02);
	RDC_REG_SET(0x17, size & 0xFF);
	RDC_REG_SET(0x16, (size>>8) & 0xFF);
	RDC_REG_SET(0x15, (size>>16) & 0xFF);
	RDC_REG_SET(0x14, addr & 0xFF);
	RDC_REG_SET(0x13, (addr>>8) & 0xFF);
	RDC_REG_SET(0x12, (addr>>16) & 0xFF);
	RDC_REG_SET(0x18, id);
	RDC_REG_SET(0x11, 0x03);

	/* Wait update LUT */
	while (1) { 
		read_data = RDC_REG_GET(0x1A);
		if ((~read_data & 0x20) == 0)
			break;

		if (--timeout == 0) {
			//EMSG("LUT update timeout error!\n"); 
			ret = -2;
			goto up_lut_err;
		}

		VC_U_DELAY(1);		
	}

	return 0;

up_lut_err:
	EMSG("LUT update error (%d)\n", ret); 

	return ret;
}

static int rdc200_enable_gamma_correction(E_VC_PANEL_CTRL_PORT_T ctrl_port,
								BOOL_T enable,
								UINT_T lut_offset[MAX_NUM_VC_PANEL_PORT])
{
	U8_T read_data = 0x00;
	U32_T base_addr = FLASH_PART_OFFSET_PANEL_GAMMA_BASE;
	UINT_T lut_o;

	if (ctrl_port & VC_PANEL_CTRL_PORT_0)
		lut_o = lut_offset[VC_PANEL_PORT_0];
	else
		lut_o = lut_offset[VC_PANEL_PORT_1];

	rdc200_drive_panel(FALSE);

	// Disable gamma correction 
	read_data = RDC_REG_GET(0x07);
	RDC_REG_SET(0x07, read_data & 0xFE);

	if (enable == TRUE) {
		rdc200_update_lut(base_addr+FLASH_ITEM_OFFSET_GAMMA_LOW_RED(lut_o),
							FLASH_SUBITEM_SIZE_PANNEL_GAMMA,
							TSID_GC_LOW_RED); 

		rdc200_update_lut(base_addr+FLASH_ITEM_OFFSET_GAMMA_LOW_GREEN(lut_o),
							FLASH_SUBITEM_SIZE_PANNEL_GAMMA,
							TSID_GC_LOW_GREEN);

		rdc200_update_lut(base_addr+FLASH_ITEM_OFFSET_GAMMA_LOW_BLUE(lut_o),
							FLASH_SUBITEM_SIZE_PANNEL_GAMMA,
							TSID_GC_LOW_BLUE);

		rdc200_update_lut(base_addr+FLASH_ITEM_OFFSET_GAMMA_HIGH_RED(lut_o),
							FLASH_SUBITEM_SIZE_PANNEL_GAMMA,
							TSID_GC_HIGH_RED);		

		rdc200_update_lut(base_addr+FLASH_ITEM_OFFSET_GAMMA_HIGH_GREEN(lut_o),
							FLASH_SUBITEM_SIZE_PANNEL_GAMMA,
							TSID_GC_HIGH_GREEN);

		rdc200_update_lut(base_addr+FLASH_ITEM_OFFSET_GAMMA_HIGH_BLUE(lut_o),
							FLASH_SUBITEM_SIZE_PANNEL_GAMMA,
							TSID_GC_HIGH_BLUE);

		// enable gamma correction 
		read_data = RDC_REG_GET(0x07);
		RDC_REG_SET(0x07, read_data | 0x01);
	}

	rdc200_drive_panel(TRUE);

	return 0;
}

static int rdc200_get_output_resolution(U32_T *h_size, U32_T *v_size)
{
	U8_T output_size_idx;
	int lvdx_line = -1; /* Set default as manual */
	
	output_size_idx = RDC_REG_GET(0x41) & 0x07; // <2:0>

	if (output_size_idx != 0) {
		*h_size = rdc200_output_res_reg[output_size_idx].h_size;
		*v_size = rdc200_output_res_reg[output_size_idx].v_size;

		lvdx_line = rdc200_output_res_reg[output_size_idx].lvdx_line;
	}
	else {
		*h_size = RDC_REG_GET(0x42) << 4; // unit : 16 pixels
		*v_size = RDC_REG_GET(0x43) << 3; // unit : 8 lines	
	}

	return lvdx_line;
}

static BOOL_T rdc200_check_text_position(U32_T *h_output_res,
											U32_T x, U32_T y,
											E_VC_OSD_TEXT_MAG_T mag,
											E_VC_OSD_DIMENSTION_T d)
{
	U32_T output_h_size, output_v_size;
	U32_T font_h_size = RDC200_FONT_BOX_WITDH, font_v_size = RDC200_FONT_BOX_HEGIHT;

	rdc200_get_output_resolution(&output_h_size, &output_v_size);

	*h_output_res = output_h_size;

	switch ((int)mag) {
	case VC_TEXT_OSD_MAG_x1:
		// Do nothing
		break;

	case VC_TEXT_OSD_MAG_x2:
		font_h_size <<= 1;
		font_v_size <<= 1;
		break;

	case VC_TEXT_OSD_MAG_x3:
		font_h_size *= 3;
		font_v_size *= 3;
		break;

	case VC_TEXT_OSD_MAG_x4:
		font_h_size <<= 2;
		font_v_size <<= 2;
		break;
		
	case VC_TEXT_OSD_MAG_x5:
		font_h_size *= 5;
		font_v_size *= 5;
		break;

	case VC_TEXT_OSD_MAG_x6:
		font_h_size *= 6;
		font_v_size *= 6;
		break;

	case VC_TEXT_OSD_MAG_x7:
		font_h_size *= 7;
		font_v_size *= 7;
		break;

	case VC_TEXT_OSD_MAG_x8:
		font_h_size <<= 3;
		font_v_size <<= 3;
		break;

	default:
		break;
	}

	if (d == VC_OSD_DIMENSTION_3D) {
		output_h_size >>= 1; // Divide by 2 (Side By Side)

		if (((x+font_h_size) < output_h_size) && (y < output_v_size))
			return TRUE;
		else
			return FALSE;
	}
	else {
		if ((x < output_h_size) && (y < output_v_size))
			return TRUE;
		else
			return FALSE;
	}
}

static BOOL_T rdc200_check_bitmap_position(U32_T *h_output_res,
										U32_T x, U32_T y,
										E_VC_OSD_BITMAP_MAG_T mag,
										E_VC_OSD_DIMENSTION_T d)
{
	U32_T output_h_size, output_v_size;
	U32_T bmp_h_size = RDC200_BMP_RES_WITDH, bmp_v_size = RDC200_BMP_RES_HEGIHT;

	rdc200_get_output_resolution(&output_h_size, &output_v_size);

	*h_output_res = output_h_size;

	switch (mag) {
	case VC_BITMAP_OSD_MAG_x1:
		// Do nothing
		break;

	case VC_BITMAP_OSD_MAG_x2:
		bmp_h_size <<= 1;
		bmp_v_size <<= 1;
		break;

	case VC_BITMAP_OSD_MAG_x3:
		bmp_h_size *= 3;
		bmp_v_size *= 3;
		break;

	case VC_BITMAP_OSD_MAG_x4:
		bmp_h_size <<= 2;
		bmp_v_size <<= 2;
		break;

	default:
		break;
	}

	if (d == VC_OSD_DIMENSTION_3D) {
		output_h_size >>= 1; // Divide by 2 (Side By Side)

		if (((x+bmp_h_size) < output_h_size) && (y < output_v_size))
				return TRUE;
			else
				return FALSE;
	}
	else {
		if ((x < output_h_size) && (y < output_v_size))
			return TRUE;
		else
			return FALSE;
	}
}

static int rdc200_disable_text_osd(VC_TEXT_DRAWER_CB *cb)
{
	U8_T drawer_id = cb->drawer_id;

	RDC_REG_MASK_SET(0x0B, ((1<<drawer_id) << 4), 0);
	RDC_REG_MASK_SET(0x11, ((1<<drawer_id) << 4), 0); // 3D

	return 0;
}

static int rdc200_hide_text_osd(VC_TEXT_DRAWER_CB *cb)
{
	U8_T reg_0x02, reg_0x03;
	U8_T drawer_id = cb->drawer_id;

	reg_0x03 = RDC_REG_GET(0x03);
	RDC_REG_SET(0x03, 0x9D);

	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x10, (1<<drawer_id)|0x1); // drw_sel data
	RDC_REG_SET(0x0E, 0x0A); // font_fsm command : drw_sel
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x0F, 0x00); // alpha data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x07); // font_fsm command : alpha
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x03, reg_0x03);

	return 0;
}

static int rdc200_show_text_osd(VC_TEXT_DRAWER_CB *cb)
{
	U8_T reg_0x03, reg_0x02;
	U8_T drawer_id = cb->drawer_id;

	reg_0x03 = RDC_REG_GET(0x03);
	RDC_REG_SET(0x03, 0x9D);

	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x10, (1<<drawer_id)|0x1); // drw_sel data
	RDC_REG_SET(0x0E, 0x0A); // font_fsm command : drw_sel
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x0F, (cb->back_alpha<<4)|cb->font_alpha); // alpha data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x07); // font_fsm command : alpha
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x03, reg_0x03);

	return 0;
}

static int rdc200_change_dimension_text_osd(VC_TEXT_DRAWER_CB *cb,
											E_VC_OSD_DIMENSTION_T d)
{
	U8_T font_pos_x, font_pos_y, font_3d_pos_x;
	U32_T output_h_size;
	U8_T drawer_id = cb->drawer_id;	

	if (d == VC_OSD_DIMENSTION_3D) {
		if (!rdc200_check_text_position(&output_h_size, cb->pos_x, cb->pos_y, cb->mag, d)) {
			EMSG("Invalid text size to drawing\n");
			return -1;
		}

		font_pos_x = cb->pos_x >> 3; // Divide by 8 (8 pixel unit)
		font_pos_y = cb->pos_y >> 3; // Divide by 8 (8 pixel unit)

		output_h_size >>= 1; // Divide by 2 (Side By Side)
		font_3d_pos_x = font_pos_x + (output_h_size >> 3); // Divide by 8 (8 pixel unit)
		RDC_REG_MASK_SET(0x0B, 0x0F, drawer_id+4);	
		RDC_REG_SET(0x0C, font_3d_pos_x);
		RDC_REG_SET(0x0D, font_pos_y);

		RDC_REG_MASK_SET(0x11, ((1<<drawer_id) << 4), ((1<<drawer_id) << 4));
	}
	else
		RDC_REG_MASK_SET(0x11, ((1<<drawer_id) << 4), 0);

	return 0;
}

static int rdc200_change_property_text_osd(VC_TEXT_DRAWER_CB *cb,
										E_VC_OSD_TEXT_MAG_T mag,
										E_VC_OSD_ALPHA_T font_alpha,
										E_VC_OSD_ALPHA_T back_alpha)
{
	U8_T reg_0x02, reg_0x03;
	E_VC_OSD_TEXT_MAG_T mag_y;
	U32_T output_h_size;
	U8_T drawer_id = cb->drawer_id;

	if (!rdc200_check_text_position(&output_h_size, cb->pos_x, cb->pos_y, mag, cb->d)) {
		EMSG("Invalid text size to drawing\n");
		return -1;
	}

	if (cb->d == VC_OSD_DIMENSTION_2D)
		mag_y = mag;
	else
		mag_y = (E_VC_OSD_TEXT_MAG_T)(mag + (E_VC_OSD_TEXT_MAG_T)1); // Twice of x

	switch (drawer_id) {
	case 0:
		RDC_REG_MASK_SET(0x3D, 0xFC, (mag<<5)|(mag_y<<2));
		break;

	case 1:
		RDC_REG_MASK_SET(0x3D, 0x03, (mag>>1));
		RDC_REG_MASK_SET(0x3E, 0xF0, ((mag & 0x1)<<7)|(mag_y<<4));
		break;

	case 2:
		RDC_REG_MASK_SET(0x3E, 0x0F, (mag<<1)|(mag_y>>2));
		RDC_REG_MASK_SET(0x3F, 0xC0, (mag_y&0x3)<<2);
		break;

	case 3:
		RDC_REG_MASK_SET(0x3F, 0x3F, (mag<<3)|(mag_y));
		break;
	}

	reg_0x03 = RDC_REG_GET(0x03);
	RDC_REG_SET(0x03, 0x9D);

	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x10, (1<<drawer_id)|0x1); // drw_sel data
	RDC_REG_SET(0x0E, 0x0A); // font_fsm command : drw_sel
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x0F, (back_alpha<<4)|font_alpha); // alpha data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x07); // font_fsm command : alpha
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x03, reg_0x03);

	return 0;
}

static int rdc200_move_text_osd(VC_TEXT_DRAWER_CB *cb, U32_T mov_x, U32_T mov_y)
{
	U8_T font_pos_x, font_pos_y, font_3d_pos_x;
	U8_T reg_0x02, reg_0x03;
	U32_T output_h_size;
	U8_T drawer_id = cb->drawer_id;

	if (!rdc200_check_text_position(&output_h_size, mov_x, mov_y, cb->mag, cb->d)) {
		EMSG("Invalid text size to drawing\n");
		return -1;
	}

	font_pos_x = mov_x >> 3; // Divide by 8 (8 pixel unit)
	font_pos_y = mov_y >> 3; // Divide by 8 (8 pixel unit)

	reg_0x03 = RDC_REG_GET(0x03);
	RDC_REG_SET(0x03, 0x9D);

	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x10, (1<<drawer_id)|0x1); // drw_sel data
	RDC_REG_SET(0x0E, 0x0A); // font_fsm command : drw_sel
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x0F, 0x00); // alpha data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x07); // font_fsm command : alpha
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	/* Frist, draw 2D text. */
	// Select drawer (cfg for 2D)
	RDC_REG_MASK_SET(0x0B, 0x0F, drawer_id);	
	RDC_REG_SET(0x0C, font_pos_x);
	RDC_REG_SET(0x0D, font_pos_y);

	if (cb->d == VC_OSD_DIMENSTION_3D) {
		output_h_size >>= 1; // Divide by 2 (Side By Side)
		font_3d_pos_x = font_pos_x + (output_h_size >> 3); // Divide by 8 (8 pixel unit)
		RDC_REG_MASK_SET(0x0B, 0x0F, drawer_id+4);	
		RDC_REG_SET(0x0C, font_3d_pos_x);
		RDC_REG_SET(0x0D, font_pos_y);
	}

	RDC_REG_SET(0x0F, (cb->back_alpha<<4)|cb->font_alpha); // alpha data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x07); // font_fsm command : alpha
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x03, reg_0x03);

	return 0;
}

static void rdc200_set_text_string(const char *string, size_t num)
{
	U8_T reg_0x02;
	size_t i;

	for (i = 0; i < num; i++) {
		RDC_REG_SET(0x0F, string[i]);

		RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
		reg_0x02 = RDC_REG_GET(0x02);
		RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
		RDC_REG_SET(0x0E, 0x05); // font_fsm command : put_char
		RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)
	}
}

static void rdc200_config_text_drawer(U8_T drawer_id, U32_T output_h_size,
										U32_T pos_x, U32_T pos_y,
										U32_T font_addr, U32_T palette_addr,
										E_VC_OSD_TEXT_MAG_T mag,
										E_VC_OSD_ALPHA_T font_alpha,
										E_VC_OSD_ALPHA_T back_alpha,
										U8_T font_color_idx, U8_T back_color_idx,
										E_VC_OSD_DIMENSTION_T d)
{
	U8_T ftdrw_en, font_pos_cfg, font_pos_x, font_pos_y, font_3d_pos_x;
	U8_T reg_0x02;
	E_VC_OSD_TEXT_MAG_T mag_y;

	if (font_addr != 0xFFFFFFFF)
		rdc200_update_lut(font_addr, FLASH_ITEM_SIZE_FONT,
							rdc200_font_drawer_sram_id[drawer_id]);

	if (palette_addr != 0xFFFFFFFF)
		rdc200_update_lut(palette_addr, 96,
							rdc200_palette_drawer_sram_id[drawer_id]);

	if (d == VC_OSD_DIMENSTION_2D)
		mag_y = mag;
	else
		mag_y = (E_VC_OSD_TEXT_MAG_T)(mag + (E_VC_OSD_TEXT_MAG_T)1); // Twice of x

	font_pos_cfg = 1 << drawer_id;
	ftdrw_en = 1 << drawer_id;

	font_pos_x = pos_x >> 3; // Divide by 8 (8 pixel unit)
	font_pos_y = pos_y >> 3; // Divide by 8 (8 pixel unit)

	/* Disable drawer include 3D. */
	//RDC_REG_MASK_SET(0x0B, (ftdrw_en << 4), 0);
	//RDC_REG_MASK_SET(0x11, (ftdrw_en << 4), 0); // 3D

	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x10, (1<<drawer_id)|0x1); // drw_sel data
	RDC_REG_SET(0x0E, 0x0A); // font_fsm command : drw_sel
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x0F, 0x00); // alpha data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x07); // font_fsm command : alpha
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	/* Frist, draw 2D text. */
	// Select drawer (cfg for 2D)
	RDC_REG_MASK_SET(0x0B, 0x0F, drawer_id);	
	RDC_REG_SET(0x0C, font_pos_x);
	RDC_REG_SET(0x0D, font_pos_y);

	if (d == VC_OSD_DIMENSTION_3D) {
		output_h_size >>= 1; // Divide by 2 (Side By Side)
		font_3d_pos_x = font_pos_x + (output_h_size >> 3); // Divide by 8 (8 pixel unit)
		RDC_REG_MASK_SET(0x0B, 0x0F, drawer_id+4);	
		RDC_REG_SET(0x0C, font_3d_pos_x);
		RDC_REG_SET(0x0D, font_pos_y);
	}

	switch (drawer_id) {
	case 0:
		RDC_REG_MASK_SET(0x3D, 0xFC, (mag<<5)|(mag_y<<2));
		break;

	case 1:
		RDC_REG_MASK_SET(0x3D, 0x03, (mag>>1));
		RDC_REG_MASK_SET(0x3E, 0xF0, ((mag & 0x1)<<7)|(mag_y<<4));
		break;

	case 2:
		RDC_REG_MASK_SET(0x3E, 0x0F, (mag<<1)|(mag_y>>2));
		RDC_REG_MASK_SET(0x3F, 0xC0, (mag_y&0x3)<<2);
		break;

	case 3:
		RDC_REG_MASK_SET(0x3F, 0x3F, (mag<<3)|(mag_y));
		break;
	}

	RDC_REG_MASK_SET(0x0B, ((1<<drawer_id) << 4), ((1<<drawer_id) << 4));

	RDC_REG_SET(0x0F, (back_color_idx<<4)|font_color_idx); // color data data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x06); // font_fsm command : color inst
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)

	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x03); // font_fsm command : clear
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)	

	RDC_REG_SET(0x0F, (back_alpha<<4)|font_alpha); // alpha data
	RDC_REG_SET(0x0E, 0x02); // font_fsm command : rdy
	reg_0x02 = RDC_REG_GET(0x02);
	RDC_REG_SET(0x02, 0x10); // input selection MIPI flag (RX_CLK DOWN)
	RDC_REG_SET(0x0E, 0x07); // font_fsm command : alpha
	RDC_REG_SET(0x02, reg_0x02); // input selection default (RX_CLK ON)
}

static int rdc200_draw_text_osd(VC_TEXT_DRAWER_CB *cb, U32_T pos_x, U32_T pos_y,
									U32_T font_addr, U32_T palette_addr,
									E_VC_OSD_TEXT_MAG_T mag,
									E_VC_OSD_ALPHA_T font_alpha,
									E_VC_OSD_ALPHA_T back_alpha,
									U8_T font_color_idx, U8_T back_color_idx,
									E_VC_OSD_DIMENSTION_T d,
									const char *string)
{
	U32_T output_h_size;
	U8_T reg_0x03;
	U8_T drawer_id = cb->drawer_id;
	size_t num_char = strlen(string);

	if (num_char > RDC200_MAX_NUM_FONT_CHAR_PER_BOX) {
		EMSG("Must be less than %d characters\n", RDC200_MAX_NUM_FONT_CHAR_PER_BOX);
		return -1;
	}

	if (!rdc200_check_text_position(&output_h_size, pos_x, pos_y, mag, d)) {
		EMSG("Invalid text size to drawing\n");
		return -2;
	}

	if (font_color_idx > 15) {
		EMSG("Font color must be 0 to 15\n");
		return -3;
	}

	if (back_color_idx > 15) {
		EMSG("Background color must be 0 to 15\n");
		return -4;
	}

	if (d == VC_OSD_DIMENSTION_3D) {
		if (mag == VC_TEXT_OSD_MAG_x8) {
			EMSG("Invaild magnification to draw 3D text\n");
			return -5;
		}
	}

	reg_0x03 = RDC_REG_GET(0x03);
	RDC_REG_SET(0x03, 0x9D);

	rdc200_config_text_drawer(drawer_id, output_h_size,
							pos_x, pos_y, font_addr,
							palette_addr, mag, font_alpha, back_alpha,
							font_color_idx, back_color_idx, d);

	rdc200_set_text_string(string, num_char);

	RDC_REG_MASK_SET(0x0B, ((1<<drawer_id) << 4), ((1<<drawer_id) << 4));
	if (d == VC_OSD_DIMENSTION_3D) /* Draw 3D text if enabled. */
		RDC_REG_MASK_SET(0x11, ((1<<drawer_id) << 4), ((1<<drawer_id) << 4));

	RDC_REG_SET(0x03, reg_0x03);

	return 0;
}

static int rdc200_disable_bitmap_osd(VC_BITMAP_DRAWER_CB *cb)
{
	U8_T bmpdr_en;
	U8_T drawer_id = cb->drawer_id;

	bmpdr_en = 1 << drawer_id;

	/* Disable drawer include 3D. Hide. */
	RDC_REG_MASK_SET(0x08, (bmpdr_en << 4), 0);
	RDC_REG_MASK_SET(0x30+drawer_id, 0x80, 0); // Clear bmpdr_3d_en

	return 0;
}

static int rdc200_change_dimension_bitmap_osd(VC_BITMAP_DRAWER_CB *cb,
												E_VC_OSD_DIMENSTION_T d)
{
	U8_T bmp_pos_x, bmp_pos_y;
	U32_T output_h_size;

	if (d == VC_OSD_DIMENSTION_3D) {
		if (!rdc200_check_bitmap_position(&output_h_size, cb->pos_x, cb->pos_y, cb->mag, d)) {
			EMSG("Invalid bitmap size to drawing\n");
			return -1;
		}

		bmp_pos_x = cb->pos_x >> 3; // Divide by 8 (8 pixel unit)
		bmp_pos_y = cb->pos_y >> 3; // Divide by 8 (8 pixel unit)

		output_h_size >>= 1; // Divide by 2 (Side By Side)
		bmp_pos_x += (output_h_size >> 3); // Divide by 8 (8 pixel unit)

		// Select drawer (cfg for 3D)
		RDC_REG_MASK_SET(0x08, 0x0F, (cb->drawer_id+4));

		RDC_REG_SET(0x09, bmp_pos_x);
		RDC_REG_SET(0x0A, bmp_pos_y);

		RDC_REG_MASK_SET(0x30+cb->drawer_id, 0x80, 0x80); // bmpdr_3d_en
	}
	else
		RDC_REG_MASK_SET(0x30+cb->drawer_id, 0x80, 0x0);

	return 0;
}

static int rdc200_change_property_bitmap_osd(VC_BITMAP_DRAWER_CB *cb,
												E_VC_OSD_BITMAP_MAG_T mag,
												E_VC_OSD_ALPHA_T alpha)
{
	U32_T output_h_size;

	if (!rdc200_check_bitmap_position(&output_h_size, cb->pos_x, cb->pos_x, mag, cb->d)) {
		EMSG("Invalid bitmap size to drawing\n");
		return -1;
	}

	RDC_REG_MASK_SET(0x30+cb->drawer_id, (0x07 << 4)|0x0F, (mag << 4)|alpha);

	return 0;
}

static int rdc200_hide_bitmap_osd(VC_BITMAP_DRAWER_CB *cb)
{
	RDC_REG_MASK_SET(0x30+cb->drawer_id, 0x0F, 0x0); // alpha 0%
	return 0;
}

static int rdc200_show_bitmap_osd(VC_BITMAP_DRAWER_CB *cb)
{
	RDC_REG_MASK_SET(0x30+cb->drawer_id, 0x0F, cb->alpha);
	return 0;
}

static int rdc200_move_bitmap_osd(VC_BITMAP_DRAWER_CB *cb, U32_T mov_x, U32_T mov_y)
{
	U8_T bmpdr_en, bmp_pos_cfg, bmp_pos_x, bmp_pos_y;
	U32_T output_h_size;
	U8_T drawer_id = cb->drawer_id;

	if (!rdc200_check_bitmap_position(&output_h_size, mov_x, mov_y, cb->mag, cb->d)) {
		EMSG("Invalid bitmap size to moving\n");
		return -1;
	}

	bmp_pos_x = mov_x >> 3; // Divide by 8 (8 pixel unit)
	bmp_pos_y = mov_y >> 3; // Divide by 8 (8 pixel unit)

	bmp_pos_cfg = 1 << drawer_id;
	bmpdr_en = 1 << drawer_id;

	RDC_REG_MASK_SET(0x30+cb->drawer_id, 0x0F, 0x0); // alpha 0%

	/* Frist, draw 2D bitmap. */
	// Select drawer (cfg for 2D)
	RDC_REG_MASK_SET(0x08, 0x0F, drawer_id);
	
	RDC_REG_SET(0x09, bmp_pos_x);
	RDC_REG_SET(0x0A, bmp_pos_y);

	/* Draw 3D bitmap if enabled. */
	if (cb->d == VC_OSD_DIMENSTION_3D) {
		// Select drawer (cfg for 3D)
		RDC_REG_MASK_SET(0x08, 0x0F, (drawer_id+4));

		output_h_size >>= 1; // Divide by 2 (Side By Side)
		bmp_pos_x += (output_h_size >> 3); // Divide by 8 (8 pixel unit)

		RDC_REG_SET(0x09, bmp_pos_x);
		RDC_REG_SET(0x0A, bmp_pos_y);
	}

	RDC_REG_MASK_SET(0x30+cb->drawer_id, 0x0F, cb->alpha);

	return 0;
}

static int rdc200_draw_bitmap_osd(VC_BITMAP_DRAWER_CB *cb, U32_T pos_x, U32_T pos_y,
									U32_T bmp_addr, E_VC_OSD_BITMAP_MAG_T mag,
									E_VC_OSD_ALPHA_T alpha,
									E_VC_OSD_DIMENSTION_T d)
{
	U8_T bmpdr_en, bmp_pos_cfg, bmp_pos_x, bmp_pos_y;
	U32_T output_h_size;
	U8_T drawer_id = cb->drawer_id;

	if (!rdc200_check_bitmap_position(&output_h_size, pos_x, pos_y, mag, d)) {
		EMSG("Invalid bitmap size to drawing\n");
		return -1;
	}

	bmp_pos_cfg = 1 << drawer_id;
	bmpdr_en = 1 << drawer_id;

	/* Disable drawer include 3D. */
	RDC_REG_MASK_SET(0x08, (bmpdr_en << 4), 0);
	RDC_REG_MASK_SET(0x30+drawer_id, 0x80, 0); // Clear bmpdr_3d_en

	rdc200_update_lut(bmp_addr, FLASH_ITEM_ACTUAL_SIZE_BMP,
						rdc200_bmp_drawer_sram_id[drawer_id]);

	bmp_pos_x = pos_x >> 3; // Divide by 8 (8 pixel unit)
	bmp_pos_y = pos_y >> 3; // Divide by 8 (8 pixel unit)

	/* Frist, draw 2D bitmap. */
	// Select drawer (cfg for 2D)
	RDC_REG_MASK_SET(0x08, 0x0F, drawer_id);	
	RDC_REG_SET(0x09, bmp_pos_x);
	RDC_REG_SET(0x0A, bmp_pos_y);

	// mag and alpha
	RDC_REG_MASK_SET(0x30+drawer_id, (0x07 << 4)|0x0F, (mag << 4)|alpha);
	
	// Enable 2D
	RDC_REG_MASK_SET(0x08, (bmpdr_en << 4), (bmpdr_en << 4));

	/* Draw 3D bitmap if enabled. */
	if (d == VC_OSD_DIMENSTION_3D) {
		// Select drawer (cfg for 3D)
		RDC_REG_MASK_SET(0x08, 0x0F, (drawer_id+4));

		output_h_size >>= 1; // Divide by 2 (Side By Side)
		bmp_pos_x += (output_h_size >> 3); // Divide by 8 (8 pixel unit)

		RDC_REG_SET(0x09, bmp_pos_x);
		RDC_REG_SET(0x0A, bmp_pos_y);

		RDC_REG_MASK_SET(0x30+drawer_id, 0x80, 0x80); // bmpdr_3d_en
	}

	return 0;
}

static int rdc200_confirm_led_control(E_VC_RDC_INDEX_T rdc_idx)
{
	U8_T retry_cnt = 200;
	U8_T reg_0x75;
	int err_val;
	int ret = 0;

	// clear FW interrupt
	RDC_REG_SET(0xC2, led_ctrl_intr_flag[rdc_idx] & 0xF7);

	do {
		reg_0x75 = RDC_REG_GET(0x75);
		if (reg_0x75 & LED_DONE_FLAG_MASK) {
			RDC_REG_SET(0x75, 0x00); /* Clear done_flag */

			if ((reg_0x75 & LED_ERR_FLAG_MASK) == 0)
				return 0;
			else {
				err_val = RDC_REG_GET(0x74);
				EMSG("Request led error (%d)\n", -err_val);
				ret = -err_val;
				return ret;
			}
		}

		VC_M_DELAY(1);
	} while (--retry_cnt);

	EMSG("Request led timeout\n");
	return -99;
}

static int rdc200_reqeust_led_color_mode(E_VC_RDC_INDEX_T rdc_idx,
							E_VC_PANEL_CTRL_PORT_T ctrl_port,
							VC_LED_CTRL_INFO_T *led_ctrl_info)
{
	U8_T reg_0x6C = 0x00;
	int ret = 0;
	E_VC_LED_CTRL_COLOR_T ctrl_color = led_ctrl_info->param.color.ctrl;

	reg_0x6C = 0/*func*/ | ((U8_T)ctrl_color<<4) | ((U8_T)ctrl_port<<0);	
	RDC_REG_SET(0x6C, reg_0x6C);

	if (ctrl_port & VC_PANEL_CTRL_PORT_0) {
		if (ctrl_color & VC_LED_CTRL_COLOR_R)
			RDC_REG_SET(0x5E, led_ctrl_info->param.color.R);
		
		if (ctrl_color & VC_LED_CTRL_COLOR_G)
			RDC_REG_SET(0x5F, led_ctrl_info->param.color.G);
		
		if (ctrl_color & VC_LED_CTRL_COLOR_B)
			RDC_REG_SET(0x60, led_ctrl_info->param.color.B);
	}

	if (ctrl_port & VC_PANEL_CTRL_PORT_1) {
		if (ctrl_color & VC_LED_CTRL_COLOR_R)
			RDC_REG_SET(0x61, led_ctrl_info->param.color.R);
		
		if (ctrl_color & VC_LED_CTRL_COLOR_G)
			RDC_REG_SET(0x62, led_ctrl_info->param.color.G);
		
		if (ctrl_color & VC_LED_CTRL_COLOR_B)
			RDC_REG_SET(0x63, led_ctrl_info->param.color.B);
	}

	led_ctrl_intr_flag[rdc_idx] = RDC_REG_GET(0xC2);

	// set FW interrupt 
	RDC_REG_SET(0xC2, led_ctrl_intr_flag[rdc_idx] | 0x08);

	return ret;
}

static int rdc200_reqeust_led_brightness_mode(E_VC_RDC_INDEX_T rdc_idx,
											E_VC_PANEL_CTRL_PORT_T ctrl_port,
											U8_T brightness)
{
	U8_T reg_0x6C = 0x00;
	int ret = 0;

	reg_0x6C = 0/*func*/ | LED_CTRL_MASK_BRIGHTNESS | ((U8_T)ctrl_port<<0);	
	RDC_REG_SET(0x6C, reg_0x6C);

	if (ctrl_port & VC_PANEL_CTRL_PORT_0)
		RDC_REG_SET(0x64, brightness);

	if (ctrl_port & VC_PANEL_CTRL_PORT_1)
		RDC_REG_SET(0x65, brightness);

	led_ctrl_intr_flag[rdc_idx] = RDC_REG_GET(0xC2);

	// set FW interrupt 
	RDC_REG_SET(0xC2, led_ctrl_intr_flag[rdc_idx] | 0x08);

	return ret;
}

static int rdc200_shift_vertical_pixel(E_VC_PANEL_CTRL_PORT_T ctrl_port,
								E_VC_SHIFT_VERTICAL_T v_direction)
{
	U16_T shift_val;
	U8_T msb, lsb;

	msb = RDC_REG_GET(0x45) & 0x7F;
	lsb = RDC_REG_GET(0x46) & 0xE0;
	shift_val = ((U16_T)msb << 3) | (lsb >> 5);

//	DMSG("V_shift_val(0x%02X)\n", shift_val);

	if (v_direction == VC_SHIFT_VERTICAL_DOWN) {
		if (shift_val > 0)
			shift_val--;
		else {
			EMSG("Bottom! Can't move to the down side anymore\n");
			return -1;
		}
	}
	else {
		if (shift_val < 0x1FF)
			shift_val++;
		else {
			EMSG("Top! Can't move to the up side anymore\n");
			return -2;
		}
	}

	msb = (shift_val >> 3) & 0x7F;
	lsb = (shift_val & 0x07) << 5;

	RDC_REG_MASK_SET(0x45, 0x7F, msb);
	RDC_REG_MASK_SET(0x46, 0xE0, lsb);

	return 0;
}

static int rdc200_shift_horizontal_pixel(E_VC_PANEL_CTRL_PORT_T ctrl_port,
									E_VC_SHIFT_HORIZONTAL_T h_direction)
{
	U16_T shift_val;
	U8_T msb, lsb;

	shift_val = RDC_REG_GET(0x44);
	shift_val = (shift_val << 1) | ((RDC_REG_GET(0x45) & 0x80) >> 7);
	DMSG("H_shift_val(0x%03X)\n", shift_val);

	if (h_direction == VC_SHIFT_HORIZONTAL_LEFT) {
		if (shift_val > 0)
			shift_val--;
		else {
			EMSG("Leftmost! Can't move to the left side anymore\n");
			return -1;
		}
	}
	else {
		if (shift_val < 0x1FF)
			shift_val++;
		else {
			EMSG("Rightmost! Can't move to the right side anymore\n");
			return -2;
		}
	}

	msb = (shift_val >> 1) & 0xFF;
	lsb = (shift_val & 0x01) << 7;

	RDC_REG_SET(0x44, msb);
	RDC_REG_MASK_SET(0x45, 0x80, lsb);

	return 0;
}

static U8_T mrx_config;
static U8_T pgen_config;

static int rdc200_enable_test_pattern(BOOL_T en)
{
	if (en == TRUE) {
		RDC_REG_MASK_SET(0x2A, 0x07, 0x04);
		RDC_REG_MASK_SET(0x37, 0x0F, 0x02);
	}
	else {
		RDC_REG_SET(0x2A, mrx_config);
		RDC_REG_SET(0x37, pgen_config);
	}

	return 0;
}

static int rdc200_generate_test_pattern(U8_T r_val, U8_T g_val, U8_T b_val)
{
	RDC_REG_SET(0x34, r_val);
	RDC_REG_SET(0x35, g_val);
	RDC_REG_SET(0x36, b_val);

	return 0;
}

static int rdc200_set_image_display_format(E_VC_IMAGE_DISPLAY_FORMAT_T fmt,
										BOOL_T swap_image_3d_mode)
{
	U8_T output_selection;

	rdc200_drive_panel(FALSE);

	output_selection = RDC_REG_GET(0x04);

	switch ((int)fmt) {
	case VC_IMAGE_DISPLAY_FORMAT_2D:
		output_selection &= ~0x04;
		break;

	case VC_IMAGE_DISPLAY_FORMAT_3D_SBS:
		output_selection |= 0x04;
		break;
	}

	RDC_REG_SET(0x04, output_selection);

	rdc200_drive_panel(TRUE);

	return 0;
}

///////////////////////////////////////
static void rdc200_config_frame_led(U8_T frame_hz, U8_T led_duty, U32_T lvds_rate_bps)
{
	U16_T duty_line;
	float _1_line_time;
	U16_T hold_line;
	U16_T led_off_time, led_on_time;

	if (lvds_rate_bps < 8) {
		EMSG("Invalid frame rate\n");
		return;
	}

	_1_line_time
		= (float)NUM_H_LINE_PIXEL/(float)((NUM_LVDS_CHANNEL+2) * (lvds_rate_bps>>3));

	hold_line
		= (U16_T)(1. / (3. * _1_line_time * (float)frame_hz)) - NUM_V_LINE_PIXEL;
	
	duty_line
		= (U16_T)((float)(NUM_V_LINE_PIXEL+hold_line) * (float)led_duty/100.);

	led_off_time = (NUM_V_LINE_PIXEL*2) + hold_line;
	led_on_time = led_off_time - duty_line;

	hold_line >>= 3; // unit : 8 pixels
	led_on_time >>= 1; // unit: 2 line
	led_off_time >>= 1; // unit: 2 line

	RDC_REG_SET(0x53, (U8_T)hold_line);
	RDC_REG_SET(0x54, (U8_T)hold_line);
	RDC_REG_SET(0x55, (U8_T)hold_line);

	// led_on_r<11:4>
	RDC_REG_SET(0x48, led_on_time>>4);

	// led_on_r<3:0> | led_off_r<11:8>
	RDC_REG_SET(0x49, ((led_on_time&0xF)<<4)|(led_off_time>>8)); 

	// led_off_r<7:0>
	RDC_REG_SET(0x4A, led_off_time & 0xFF);

	// led_on_g<11:4>
	RDC_REG_SET(0x4B, led_on_time>>4);

	// led_on_g<3:0> | led_off_g<11:8>
	RDC_REG_SET(0x4C, ((led_on_time&0xF)<<4)|(led_off_time>>8)); 

	// led_off_g<7:0>
	RDC_REG_SET(0x4D, led_off_time & 0xFF);

	// led_on_b<11:4>
	RDC_REG_SET(0x4E, led_on_time>>4);

	// led_on_b<3:0> | led_off_b<11:8>
	RDC_REG_SET(0x4F, ((led_on_time&0xF)<<4)|(led_off_time>>8)); 

	// led_off_b<7:0>
	RDC_REG_SET(0x50, led_off_time & 0xFF);
}

#define VC_LED_DUTY_TO_DOUBLE(duty)\
	((double)(duty) / 100./ (double)VC_LED_DUTY_DIVIDER)


static int rdc200_set_led_duty(E_VC_PANEL_CTRL_PORT_T ctrl_port,
							E_VC_LED_CTRL_COLOR_T color,
							VC_LED_DUTY_INFO_T new_duty[MAX_NUM_VC_PANEL_PORT])
{
	U32_T output_h_size, output_v_size;
	S32_T R_tuning = 0, G_tuning = 0, B_tuning = 0;
	U16_T holding_time, led_off_time;
	double percentage;

	rdc200_get_output_resolution(&output_h_size, &output_v_size);

	/*
	 * First: Calculate the value of register to write at the same time.
	 *        And calculate the returning value.
	 */
	if (color & VC_LED_CTRL_COLOR_R) {
		percentage = VC_LED_DUTY_TO_DOUBLE(new_duty->R);
		if (percentage <= 0.0) {
			EMSG("R: Can't step to the down side anymore\n");
			return -1;
		}

		if (percentage > 1.0) {
			EMSG("R: Can't step to the up side anymore\n");
			return -2;
		}

		holding_time = (U16_T)RDC_REG_GET(0x53) << 3; /* 8 line unit */
		led_off_time = ((U16_T)(RDC_REG_GET(0x49)&0x0F)<<8) | RDC_REG_GET(0x4A);
		led_off_time <<= 1; /* unit = 2 */

		R_tuning = (S32_T)((led_off_time - (output_v_size + holding_time) * percentage) /(double)2.0 + 0.5);
		if (R_tuning < 0) {
			EMSG("R: MIN register value is out of range\n");
			return -3;
		}

		if (R_tuning > 0xFFF) {
			EMSG("R: MAX register value is out of range\n");
			return -4;
		}

		new_duty[0].R = (S32_T)((double)(led_off_time - (R_tuning<<1))
					/ (double)(output_v_size + holding_time)
					* (100*VC_LED_DUTY_DIVIDER));
	}

	if (color & VC_LED_CTRL_COLOR_G) {
		percentage = VC_LED_DUTY_TO_DOUBLE(new_duty->G);
		if (percentage <= 0.0) {
			EMSG("G: Can't step to the down side anymore\n");
			return -5;
		}

		if (percentage > 1.0) {
			EMSG("G: Can't step to the up side anymore\n");
			return -6;
		}

		holding_time = (U16_T)RDC_REG_GET(0x54) << 3; /* 8 line unit */
		led_off_time = ((U16_T)(RDC_REG_GET(0x4C)&0x0F)<<8) | RDC_REG_GET(0x4D);
		led_off_time <<= 1; /* unit = 2 */

		G_tuning = (S32_T)((led_off_time - (output_v_size + holding_time) * percentage) /(double)2.0 + 0.5);
		if (G_tuning < 0) {
			EMSG("G: MIN register value is out of range\n");
			return -7;
		}

		if (G_tuning > 0xFFF) {
			EMSG("G: MAX register value is out of range\n");
			return -8;
		}

		new_duty[0].G = (S32_T)((double)(led_off_time - (G_tuning<<1))
					/ (double)(output_v_size + holding_time)
					* (100*VC_LED_DUTY_DIVIDER));
	}

	if (color & VC_LED_CTRL_COLOR_B) {
		percentage = VC_LED_DUTY_TO_DOUBLE(new_duty->B);
		if (percentage <= 0.0) {
			EMSG("G: Can't step to the down side anymore\n");
			return -9;
		}

		if (percentage > 1.0) {
			EMSG("G: Can't step to the up side anymore\n");
			return -10;
		}

		holding_time = (U16_T)RDC_REG_GET(0x55) << 3; /* 8 line unit */
		led_off_time = ((U16_T)(RDC_REG_GET(0x4F)&0x0F)<<8) | RDC_REG_GET(0x50);
		led_off_time <<= 1; /* unit = 2 */

		B_tuning = (S32_T)((led_off_time - (output_v_size + holding_time) * percentage) /(double)2.0 + 0.5);
		if (B_tuning < 0) {
			EMSG("B: MIN register value is out of range\n");
			return -11;
		}

		if (B_tuning > 0xFFF) {
			EMSG("B: MAX register value is out of range\n");
			return -12;
		}

		new_duty[0].B = (S32_T)((double)(led_off_time - (B_tuning<<1))
					/ (double)(output_v_size + holding_time)
					* (100*VC_LED_DUTY_DIVIDER));
	}

	new_duty[1] = new_duty[0];

	/*
	 * Second: Write the calculated value into register.
	 */
	if (color & VC_LED_CTRL_COLOR_R) {
		RDC_REG_SET(0x48, R_tuning>>4); /* led_on_r */
		RDC_REG_MASK_SET(0x49, 0xF0, (R_tuning&0x0F)<<4);
	}

	if (color & VC_LED_CTRL_COLOR_G) {
		RDC_REG_SET(0x4B, G_tuning>>4); /* led_on_g */
		RDC_REG_MASK_SET(0x4C, 0xF0, (G_tuning&0x0F)<<4);
	}

	if (color & VC_LED_CTRL_COLOR_B) {
		RDC_REG_SET(0x4E, B_tuning>>4); /* led_on_b */
		RDC_REG_MASK_SET(0x4F, 0xF0, (B_tuning&0x0F)<<4);
	}

	return 0;
}

static void rdc200_get_led_duty_from_reg(E_VC_LED_CTRL_COLOR_T color,
										VC_LED_DUTY_INFO_T *info)
{
	U32_T output_h_size, output_v_size;
	S32_T delta;
	U16_T r_holditme, g_holditme, b_holditme;
	U16_T led_on_r, led_off_r;
	U16_T led_on_g, led_off_g;
	U16_T led_on_b, led_off_b;
	U8_T reg_0x49, reg_0x4C, reg_0x4F;
	double duty_percentage;

	rdc200_get_output_resolution(&output_h_size, &output_v_size);

	if (color & VC_LED_CTRL_COLOR_R) {
		reg_0x49 = RDC_REG_GET(0x49);
		led_on_r = ((U16_T)RDC_REG_GET(0x48) << 4) | (reg_0x49 >> 4);
		led_off_r = ((U16_T)(reg_0x49&0x0F)<<8) | RDC_REG_GET(0x4A);
		r_holditme = (U16_T)RDC_REG_GET(0x53) << 3; /* 8 line unit */

		delta = (led_off_r - led_on_r) << 1; /* 2 line unit */

		duty_percentage = (delta/(double)(output_v_size + r_holditme)) * 100.;
		info->R = (S32_T)(duty_percentage * VC_LED_DUTY_DIVIDER + 0.5);
	}

	if (color & VC_LED_CTRL_COLOR_G) {
		reg_0x4C = RDC_REG_GET(0x4C);
		led_on_g = ((U16_T)RDC_REG_GET(0x4B) << 4) | (reg_0x4C >> 4);
		led_off_g = ((U16_T)(reg_0x4C&0x0F)<<8) | RDC_REG_GET(0x4D);
		g_holditme = (U16_T)RDC_REG_GET(0x54) << 3; /* 8 line unit */

		delta = (led_off_g - led_on_g) << 1; /* 2 line unit */

		duty_percentage = (delta/(double)(output_v_size + g_holditme)) * 100.;
		info->G = (S32_T)(duty_percentage * VC_LED_DUTY_DIVIDER + 0.5);
	}	

	if (color & VC_LED_CTRL_COLOR_B) {
		reg_0x4F = RDC_REG_GET(0x4F);
		led_on_b = ((U16_T)RDC_REG_GET(0x4E) << 4) | (reg_0x4F >> 4);
		led_off_b = ((U16_T)(reg_0x4F&0x0F)<<8) | RDC_REG_GET(0x50);
		b_holditme = (U16_T)RDC_REG_GET(0x55) << 3; /* 8 line unit */

		delta = (led_off_b - led_on_b) << 1; /* 2 line unit */

		duty_percentage = (delta/(double)(output_v_size + b_holditme)) * 100.;
		info->B = (S32_T)(duty_percentage * VC_LED_DUTY_DIVIDER + 0.5);
	}
}

static S32_T rdc200_get_led_duty(struct _VC_RDC_DEV_INFO_T *dev,
								E_VC_PANEL_CTRL_PORT_T ctrl_port,
								VC_LED_DUTY_INFO_T info[MAX_NUM_VC_PANEL_PORT])
{
	if (ctrl_port & VC_PANEL_CTRL_PORT_0) {
		if (dev->led_duty_info[0].R == -1)
			rdc200_get_led_duty_from_reg(VC_LED_CTRL_COLOR_ALL, &dev->led_duty_info[0]);
	}

	if (ctrl_port & VC_PANEL_CTRL_PORT_1)
		if (dev->led_duty_info[1].R == -1)
			rdc200_get_led_duty_from_reg(VC_LED_CTRL_COLOR_ALL, &dev->led_duty_info[1]);	

	info[0] = dev->led_duty_info[0];
	info[1] = dev->led_duty_info[1];

	return 0;
}

static void rdc200_fill_step_value(VC_LED_DUTY_INFO_T *new_duty,
									E_VC_LED_CTRL_COLOR_T color, BOOL_T up,
									VC_LED_DUTY_INFO_T *dev_duty)
{
	if (color & VC_LED_CTRL_COLOR_R) {
		if (up == TRUE)
			new_duty->R = RDC200_STEP_UP_LED_DUTY_VALUE(dev_duty->R);
		else
			new_duty->R = RDC200_STEP_DOWN_LED_DUTY_VALUE(dev_duty->R);
	}

	if (color & VC_LED_CTRL_COLOR_G) {
		if (up == TRUE)
			new_duty->G = RDC200_STEP_UP_LED_DUTY_VALUE(dev_duty->G);
		else
			new_duty->G = RDC200_STEP_DOWN_LED_DUTY_VALUE(dev_duty->G);
	}

	if (color & VC_LED_CTRL_COLOR_B) {
		if (up == TRUE)
			new_duty->B = RDC200_STEP_UP_LED_DUTY_VALUE(dev_duty->B);
		else
			new_duty->B = RDC200_STEP_DOWN_LED_DUTY_VALUE(dev_duty->B);
	}
}

static S32_T rdc200_set_frame_rate(VC_RDC_DEV_INFO_T *dev, S32_T new_rate)
{
	U16_T pll2_refdiv;
	U32_T output_h_size, output_v_size;
	U32_T r_holding_time, g_holding_time, b_holding_time;
	double frame_rate_hz, new_frame_rate_hz;
	int x_clock;
	int lvdx_line;
	U32_T bB4_bA;
	S32_T ret;
	U32_T vertical_line_pixel, scan_time, scans_holds;

	if (new_rate <= 0) {
		EMSG("Can't step to the down side anymore\n");
		return -1;
	}
	new_frame_rate_hz = (double)new_rate / (double)VC_FRAME_RATE_DIVIDER;

	pll2_refdiv = RDC_REG_GET(0x28) & 0x03;
	x_clock = (RDC_XTAL_FREQ_MHz / (pll2_refdiv + 1));

	lvdx_line = rdc200_get_output_resolution(&output_h_size, &output_v_size);
	if (lvdx_line == -1) {
		if (output_v_size > 1000)
			lvdx_line = 8;
		else
			lvdx_line = 4;
	}
	vertical_line_pixel = output_h_size;
	scan_time = output_v_size;

	r_holding_time = (U32_T)RDC_REG_GET(0x53) << 3; /* 8 pixel unit */
	g_holding_time = (U32_T)RDC_REG_GET(0x54) << 3;
	b_holding_time = (U32_T)RDC_REG_GET(0x55) << 3;

	scans_holds = scan_time*3 + r_holding_time+g_holding_time+b_holding_time;

	bB4_bA = (U32_T)(4*(2 + (double)vertical_line_pixel/(double)lvdx_line)*new_frame_rate_hz * scans_holds * (4. /(double)(1000*1000*x_clock))+0.5);
	if (bB4_bA > 0x3FF) { /* pll2_plln<9:0> */
		EMSG("Can't step to the up side anymore\n");
		return -2;
	}

	RDC_REG_SET(0x27, bB4_bA>>2);
	RDC_REG_MASK_SET(0x28, 0xC0, (bB4_bA & 0x3)<<6);

	frame_rate_hz = 1.0/(double)(scans_holds*(4 * (2 + vertical_line_pixel/(double)lvdx_line)* 4 /(double)(bB4_bA*x_clock)/1000.0/1000.0));

	ret = (S32_T)(frame_rate_hz * VC_FRAME_RATE_DIVIDER);

	return ret;
}

static S32_T rdc200_get_frame_rate_from_reg(void)
{
	U16_T pll2plln;
	U16_T pll2_refdiv;
	U8_T reg_0x28;
	U32_T output_h_size, output_v_size, feedback_divider_ratio;
	double one_line_time;
	U16_T r_holditme, g_holditme, b_holditme;
	int vco_osc_freq, lvdx_line;
	double pll_out_freq;
	double lvds_tx_clock;
	double frame_rate_hz;
	int x_clock;
	U32_T vertical_line_pixel, scan_time;

	reg_0x28 = RDC_REG_GET(0x28);
	pll2plln = ((U16_T)RDC_REG_GET(0x27) << 2) | (reg_0x28 >> 6);
	feedback_divider_ratio = (pll2plln >> 2)*4 + (pll2plln & 0x3);

	pll2_refdiv = reg_0x28 & 0x03;
	x_clock = (RDC_XTAL_FREQ_MHz / (pll2_refdiv + 1));
						
	vco_osc_freq = feedback_divider_ratio * x_clock;
	pll_out_freq = vco_osc_freq / 2.0f;
	lvds_tx_clock = pll_out_freq / 2.0f;
	
	lvdx_line = rdc200_get_output_resolution(&output_h_size, &output_v_size);
	vertical_line_pixel = output_h_size;
	scan_time = output_v_size;

	one_line_time = 4. * (2. + (double)vertical_line_pixel/(double)lvdx_line) / (double)lvds_tx_clock / 1000.0/1000.0;

	r_holditme = (U16_T)RDC_REG_GET(0x53) << 3; /* 8 pixel unit */
	g_holditme = (U16_T)RDC_REG_GET(0x54) << 3;
	b_holditme = (U16_T)RDC_REG_GET(0x55) << 3;

	frame_rate_hz = 1.0/((double)(scan_time*3 + (r_holditme+g_holditme+b_holditme)) * one_line_time);

	return (S32_T)(frame_rate_hz * VC_FRAME_RATE_DIVIDER);
}

static S32_T rdc200_get_frame_rate(struct _VC_RDC_DEV_INFO_T *dev)
{	
	return dev->frame_rate;
}

static S32_T rdc200_step_frame_rate(struct _VC_RDC_DEV_INFO_T *dev, BOOL_T up)
{
	S32_T applied_frame_rate;
	S32_T new_frame_rate;

	if (up == TRUE)
		new_frame_rate = RDC200_STEP_UP_FRAME_RATE_VALUE(dev->frame_rate);
	else
		new_frame_rate = RDC200_STEP_DOWN_FRAME_RATE_VALUE(dev->frame_rate);

	applied_frame_rate = rdc200_set_frame_rate(dev, new_frame_rate);
	if (applied_frame_rate > 0)
		dev->frame_rate = applied_frame_rate; /* Update new rate */

	return applied_frame_rate;
}

static int rdc200_request_cokoa_generation(VC_COKOA_PARAM_T *p)
{
	U8_T intr_flag;
	U16_T k1, k2, interp;

	RDC_REG_SET(0x5E, p->lt_x & 0xFF);
	RDC_REG_SET(0x5F, ((p->lt_x>>8)&0x1) | ((p->lt_y & 0x7F)<<1));

	RDC_REG_SET(0x60, p->rt_x & 0xFF);
	RDC_REG_SET(0x61, ((p->rt_x>>8)&0x1) | ((p->rt_y & 0x7F)<<1));

	RDC_REG_SET(0x62, p->lb_x & 0xFF);
	RDC_REG_SET(0x63, ((p->lb_x>>8)&0x1) | ((p->lb_y & 0x7F)<<1));

	RDC_REG_SET(0x64, p->rb_x & 0xFF);
	RDC_REG_SET(0x65, ((p->rb_x>>8)&0x1) | ((p->rb_y & 0x7F)<<1));

	k1 = (U16_T)(p->k1 * 1000.);
	k2 = (U16_T)(p->k2 * 1000.);
	interp = (U16_T)(p->interp * 1000.);

	RDC_REG_SET(0x6A, k1 & 0xFF); /* case for Little endian */
	RDC_REG_SET(0x6B, k2 & 0xFF); /* case for Little endian */

	RDC_REG_SET(0x6D, (((interp>>8)&0x3)<<6)
					| (((k2>>8)&0x3)<<2)
					| (((k1>>8)&0x3)<<0)
				);

	RDC_REG_SET(0x72, interp & 0xFF);

	RDC_REG_SET(0x73, (((interp>>10)&0x1)<<7) | (p->chrm_r & 0x7F));
	RDC_REG_SET(0x74, p->chrm_g & 0x7F);
	RDC_REG_SET(0x75, p->chrm_b & 0x7F);

	RDC_REG_SET(0x6C, 0x00); // func = 0

	// set FW interrupt 
	intr_flag = RDC_REG_GET(0xC2);
	RDC_REG_SET(0xC2, intr_flag | 0x08);

	// clear FW interrupt
	RDC_REG_SET(0xC2, intr_flag & 0xF7);

	return 0;
}

static int rdc200_prepare_controller(VC_RDC_DEV_INFO_T *dev)
{
	dev->frame_rate = rdc200_get_frame_rate_from_reg();

    /* Save mrx_cfg and pgen_cfg used by enable/disable Test Pattern */
    mrx_config = RDC_REG_GET(0x2A);
    pgen_config = RDC_REG_GET(0x37);
    
	return 0;
}

static int rdc200_enable_distortion_correction(E_VC_PANEL_CTRL_PORT_T ctrl_port, BOOL_T dc_en,
						UINT_T lut_offset[MAX_NUM_VC_PANEL_PORT], BOOL_T ca_en)
{
	U8_T read_data;
	U32_T lut_addr;
	UINT_T lut_o;

	if (ctrl_port & VC_PANEL_CTRL_PORT_0)
		lut_o = lut_offset[VC_PANEL_PORT_0];
	else
		lut_o = lut_offset[VC_PANEL_PORT_1];

	rdc200_drive_panel(FALSE);

	read_data = RDC_REG_GET(0x07);
	RDC_REG_SET(0x07, read_data & 0xFD); // distc_disable

	if (dc_en == TRUE) {
		read_data = RDC_REG_GET(0x7F) & 0xCF;
		RDC_REG_SET(0x7F, read_data | (ca_en << 5) | (1 << 4));

		read_data = RDC_REG_GET(0x07);
		RDC_REG_SET(0x07, read_data & 0xFD); // distc_disable

		lut_addr = FLASH_ITEM_ADDR_DC_RED(lut_o);
		rdc200_update_lut(lut_addr, FLASH_ITEM_SIZE_DC, TSID_DC_RED);

		if (ca_en == TRUE) {
			lut_addr = FLASH_ITEM_ADDR_DC_GREEN(lut_o);
			rdc200_update_lut(lut_addr, FLASH_ITEM_SIZE_DC, TSID_DC_GREEN);

			lut_addr = FLASH_ITEM_ADDR_DC_BLUE(lut_o);
			rdc200_update_lut(lut_addr, FLASH_ITEM_SIZE_DC, TSID_DC_BLUE);
		}

		read_data = RDC_REG_GET(0x07);
		RDC_REG_SET(0x07, read_data | 0x02); // distc_enable
	}

	rdc200_drive_panel(TRUE);

	return 0;
}

static BOOL_T rdc200_check_pll_lock(U8_T pll_idx)
{
    U8_T reg_1A;
    U8_T lock_mask = (1<<pll_idx) << 2; // <0> : PLL0 lock, <1> : PLL1 lock, <2> : PLL2 lock
    U8_T lock_timeout = 100;

    do {
        reg_1A = RDC_REG_GET(0x1A); // monitoring_basic[4:2]
        if (reg_1A & lock_mask)
            return TRUE;
        
        VC_M_DELAY(1);
    } while (--lock_timeout);
    
    EMSG("PLL_(%d) Unlock :  timeout\n", pll_idx);
    
    return FALSE;
}

static int rdc200_change_input_display_interface(E_VC_INPUT_DISPLAY_IF_T dif,
												VC_INPUT_DISPLAY_IF_CFG *cfg)
{
	U8_T reg_0x20 = 0x0;
	U8_T lvds_rev_bus = 0; // Don't reverse both
	U8_T lvds_pclk = 0;
	U8_T lvds_port = 0;
	U8_T mrx = 0;
    
	switch (dif) {
	case VC_INPUT_DISPLAY_IF_LVDS_0:
	case VC_INPUT_DISPLAY_IF_LVDS_1:
	case VC_INPUT_DISPLAY_IF_LVDS_BOTH:
		mrx = 0;
        reg_0x20 = RDC_REG_GET(0x20) & 0x9F;
            
		if (dif == VC_INPUT_DISPLAY_IF_LVDS_0) {
			reg_0x20 |= 0x40; // power down LVDS port 1
			lvds_port = 0;
		}
		else if (dif == VC_INPUT_DISPLAY_IF_LVDS_1) {
			reg_0x20 |= 0x20; // power down LVDS port 0
			lvds_pclk = 1; // Use port 1 clock
			lvds_port = 1;
		}
		else {
			reg_0x20 |= 0x00; // LVDS port 0/1 power on
			if (cfg->lvds.dual_mode_type == VC_LVDS_DUAL_MODE_PORT0_EVEN)
				lvds_port = 2;
			else
				lvds_port = 3;
		}

		// config
		RDC_REG_SET(0x92, ((U8_T)(lvds_pclk&0x01) <<7)
					|((U8_T)(lvds_port&0x03) << 5)
					|((U8_T)(lvds_rev_bus&0x03) << 3)
					|(U8_T)(cfg->lvds.color_depth&0x01));		
		break;

	case VC_INPUT_DISPLAY_IF_MIPI:
		reg_0x20 = 0x60;
		mrx = 2;
		
        RDC_REG_SET(0x92,0x00); //Set input config to default
		// config
		// {mipi_dr, mipi_num_lane, color_coding}
		RDC_REG_SET(0x03, (U8_T)(cfg->mipi.dr<<5)
							|(cfg->mipi.num_lane<<3)|0x05);
		
		// mipi_auto_pd, mipi_cont_clk
		RDC_REG_MASK_SET(0xD6, 0xC,
						((cfg->mipi.auto_pd << 3)
						| (cfg->mipi.cont_clk << 2)));

		switch (cfg->mipi.dr) {
		case VC_MIPI_DR_500Mbps:
			// pll0_plln, pll0_postd, pll0_refdiv
			RDC_REG_SET(0x23, 0x0F);
			RDC_REG_SET(0x24, (0x0<<6)|(0x1<<2)|0x0);
			break;

		case VC_MIPI_DR_700Mbps:
		case VC_MIPI_DR_864Mbps:
			// pll0_plln, pll0_postd, pll0_refdiv
			RDC_REG_SET(0x23, 0x12);
			RDC_REG_SET(0x24, (0x0<<6)|(0x1<<2)|0x0);
			break;

		case VC_MIPI_DR_1000Mbps:
		case VC_MIPI_DR_1344Mbps:
			// pll0_plln, pll0_postd, pll0_refdiv
			RDC_REG_SET(0x23, 0x0A);
			RDC_REG_SET(0x24, (0x2<<6)|(0x2<<2)|0x0);
			break;

		case VC_MIPI_DR_1500Mbps:
			// pll0_plln, pll0_postd, pll0_refdiv
			RDC_REG_SET(0x23, 0x0B);
			RDC_REG_SET(0x24, (0x3<<6)|(0x2<<2)|0x0);
			break;
		}

		break;

	default:
		break;
	}
    
    //pll lock check
    rdc200_check_pll_lock(0);
	// mrx
	RDC_REG_MASK_SET(0x2A, 0x7, mrx);

	// power down
	RDC_REG_SET(0x20, reg_0x20);

	return 0;
}

static void rdc200_set_default(VC_RDC_DEV_INFO_T *dev)
{
	dev->led_duty = 15; /* Default. HMD system. Beam project: 95 */

	dev->min_led_duty = 5;
	dev->max_led_duty = 95;

	dev->sector_size_shift = 12;
	//DMSG("SECTOR SIZE: 0x%X\n", 1 << dev->sector_size_shift);
}

static int rdc200_get_fw_version(U8_T *major_ver, U8_T *minor_ver, U8_T *rel_ver)
{
	U8_T upper_reg, lower_reg;

	upper_reg = RDC_REG_GET(0xA3);
	lower_reg = RDC_REG_GET(0x29);
	*major_ver = (upper_reg&0xF0) | (lower_reg>>4);

	upper_reg = RDC_REG_GET(0xC8);
	lower_reg = RDC_REG_GET(0x7C);
	*minor_ver = (upper_reg&0xF0) | (lower_reg>>4);

	*rel_ver = 0;

	return 0;
}

/******************************************************************************
 * START.
 * 1. Reference codes for RDC200 power On/Off/Suspend/Resume.
 * 2. Reference codes for RDC200 MIPI configuration and monitoring.
 *****************************************************************************/
/* NOTE: The gpio numbers are pseudo number for use in the examples. */
#define GPIO_RDC200_LDO_1_2V	14
#define GPIO_RDC200_LDO_1_8V	18
#define GPIO_RDC200_LDO_3_3V	19
#define GPIO_RDC200_RESETB		20

/* NOTE: Dummy GPIO function to avoid compile-error. */
void gpio_set_value(unsigned int gpio, int value)
{
}

static int rdc200_power_on(void)
{
	gpio_set_value(GPIO_RDC200_LDO_1_2V, 1);
	gpio_set_value(GPIO_RDC200_LDO_1_8V, 1);
	gpio_set_value(GPIO_RDC200_LDO_3_3V, 1);
	VC_M_DELAY(1);

	gpio_set_value(GPIO_RDC200_RESETB, 1);
	VC_M_DELAY(500);

	return 0;
}

static int rdc200_power_off(void)
{
	rdc200_poweron_panel(FALSE);
    VC_M_DELAY(50);

	gpio_set_value(GPIO_RDC200_RESETB, 0);
	VC_M_DELAY(50);

	gpio_set_value(GPIO_RDC200_LDO_3_3V, 0);
	gpio_set_value(GPIO_RDC200_LDO_1_8V, 0);
	gpio_set_value(GPIO_RDC200_LDO_1_2V, 0);

	return 0;
}

static int rdc200_suspend(void)
{
	rdc200_poweron_panel(FALSE);
    VC_M_DELAY(50);
	
	gpio_set_value(GPIO_RDC200_RESETB, 0);
	VC_M_DELAY(10);

	return 0;
}

static int rdc200_resume(void)
{

	gpio_set_value(GPIO_RDC200_RESETB, 1);
	VC_M_DELAY(500);

	return 0;
}


/*
RDC200 MIPI CSR Configruation : I2C 8 Bytes Write command needed.
*/

typedef enum {
	RDC_MIPI_DSI_NUM_LANE_1 = 0,
	RDC_MIPI_DSI_NUM_LANE_2,
	RDC_MIPI_DSI_NUM_LANE_3,
	RDC_MIPI_DSI_NUM_LANE_4
} E_RDC_MIPI_DSI_NUM_LANE_T;

/*
Select DSI video mode that the host DPI module should generatepackets for.
	00000000-Non-Burst mode with Sync Pulses
	00000001 -Non-Burst mode with SyncEvents
	00000002 -Burst mode
*/
typedef enum {
	RDC_MIPI_DSI_VIDEO_MODE_NON_BURST_PULSE = 0,
	RDC_MIPI_DSI_VIDEO_MODE_NON_BURST_EVENT = 1,
	RDC_MIPI_DSI_VIDEO_MODE_BURST = 2
} E_RDC_MIPI_DSI_VIDEO_MODE_T;

#define CSR_DSI_PERIPH_CFG_NUM_LANES_ADDR		(0x000000 + 0x00)
#define CSR_DSI_PERIPH_CFG_DISABLE_EOTP_ADDR	(0x000000 + 0x24)
#define CSR_DSI_PERIPH_CFG_DPI_VIDEO_MOD_ADDR	(0x000200 + 0x10)

/* 
 * vc_rdc_i2c_slave_addr: 0x44 or 0x45
 */
int rdc200_config_input_mipi(E_RDC_MIPI_DSI_NUM_LANE_T num_lane,
							BOOL_T eotp_require,
							E_RDC_MIPI_DSI_VIDEO_MODE_T dsi_video_mode)
{
	U8_T buf[8];

	buf[0] = 0x84; /* ACCESS CODE */
	buf[1] = (CSR_DSI_PERIPH_CFG_NUM_LANES_ADDR >> 16) & 0xFF;
	buf[2] = (CSR_DSI_PERIPH_CFG_NUM_LANES_ADDR >> 8) & 0xFF;
	buf[3] = CSR_DSI_PERIPH_CFG_NUM_LANES_ADDR & 0xFF; /* CSR ADDR[7:0] */
	buf[4] = 0x00; /* CSR DATA[31:24] */
	buf[5] = 0x00; /* CSR DATA[23:16] */
	buf[6] = 0x00; /* CSR DATA[15:8] */
	buf[7] = num_lane & 0xFF; /* CSR DATA[7:0] */
	rtiVC_WriteBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0,
							vc_rdc_i2c_slave_addr, buf, 8);

	buf[0] = 0x84; /* ACCESS CODE */
	buf[1] = (CSR_DSI_PERIPH_CFG_DISABLE_EOTP_ADDR >> 16) & 0xFF;
	buf[2] = (CSR_DSI_PERIPH_CFG_DISABLE_EOTP_ADDR >> 8) & 0xFF;
	buf[3] = CSR_DSI_PERIPH_CFG_DISABLE_EOTP_ADDR & 0xFF; /* CSR ADDR[7:0] */
	buf[4] = 0x00; /* CSR DATA[31:24] */
	buf[5] = 0x00; /* CSR DATA[23:16] */
	buf[6] = 0x00; /* CSR DATA[15:8] */
	if (eotp_require == TRUE)
		buf[7] = 0; /* CSR DATA[7:0] */
	else
		buf[7] = 1; /* CSR DATA[7:0] */
	rtiVC_WriteBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0,
							vc_rdc_i2c_slave_addr, buf, 8);

	buf[0] = 0x84; /* ACCESS CODE */
	buf[1] = (CSR_DSI_PERIPH_CFG_DPI_VIDEO_MOD_ADDR >> 16) & 0xFF;
	buf[2] = (CSR_DSI_PERIPH_CFG_DPI_VIDEO_MOD_ADDR >> 8) & 0xFF;
	buf[3] = CSR_DSI_PERIPH_CFG_DPI_VIDEO_MOD_ADDR & 0xFF; /* CSR ADDR[7:0] */
	buf[4] = 0x00; /* CSR DATA[31:24] */
	buf[5] = 0x00; /* CSR DATA[23:16] */
	buf[6] = 0x00; /* CSR DATA[15:8] */
	buf[7] = dsi_video_mode & 0xFF; /* CSR DATA[7:0] */
	rtiVC_WriteBurstDeviceExt(VC_DEVICE_CH_RDC_RDP0,
							vc_rdc_i2c_slave_addr, buf, 8);

	return 0;
}

/*
RDC200 INPUT VCD Monitoring
*/

typedef struct {
	unsigned short hsw;
	unsigned short hbp;
	unsigned short hfp;
	unsigned short vsw;
	unsigned short vbp;
	unsigned short vfp;
	unsigned short ha;
	unsigned short va;
} RDC_VIDEO_COORD_INFO_T;

typedef enum {
	RDC_VIDEO_COORD_INPUT_LVDS = 0,
	RDC_VIDEO_COORD_INPUT_MIPI = 2
} E_RDC_VIDEO_COORD_INPUT_T;

/* Get the informations of video coordination. */
void rdc200_get_video_coord_info(RDC_VIDEO_COORD_INFO_T *vcd,
									E_RDC_VIDEO_COORD_INPUT_T type)
{
	unsigned char reg_0x8D, reg_0x8E, reg_0x8F, reg_0x90;

	RDC_REG_MASK_SET(0x2C, 0x70, (unsigned char)(type&0x7)<<4);

	vcd->hsw = RDC_REG_GET(0x8A);
	vcd->hbp = RDC_REG_GET(0x8B);
	vcd->hfp = RDC_REG_GET(0x8C);

	reg_0x8D = RDC_REG_GET(0x8D);
	reg_0x8E = RDC_REG_GET(0x8E);
	reg_0x8F = RDC_REG_GET(0x8F);
	reg_0x90 = RDC_REG_GET(0x90);

	vcd->vsw = reg_0x8D >> 3;
	vcd->vbp = ((unsigned short)(reg_0x8D & 0x7) << 3) | (reg_0x8E >> 5);
	vcd->vfp = reg_0x8E & 0x1F;
	vcd->ha = ((unsigned short)reg_0x8F << 4) | (reg_0x90 >> 4);
	vcd->va = ((unsigned short)(reg_0x90&0x7) << 8) | RDC_REG_GET(0x91);
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

/******************************************************************************
 * END.
 * Reference codes for RDC200 power On/Off/Suspend/Resume.
 *****************************************************************************/

int rdc200_get_device_id(U16_T *chip_id, U16_T *rev_num)
{
	U8_T reg_val;

	reg_val = RDC_REG_GET(0x19);

	*chip_id = reg_val;
	*rev_num = 0;

	return 0;
}

int RDC200_Probe(VC_RDC_DEV_INFO_T *dev)
{
	rdc200_set_default(dev);

#if 0
{
	float _1_line_time, result;

	result = ((float)NUM_H_LINE_PIXEL/(float)(NUM_LVDS_CHANNEL+2)) / (float)10000;
	printf("%.5f\n", result);

	result = (float)NUM_H_LINE_PIXEL/((float)(NUM_LVDS_CHANNEL+2) * (float)10000);
	printf("%.5f\n", result);
}
#endif

	dev->power_on = rdc200_power_on;
	dev->power_off = rdc200_power_off;
	dev->suspend = rdc200_suspend;
	dev->resume = rdc200_resume;

	dev->get_device_id = rdc200_get_device_id;

	dev->get_fw_version = rdc200_get_fw_version;
	dev->prepare_controller = rdc200_prepare_controller;

	dev->flash_prepare = rdc200_flash_prepare;
	dev->flash_finish = rdc200_flash_finish;

	dev->flash_erase_sector = rdc200_flash_erase_sector;
	dev->flash_erase_chip = rdc200_flash_erase_chip;
	dev->flash_write = rdc200_flash_program;
	dev->flash_read = rdc200_flash_read;

	dev->enable_test_pattern = rdc200_enable_test_pattern;
	dev->generate_test_pattern = rdc200_generate_test_pattern;

	dev->shift_horizontal_pixel = rdc200_shift_horizontal_pixel;
	dev->shift_vertical_pixel = rdc200_shift_vertical_pixel;

	dev->get_led_duty = rdc200_get_led_duty;
	dev->set_led_duty = rdc200_set_led_duty;
	dev->fill_step_value = rdc200_fill_step_value;

	dev->get_frame_rate = rdc200_get_frame_rate;
	dev->step_frame_rate = rdc200_step_frame_rate;

	dev->set_image_display_format = rdc200_set_image_display_format;
	dev->get_output_resolution = rdc200_get_output_resolution;

	dev->poweron_panel = rdc200_poweron_panel;

	dev->request_cokoa_generation = rdc200_request_cokoa_generation;

	dev->update_lut = rdc200_update_lut;
	dev->enable_distortion_correction = rdc200_enable_distortion_correction;

	dev->draw_bitmap_osd = rdc200_draw_bitmap_osd;
	dev->move_bitmap_osd = rdc200_move_bitmap_osd;
	dev->show_bitmap_osd = rdc200_show_bitmap_osd;
	dev->hide_bitmap_osd = rdc200_hide_bitmap_osd;
	dev->change_property_bitmap_osd = rdc200_change_property_bitmap_osd;
	dev->change_dimension_bitmap_osd = rdc200_change_dimension_bitmap_osd;
	dev->disable_bitmap_osd = rdc200_disable_bitmap_osd;

	dev->draw_text_osd = rdc200_draw_text_osd;
	dev->move_text_osd = rdc200_move_text_osd;
	dev->show_text_osd = rdc200_show_text_osd;
	dev->hide_text_osd = rdc200_hide_text_osd;
	dev->change_property_text_osd = rdc200_change_property_text_osd;
	dev->change_dimension_text_osd = rdc200_change_dimension_text_osd;
	dev->disable_text_osd = rdc200_disable_text_osd;

    dev->enable_gamma_correction = rdc200_enable_gamma_correction;

    dev->change_input_display_interface = rdc200_change_input_display_interface;

	dev->reqeust_led_brightness_mode = rdc200_reqeust_led_brightness_mode;
	dev->reqeust_led_color_mode = rdc200_reqeust_led_color_mode;
	dev->confirm_led_control = rdc200_confirm_led_control;

	return 0;
}
#endif // #if defined(CFG_MDC_RDC200)

