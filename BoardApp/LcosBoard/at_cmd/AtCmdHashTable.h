#ifndef __ATCMDHASHTABLE_H__
#define __ATCMDHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef struct __asAtKvList asAtKvList;
typedef enum __aeCmdName{
	kCmdTemperature = 0x0,
	kCmdSn = 0x1,
	kCmdCwSpeed = 0x2,
	kCmdReset = 0x3,
	kCmdCurrent = 0x4,
	kCmdUpgradeOvp921Firmware = 0x5,
	kCmdSilentAsyncMessages = 0x6,
	kCmdFlashBootb = 0x7,
	kCmdSpiFlash = 0x8,
	kCmdVersion = 0x9,
	kCmdRdc200a = 0xa,
	kCmdTestPattern = 0xb,
	kCmdOvp921 = 0xc,
	kCmdSystem = 0xd,
	kCmdRdp250h = 0xe,
	kCmdUpgradeOvp921Anf = 0xf,
	kCmdEeprom = 0x10,
	kCmdLightSource = 0x11,
	kCmdLogInfo = 0x12,
	kCmdInstallationMode = 0x13,
	kCmdError = 0x14,
	kCmdLightSourceTime = 0x15,
} aeCmdName;

extern unsigned short g_atcmd_num;
extern asCmdListUnit gsCmdList[];
extern uint16_t gScmd_list_len;


#ifdef __cplusplus
}
#endif

#endif
