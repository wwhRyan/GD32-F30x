#ifndef __ATCMDHASHTABLE_H__
#define __ATCMDHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef struct __asAtKvList asAtKvList;
typedef enum __aeCmdName{
	kCmdSystem = 0x0,
	kCmdLightSource = 0x1,
	kCmdUpgradeOvp921Anf = 0x2,
	kCmdRdc200a = 0x3,
	kCmdSpiFlash = 0x4,
	kCmdTestPattern = 0x5,
	kCmdError = 0x6,
	kCmdEeprom = 0x7,
	kCmdLogInfo = 0x8,
	kCmdInstallationMode = 0x9,
	kCmdSilentAsyncMessages = 0xa,
	kCmdFlashBootb = 0xb,
	kCmdRdp250h = 0xc,
	kCmdOvp921 = 0xd,
	kCmdLightSourceTime = 0xe,
	kCmdSpiFlashErase = 0xf,
	kCmdUpgradeOvp921Firmware = 0x10,
	kCmdSn = 0x11,
	kCmdReset = 0x12,
	kCmdVersion = 0x13,
	kCmdCurrent = 0x14,
	kCmdCwSpeed = 0x15,
	kCmdTemperature = 0x16,
} aeCmdName;

extern unsigned short g_atcmd_num;
extern asCmdListUnit gsCmdList[];
extern uint16_t gScmd_list_len;


#ifdef __cplusplus
}
#endif

#endif
