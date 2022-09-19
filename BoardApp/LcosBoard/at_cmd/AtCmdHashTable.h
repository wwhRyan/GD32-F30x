#ifndef __ATCMDHASHTABLE_H__
#define __ATCMDHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef struct __asAtKvList asAtKvList;
typedef enum __aeCmdName{
	kCmdCurrent = 0x0,
	kCmdTemperature = 0x1,
	kCmdError = 0x2,
	kCmdSn = 0x3,
	kCmdUpgradeOvp921Firmware = 0x4,
	kCmdEeprom = 0x5,
	kCmdLightSourceTime = 0x6,
	kCmdTestPattern = 0x7,
	kCmdVersion = 0x8,
	kCmdFlashBootb = 0x9,
	kCmdLightSource = 0xa,
	kCmdSilentAsyncMessages = 0xb,
	kCmdSpiFlash = 0xc,
	kCmdSystem = 0xd,
	kCmdLogInfo = 0xe,
	kCmdInstallationMode = 0xf,
	kCmdUpgradeOvp921Anf = 0x10,
	kCmdCwSpeed = 0x11,
	kCmdReset = 0x12,
	kCmdOvp921 = 0x13,
	kCmdRdc200a = 0x14,
	kCmdRdp250h = 0x15,
} aeCmdName;

extern unsigned short g_atcmd_num;
extern asCmdListUnit gsCmdList[];
extern uint16_t gScmd_list_len;


#ifdef __cplusplus
}
#endif

#endif
