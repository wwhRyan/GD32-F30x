#ifndef __ATCMDHASHTABLE_H__
#define __ATCMDHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef struct __asAtKvList asAtKvList;
typedef enum __aeCmdName{
	kCmdCwSpeed = 0x0,
	kCmdLightSourceTime = 0x1,
	kCmdOvp921 = 0x2,
	kCmdReset = 0x3,
	kCmdSystem = 0x4,
	kCmdError = 0x5,
	kCmdUpgradeOvp921Firmware = 0x6,
	kCmdEeprom = 0x7,
	kCmdUpgradeOvp921Anf = 0x8,
	kCmdLogInfo = 0x9,
	kCmdLightSource = 0xa,
	kCmdVersion = 0xb,
	kCmdCurrent = 0xc,
	kCmdInstallationMode = 0xd,
	kCmdSilentAsyncMessages = 0xe,
	kCmdTestPattern = 0xf,
	kCmdSn = 0x10,
	kCmdTemperature = 0x11,
} aeCmdName;

extern unsigned short g_atcmd_num;
extern asCmdListUnit gsCmdList[];
extern uint16_t gScmd_list_len;


#ifdef __cplusplus
}
#endif

#endif
