#ifndef __ATCMDHASHTABLE_H__
#define __ATCMDHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef struct __asAtKvList asAtKvList;
typedef enum __aeCmdName{
	kCmdTemperature = 0x0,
	kCmdLightSourceTime = 0x1,
	kCmdSystem = 0x2,
	kCmdInstallationMode = 0x3,
	kCmdLogInfo = 0x4,
	kCmdLightSource = 0x5,
	kCmdFlashBootb = 0x6,
	kCmdCwSpeed = 0x7,
	kCmdReset = 0x8,
	kCmdSpiFlash = 0x9,
	kCmdEeprom = 0xa,
	kCmdRdc200a = 0xb,
	kCmdVersion = 0xc,
	kCmdTestPattern = 0xd,
	kCmdRdp250h = 0xe,
	kCmdError = 0xf,
	kCmdSn = 0x10,
	kCmdSilentAsyncMessages = 0x11,
	kCmdUpgradeOvp921Firmware = 0x12,
	kCmdCurrent = 0x13,
	kCmdUpgradeOvp921Anf = 0x14,
	kCmdOvp921 = 0x15,
} aeCmdName;

extern unsigned short g_atcmd_num;
extern asCmdListUnit gsCmdList[];
extern uint16_t gScmd_list_len;


#ifdef __cplusplus
}
#endif

#endif
