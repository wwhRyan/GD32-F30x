#ifndef __ATKEYHASHTABLE_H__
#define __ATKEYHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef enum __aeKeyName{
	kKeyCeilingFront = 0x0,
	kKeyIdle = 0x1,
	kKeyProjector = 0x2,
	kKeyOff = 0x3,
	kKeyClear = 0x4,
	kKeyEeprom = 0x5,
	kKeyAnf2 = 0x6,
	kKeyTableFront = 0x7,
	kKeyData = 0x8,
	kKeyLightEngine = 0x9,
	kKeyNtcRed = 0xa,
	kKeyI2cRed = 0xb,
	kKeyY = 0xc,
	kKeyCheckerboard = 0xd,
	kKeyYellow = 0xe,
	kKeyR = 0xf,
	kKeyMagenta = 0x10,
	kKeyRdc200a = 0x11,
	kKeyMinute = 0x12,
	kKeyAnf1 = 0x13,
	kKeyAnf3 = 0x14,
	kKeyAddr = 0x15,
	kKeyTableRear = 0x16,
	kKeyEmpty = 0x17,
	kKeyFactory = 0x18,
	kKeyWhite = 0x19,
	kKeyVerticalRamp = 0x1a,
	kKeyBlue = 0x1b,
	kKeySecond = 0x1c,
	kKeyI2cBlue = 0x1d,
	kKeyB = 0x1e,
	kKeySourceLight = 0x1f,
	kKeySize = 0x20,
	kKeyG = 0x21,
	kKeyOk = 0x22,
	kKeyAll = 0x23,
	kKeyRdp250h = 0x24,
	kKeyCeilingRear = 0x25,
	kKeyHorizontalRamp = 0x26,
	kKeyGrey = 0x27,
	kKeyI2cLcos = 0x28,
	kKeyMcu = 0x29,
	kKeyHour = 0x2a,
	kKeyNtcLcos = 0x2b,
	kKeyUser = 0x2c,
	kKeyGreen = 0x2d,
	kKeyOn = 0x2e,
	kKeyGrid = 0x2f,
	kKeyOvp921 = 0x30,
	kKeyI2cEnv = 0x31,
	kKeyLightEngineBoard = 0x32,
	kKeyNtcGreen = 0x33,
	kKeyStatus = 0x34,
	kKeyI2cGreen = 0x35,
	kKeyBlack = 0x36,
	kKeyCyan = 0x37,
	kKeyNtcBlue = 0x38,
	kKeyCrc = 0x39,
	kKeyScatteringWheel = 0x3a,
	kKeyRed = 0x3b,
	kKeyIdx = 0x3c,
	kKeyNtcEnv = 0x3d,
} aeKeyName;

extern unsigned short g_atkey_num;
extern asKeyListUnit gsKeyList[];
extern uint16_t gSkey_list_len;


#ifdef __cplusplus
}
#endif

#endif
