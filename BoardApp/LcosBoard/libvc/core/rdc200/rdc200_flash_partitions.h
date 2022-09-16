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

#ifndef __RDC200_FLASH_PARTITIONS_H__
#define __RDC200_FLASH_PARTITIONS_H__

#ifdef __cplusplus 
extern "C"{
#endif

#include "rti_vc_types.h"

#define MAX_NUM_PANEL	2

typedef enum
{
	FLASH_FW_CFG_PANEL_MODEL_IDX_RDP501H = 0,
	FLASH_FW_CFG_PANEL_MODEL_IDX_RDP360H = 1,
	FLASH_FW_CFG_PANEL_MODEL_IDX_RDP550F = 2,
	FLASH_FW_CFG_PANEL_MODEL_IDX_RDP700Q = 3,
	FLASH_FW_CFG_PANEL_MODEL_IDX_RDP551F = 4,
	FLASH_FW_CFG_PANEL_MODEL_IDX_RDP502H = 5,
	FLASH_FW_CFG_PANEL_MODEL_IDX_RDP370F = 6,
	MAX_NUM_FLASH_FW_CFG_PANEL_MODEL_IDX
} E_FLASH_FW_CFG_PANEL_MODEL_IDX_T;

typedef enum
{
	FLASH_FW_RESOULTION_IDX_MANUAL = 0, // Use LUT
	FLASH_FW_RESOULTION_IDX_HD = 1,
	FLASH_FW_RESOULTION_IDX_FHD = 2,
	FLASH_FW_RESOULTION_IDX_2K1K = 3,
	FLASH_FW_RESOULTION_IDX_QHD = 4,
	FLASH_FW_RESOULTION_IDX_2K2K = 5,
	FLASH_FW_RESOULTION_IDX_WQXGA = 6,
	FLASH_FW_RESOULTION_IDX_HD3D = 7,
	MAX_NUM_FLASH_FW_RESOULTION_IDX
} E_FLASH_FW_RESOULTION_IDX_T;

typedef enum
{
	FLASH_FW_CFG_DIF_PATH_IDX_LVDS = 0,
	FLASH_FW_CFG_DIF_PATH_IDX_LVCMOS = 1,
	FLASH_FW_CFG_DIF_PATH_IDX_MIPI = 2,
	FLASH_FW_CFG_DIF_PATH_IDX_HDMI = 3,
	FLASH_FW_CFG_DIF_PATH_IDX_PG = 4, // Pattern Generator
	FLASH_FW_CFG_DIF_PATH_IDX_DIS = 5, // input path disbale,
	MAX_NUM_FLASH_FW_CFG_DIF_PATH_IDX
} E_FLASH_FW_CFG_DIF_PATH_IDX_T;

typedef enum
{
	FLASH_FW_CFG_LVDS_PCLK_PORT0 = 0,
	FLASH_FW_CFG_LVDS_PCLK_PORT1 = 1,
	MAX_NUM_FLASH_FW_CFG_LVDS_PCLK
} E_FLASH_FW_CFG_LVDS_PLCK_T;

typedef enum
{
	FLASH_FW_CFG_LVDS_PORT_SINGLE_0 = 0,
	FLASH_FW_CFG_LVDS_PORT_SINGLE_1 = 1,
	FLASH_FW_CFG_LVDS_PORT_DUAL_0 = 2,
	FLASH_FW_CFG_LVDS_PORT_DUAL_1 = 3,
	MAX_NUM_FLASH_FW_CFG_LVDS_PORT
} E_FLASH_FW_CFG_LVDS_PORT_T;

typedef enum
{
	FLASH_FW_CFG_LVDS_REV_BUS_DIS = 0,
	FLASH_FW_CFG_LVDS_REV_BUS_0 = 1,
	FLASH_FW_CFG_LVDS_REV_BUS_1 = 2,
	FLASH_FW_CFG_LVDS_REV_BUS_BOTH = 3,
	MAX_NUM_FLASH_FW_CFG_LVDS_REV_BUS
} E_FLASH_FW_CFG_LVDS_REV_BUS_T;

typedef enum
{
	FLASH_FW_CFG_LVDS_COLOR_DEPTH_24 = 0,
	FLASH_FW_CFG_LVDS_COLOR_DEPTH_18 = 1,
	MAX_NUM_FLASH_FW_CFG_LVDS_COLOR_DEPTH
} E_FLASH_FW_CFG_LVDS_COLOR_DEPTH_T;

typedef enum
{
	FLASH_FW_CFG_RDC_INIT_SEL_FILE = 0,
	FLASH_FW_CFG_RDC_INIT_SEL_CFG = 1,
	MAX_NUM_FLASH_FW_CFG_RDC_INIT_SEL
} E_FLASH_FW_CFG_RDC_INIT_SEL_T;

// DR: Data Rate
typedef enum
{
	FLASH_FW_CFG_MIPI_DR_500Mbps = 0,
	FLASH_FW_CFG_MIPI_DR_700Mbps = 1,
	FLASH_FW_CFG_MIPI_DR_864Mbps = 2,
	FLASH_FW_CFG_MIPI_DR_1000Mbps = 3,
	FLASH_FW_CFG_MIPI_DR_1344Mbps = 4,
	FLASH_FW_CFG_MIPI_DR_1500Mbps = 5,
	MAX_NUM_FLASH_FW_CFG_MIPI_DR
} E_FLASH_FW_CFG_MIPI_DR_T;

typedef enum
{
	FLASH_FW_CFG_MIPI_NUM_LANE_1 = 0,
	FLASH_FW_CFG_MIPI_NUM_LANE_2 = 1,
	FLASH_FW_CFG_MIPI_NUM_LANE_3 = 2,
	FLASH_FW_CFG_MIPI_NUM_LANE_4 = 3,
	MAX_NUM_FLASH_FW_CFG_MIPI_NUM_LANE
} E_FLASH_FW_CFG_MIPI_NUM_LANE_T;

// Auto power down(PD)
typedef enum
{
	FLASH_FW_CFG_MIPI_AUTO_PD_pwr_up_inactive = 0,
	FLASH_FW_CFG_MIPI_AUTO_PD_pwr_down_inactive = 1,
	MAX_NUM_FLASH_FW_CFG_MIPI_AUTO_PD
} E_FLASH_FW_CFG_MIPI_AUTO_PD_T;

// Continnous clock mode
typedef enum
{
	FLASH_FW_CFG_MIPI_CONT_CLK_none = 0,
	FLASH_FW_CFG_MIPI_CONT_CLK_cont = 1,
	MAX_NUM_FLASH_FW_CFG_MIPI_CONT_CLK
} E_FLASH_FW_CFG_MIPI_CONT_CLK_T;

#if defined(_WIN32) || defined(WINDOWS)
#pragma pack(push, 1)
#endif

/*
Configuration Version
*/

#define RDC_FW_CFG_VER_MAJOR		1 
#define RDC_FW_CFG_VER_MINOR		0
#define RDC_FW_CFG_VER_REL			0

#define FLASH_FW_CFG_MAGIC_CODE		0xDEAD4EAD

/*
 Fiwmware configuration information.
 Save as the big-endian format.
*/
typedef struct
{
	U32_T magic_code;        // offset[0x00], 4 bytes
	U32_T version;           // offset[0x04], VC version: [0x04](Major), [0x05](Minor), [0x06](release), [0x07](reserved)

	unsigned char panel_model;       // offset[0x08],
	unsigned char panel_attached_port_bits; // offset[0x09], for RDP501H (No has I2C)
	unsigned char I2C_addr_panel_0;  // offset[0x0A], [6:0]: addr
	unsigned char I2C_addr_panel_1;  // offset[0x0B], [6:0]: addr
	unsigned char rdc_init_sel;      // offset[0x0C], 0: init file, 1: configuration data
	unsigned char input_resoultion;  // offset[0x0D],
	unsigned char output_resoultion; // offset[0x0E],
	unsigned char dif_path;          // offset[0x0F],

	unsigned char lvds_pclk;         // offset[0x10],
	unsigned char lvds_port;         // offset[0x11],
	unsigned char lvds_rev_bus;      // offset[0x12],
	unsigned char lvds_color_depth;  // offset[0x13],

	unsigned char mipi_dr;           // offset[0x14],
	unsigned char mipi_num_lane;     // offset[0x15],
	unsigned char mipi_auto_pd;      // offset[0x16],
	unsigned char mipi_cont_clk;     // offset[0x17],

	U8_T bootlogo_en; // [7:0]  0x01: enable, 0x00 or 0xFF: disable
	U8_T bootlogo_lut;
	U8_T mag; // [7:0] used
	U8_T debug_en; // 1: enable, 0: disable: fw cfg, build date, i2c fail, boot completed
	U16_T pos_x; // [7:0]
	U16_T pos_y; // [7:0]

	/* Power on/off Panel when boot time. 0: power off, 1: power on */
	U8_T boot_panel_on;

	/* Vsync signal indicator */
	U8_T vsync_sig_indicator;
	U16_T vsync_pulse_pos;
} GNUPACKED FLASH_FW_CFG_INFO_T;

#define FLASH_RDC_PATCH_MAGIC_CODE		0x1EDA4C3D
#define FLASH_PANEL_0_PATCH_MAGIC_CODE  0xFCBA0D10
#define FLASH_PANEL_1_PATCH_MAGIC_CODE  0xFCBB0D10
    
/**
 RDC/Panel LUT node.
*/
typedef struct {
	U16_T	reg;
	U8_T	val;
} GNUPACKED RD_LUT_NODE_T;

typedef struct {
	U32_T magic_code;
	U16_T num_node;
} GNUPACKED RD_LUT_HEADER_T;

/* RDC record type stored in FLASH. */
typedef struct {
	RD_LUT_HEADER_T	hdr; /* LUT Header */
	RD_LUT_NODE_T node[1]; /* N nodes */
} GNUPACKED RD_LUT_RECORD_T;

// Used to init LUT . To be changed in futher.....
typedef struct {
    U8_T panel_idx;
    U16_T num_set; // Number of data set (address and value).
    U32_T next_hdr; // next header offset. 0x0 = none.
} GNUPACKED PANEL_INIT_DATA_HEADER_T;

#if defined(_WIN32) || defined(WINDOWS) || defined(_WIN64)
#pragma pack(pop)
#endif

#define FLASH_BLOCK_SIZE		0x8000
#define FLASH_SECTOR_SIZE		0x1000

/*=============================================================================
*
* Partitions
*
* NOTE: Assum that the size of FLASH block is 32KB(max)
*       and the size of FLASH sector is 4KB.
*=============================================================================*/
#define FLASH_PART_OFFSET_BOOT_FW				0x000000
	#define FLASH_PART_SIZE_BOOT_FW				0x8000

#define FLASH_PART_OFFSET_FW_CFG				0x008000
	#define FLASH_PART_SIZE_FW_CFG				256

#define FLASH_PART_OFFSET_RDC200_INIT_REG		0x009000
	#define FLASH_PART_SIZE_RDC200_INIT			0x1000
	#define FLASH_ITEM_SIZE_RDC200_INIT			2048 // MAX: count(2 bytes) [addr(2 bytes) + data(1byte) ....]

#define FLASH_PART_OFFSET_PANEL_INIT_BASE		0x00A000
	#define FLASH_PART_SIZE_PANEL_INIT			0x1000
	#define FLASH_ITEM_SIZE_PANEL_INIT			2048 // addr(2 bytes) + data(1byte) + sizeof(PANEL_LUT_HEADER_T): per panel

	#define FLASH_ITEM_OFFSET_PANEL_INIT(lut_offset)\
		(FLASH_PART_OFFSET_PANEL_INIT_BASE + (FLASH_BLOCK_SIZE * (lut_offset)))


/* Gamma Correction. */
#define FLASH_PART_OFFSET_PANEL_GAMMA_BASE		0x00C000
	#define FLASH_PART_SIZE_PANEL_GAMMA_LUT		0x1000 // each LUT item size
	
	#define FLASH_SUBITEM_SIZE_PANNEL_GAMMA		512 // each sub-item size
	#define FLASH_REAL_LUT_SIZE_PANEL_GAMMA		(6 * FLASH_SUBITEM_SIZE_PANNEL_GAMMA) // real LUT size

	// Memory offset in a LUT.
	#define FLASH_ITEM_OFFSET_PANEL_GAMMA(lut_offset)\
		(FLASH_SECTOR_SIZE * (lut_offset))

	#define FLASH_ITEM_OFFSET_GAMMA_LOW_RED(lut_offset)\
		(FLASH_ITEM_OFFSET_PANEL_GAMMA(lut_offset))

	#define FLASH_ITEM_OFFSET_GAMMA_LOW_GREEN(lut_offset)\
		(FLASH_ITEM_OFFSET_GAMMA_LOW_RED(lut_offset) + FLASH_SUBITEM_SIZE_PANNEL_GAMMA)

	#define FLASH_ITEM_OFFSET_GAMMA_LOW_BLUE(lut_offset)\
		(FLASH_ITEM_OFFSET_GAMMA_LOW_GREEN(lut_offset) + FLASH_SUBITEM_SIZE_PANNEL_GAMMA)

	#define FLASH_ITEM_OFFSET_GAMMA_HIGH_RED(lut_offset)\
		(FLASH_ITEM_OFFSET_GAMMA_LOW_BLUE(lut_offset) + FLASH_SUBITEM_SIZE_PANNEL_GAMMA)

	#define FLASH_ITEM_OFFSET_GAMMA_HIGH_GREEN(lut_offset)\
		(FLASH_ITEM_OFFSET_GAMMA_HIGH_RED(lut_offset) + FLASH_SUBITEM_SIZE_PANNEL_GAMMA)

	#define FLASH_ITEM_OFFSET_GAMMA_HIGH_BLUE(lut_offset)\
		(FLASH_ITEM_OFFSET_GAMMA_HIGH_GREEN(lut_offset) + FLASH_SUBITEM_SIZE_PANNEL_GAMMA)

/* Horizontal Correction */
#define FLASH_PART_OFFSET_PANNEL_HC_BASE		0x070000
	#define FLASH_PART_SIZE_PANEL_HC_LUT		0x1000
	#define FLASH_SUBITEM_SIZE_PANNEL_HC		0x800 // each sub-item size

	#define FLASH_ITEM_SIZE_PANEL_HC\
		(2*FLASH_SUBITEM_SIZE_PANNEL_HC)

	#define FLASH_ITEM_OFFSET_HC_EVEN\
		(FLASH_PART_OFFSET_PANNEL_HC_BASE)

	#define FLASH_ITEM_OFFSET_HC_ODD\
		(FLASH_ITEM_OFFSET_HC_EVEN + FLASH_SUBITEM_SIZE_PANNEL_HC)
    
#define FLASH_PART_OFFSET_RDC_PATCH_BASE    0x071000
    #define FLASH_PART_SIZE_RDC_PATCH        0x1000

#define FLASH_PART_OFFSET_PANEL_0_PATCH_BASE    0x072000
#define FLASH_PART_OFFSET_PANEL_1_PATCH_BASE    0x073000
    #define FLASH_PART_SIZE_PANEL_PATCH        0x1000

/* Vertical Correction */
#define FLASH_PART_OFFSET_PANNEL_VC_BASE		0x074000
	#define FLASH_PART_SIZE_PANEL_VC_LUT		0x1000
	#define FLASH_SUBITEM_SIZE_PANNEL_VC		0x300 // each sub-item size

	#define FLASH_ITEM_SIZE_PANEL_VC\
		(4*FLASH_SUBITEM_SIZE_PANNEL_VC)

	#define FLASH_ITEM_OFFSET_VC_0_3\
		(FLASH_PART_OFFSET_PANNEL_VC_BASE)

	#define FLASH_ITEM_OFFSET_VC_4_7\
		(FLASH_ITEM_OFFSET_VC_0_3 + FLASH_SUBITEM_SIZE_PANNEL_VC)

	#define FLASH_ITEM_OFFSET_VC_8_11\
		(FLASH_ITEM_OFFSET_VC_4_7 + FLASH_SUBITEM_SIZE_PANNEL_VC)

	#define FLASH_ITEM_OFFSET_VC_12_15\
		(FLASH_ITEM_OFFSET_VC_8_11 + FLASH_SUBITEM_SIZE_PANNEL_VC)

/*
 Distortion Correction (Optic)

 -40 degree: base + sector_size x 0 (RED, GREEN, BLUE)
 -39 degree: base + sector_size x 1
 	.
  0 degree: base + sector_size x 40	
	.
 +40 degree: base + sector_size x 80
*/
#define FLASH_PART_OFFSET_DC				0x010000
	#define FLASH_PART_SIZE_DC					0x60000 // total part size

	#define FLASH_ITEM_SIZE_DC					0x50C // 1 item: 1292 bytes
	#define FLASH_NUM_ITEM_DC_PER_SECTOR		3
	#define FLASH_DC_LUT_INFO_SIZE				(FLASH_NUM_ITEM_DC_PER_SECTOR * FLASH_ITEM_SIZE_DC)

	#define FLASH_ITEM_NUM_DC_SET				81

	#define MIN_DC_DEGREE			-40
	#define MAX_DC_DEGREE			40

	// Memory offset in a LUT.
	#define FLASH_ITEM_OFFSET_DC(lut_offset)\
		(FLASH_SECTOR_SIZE * (lut_offset))
	
	#define FLASH_ITEM_OFFSET_DC_RED(lut_offset)\
		(FLASH_ITEM_OFFSET_DC(lut_offset) + FLASH_ITEM_SIZE_DC * 0)

	#define FLASH_ITEM_OFFSET_DC_GREEN(lut_offset)\
		(FLASH_ITEM_OFFSET_DC(lut_offset) + FLASH_ITEM_SIZE_DC * 1)

	#define FLASH_ITEM_OFFSET_DC_BLUE(lut_offset)\
		(FLASH_ITEM_OFFSET_DC(lut_offset) + FLASH_ITEM_SIZE_DC * 2)

	// Parition address in FLASH.
	#define FLASH_ITEM_ADDR_DC(lut_offset)\
		(FLASH_PART_OFFSET_DC + FLASH_ITEM_OFFSET_DC(lut_offset))
	
	#define FLASH_ITEM_ADDR_DC_RED(lut_offset)\
		(FLASH_PART_OFFSET_DC + FLASH_ITEM_OFFSET_DC_RED(lut_offset))

	#define FLASH_ITEM_ADDR_DC_GREEN(lut_offset)\
		(FLASH_PART_OFFSET_DC + FLASH_ITEM_OFFSET_DC_GREEN(lut_offset))

	#define FLASH_ITEM_ADDR_DC_BLUE(lut_offset)\
		(FLASH_PART_OFFSET_DC + FLASH_ITEM_OFFSET_DC_BLUE(lut_offset))

#define FLASH_PART_OFFSET_FONT				0x078000
	#define FLASH_PART_SIZE_FONT			0x10000
	#define FLASH_ITEM_SIZE_FONT			0x1000 // each item size

	#define MAX_NUM_FLASH_ITEM_FONT			16

#define FLASH_PART_OFFSET_PALETTE			0x088000
	#define FLASH_PART_SIZE_PALETTE			0x2000
	#define FLASH_ITEM_SIZE_PALETTE			48 // each item size (0x30)
	#define FLASH_SET_SIZE_PALETTE			0x1000 // The size of color set.
	#define FLASH_COLOR_SET_ALIGN_SIZE		0x100

	#define MAX_NUM_FLASH_ITEM_PALETTE		2

	#define FLASH_ITEM_OFFSET_PALETTE(lut_offset)\
		(FLASH_SECTOR_SIZE * (lut_offset))
	

#define FLASH_PART_OFFSET_BMP				0x090000
	#define FLASH_PART_SIZE_BMP				0x40000
	#define FLASH_ITEM_SIZE_BMP				0x4000 // each item size
	#define FLASH_ITEM_ACTUAL_SIZE_BMP		15360 

	#define MAX_NUM_FLASH_ITEM_BMP			16

/*=============================================================================
*
* Target SRAM ID
*
*=============================================================================*/
/* Distortion Correction */
#define TSID_DC_RED					0x00
#define TSID_DC_GREEN				0x01
#define TSID_DC_BLUE				0x02

/* Gamma Correction */
#define TSID_GC_LOW_RED				0x03
#define TSID_GC_LOW_GREEN			0x04
#define TSID_GC_LOW_BLUE			0x05
#define TSID_GC_HIGH_RED			0x06
#define TSID_GC_HIGH_GREEN			0x07
#define TSID_GC_HIGH_BLUE			0x08

/* Horizontal Correction */
#define TSID_HC_EVEN				0x09
#define TSID_HC_ODD					0x0A

/* Vertical Correction */
#define TSID_VC_0_3					0x0B
#define TSID_VC_4_7					0x0C
#define TSID_VC_8_11				0x0D
#define TSID_VC_12_15				0x0E

/* Bitmap drawer */
#define TSID_BMP_DRAWER_0			0x80
#define TSID_BMP_DRAWER_1			0x81
#define TSID_BMP_DRAWER_2			0x82
#define TSID_BMP_DRAWER_3			0x83

/* Font Bitmap for text box */
#define TSID_FONT_BMP_0				0x84
#define TSID_FONT_BMP_1				0x85
#define TSID_FONT_BMP_2				0x86
#define TSID_FONT_BMP_3				0x87

/* font/bg color for text box */
#define TSID_FONT_BG_COLOR_0		0x88
#define TSID_FONT_BG_COLOR_1		0x89
#define TSID_FONT_BG_COLOR_2		0x8A
#define TSID_FONT_BG_COLOR_3		0x8B

#ifdef __cplusplus
}
#endif

#endif // __RDC200_FLASH_PARTITIONS_H__

