#ifndef __ATCMDHASHTABLE_H__
#define __ATCMDHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef struct __asAtKvList asAtKvList;
typedef enum __aeCmdName{
	kCmdSn = 0x0,
	kCmdOvp921 = 0x1,
	kCmdSystem = 0x2,
	kCmdInstallationMode = 0x3,
	kCmdVersion = 0x4,
	kCmdUpgradeOvp921Firmware = 0x5,
	kCmdEeprom = 0x6,
	kCmdRdp250h = 0x7,
	kCmdLightSource = 0x8,
	kCmdCurrent = 0x9,
	kCmdCwSpeed = 0xa,
	kCmdError = 0xb,
	kCmdTemperature = 0xc,
	kCmdSpiFlash = 0xd,
	kCmdLightSourceTime = 0xe,
	kCmdReset = 0xf,
	kCmdFlashBootb = 0x10,
	kCmdRdc200a = 0x11,
	kCmdTestPattern = 0x12,
	kCmdSpiFlashErase = 0x13,
	kCmdSilentAsyncMessages = 0x14,
	kCmdUpgradeOvp921Anf = 0x15,
	kCmdLogInfo = 0x16,
} aeCmdName;

extern unsigned short g_atcmd_num;
extern asCmdListUnit gsCmdList[];
extern uint16_t gScmd_list_len;


#ifdef __cplusplus
}
#endif

#endif
