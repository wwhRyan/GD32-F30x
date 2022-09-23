#ifndef __ATKEYHASHTABLE_H__
#define __ATKEYHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef enum __aeKeyName{
	kKeyScatteringWheel = 0x0,
	kKeyYellow = 0x1,
	kKeyEmpty = 0x2,
	kKeyOk = 0x3,
	kKeyGrid = 0x4,
	kKeyI2cEnv = 0x5,
	kKeyY = 0x6,
	kKeyFactory = 0x7,
	kKeyI2cLcos = 0x8,
	kKeyBlue = 0x9,
	kKeyMinute = 0xa,
	kKeyCeilingFront = 0xb,
	kKeyBlack = 0xc,
	kKeyB = 0xd,
	kKeyNtcLcos = 0xe,
	kKeyLightEngine = 0xf,
	kKeyTableRear = 0x10,
	kKeyAnf2 = 0x11,
	kKeySourceLight = 0x12,
	kKeyMagenta = 0x13,
	kKeyTableFront = 0x14,
	kKeyNtcRed = 0x15,
	kKeyAnf1 = 0x16,
	kKeyHour = 0x17,
	kKeyCeilingRear = 0x18,
	kKeyGrey = 0x19,
	kKeyRed = 0x1a,
	kKeyIdle = 0x1b,
	kKeyI2cRed = 0x1c,
	kKeyUser = 0x1d,
	kKeyLightEngineBoard = 0x1e,
	kKeyEeprom = 0x1f,
	kKeyData = 0x20,
	kKeyMcu = 0x21,
	kKeyOvp921 = 0x22,
	kKeyAnf3 = 0x23,
	kKeyCrc = 0x24,
	kKeySize = 0x25,
	kKeyRdc200a = 0x26,
	kKeyStatus = 0x27,
	kKeyWhite = 0x28,
	kKeyCheckerboard = 0x29,
	kKeyI2cBlue = 0x2a,
	kKeyClear = 0x2b,
	kKeyRdp250h = 0x2c,
	kKeyOff = 0x2d,
	kKeyOn = 0x2e,
	kKeyNtcEnv = 0x2f,
	kKeyG = 0x30,
	kKeyAll = 0x31,
	kKeyText = 0x32,
	kKeyR = 0x33,
	kKeyNtcGreen = 0x34,
	kKeyGreen = 0x35,
	kKeyI2cGreen = 0x36,
	kKeyIdx = 0x37,
	kKeyAddr = 0x38,
	kKeyProjector = 0x39,
	kKeyCyan = 0x3a,
	kKeyNtcBlue = 0x3b,
	kKeyHorizontalRamp = 0x3c,
	kKeySecond = 0x3d,
	kKeyVerticalRamp = 0x3e,
	kKeyTime = 0x3f,
} aeKeyName;

extern unsigned short g_atkey_num;
extern asKeyListUnit gsKeyList[];
extern uint16_t gSkey_list_len;


#ifdef __cplusplus
}
#endif

#endif
