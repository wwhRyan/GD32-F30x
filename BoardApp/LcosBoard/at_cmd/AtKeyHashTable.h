#ifndef __ATKEYHASHTABLE_H__
#define __ATKEYHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef enum __aeKeyName{
	kKeyEeprom = 0x0,
	kKeyIdle = 0x1,
	kKeyNtcRed = 0x2,
	kKeyBlack = 0x3,
	kKeyHour = 0x4,
	kKeyUser = 0x5,
	kKeyYellow = 0x6,
	kKeyBlue = 0x7,
	kKeySourceLight = 0x8,
	kKeyTableFront = 0x9,
	kKeyGreen = 0xa,
	kKeyG = 0xb,
	kKeyVerticalRamp = 0xc,
	kKeyGrey = 0xd,
	kKeyWhite = 0xe,
	kKeyStatus = 0xf,
	kKeyOk = 0x10,
	kKeyRed = 0x11,
	kKeyI2cBlue = 0x12,
	kKeyNtcLcos = 0x13,
	kKeyCeilingRear = 0x14,
	kKeyMinute = 0x15,
	kKeyAddr = 0x16,
	kKeyMcu = 0x17,
	kKeyY = 0x18,
	kKeyClear = 0x19,
	kKeyI2cEnv = 0x1a,
	kKeyFactory = 0x1b,
	kKeyOvp921 = 0x1c,
	kKeyAnf1 = 0x1d,
	kKeyCrc = 0x1e,
	kKeyOff = 0x1f,
	kKeyData = 0x20,
	kKeyAnf3 = 0x21,
	kKeyRdc200a = 0x22,
	kKeyNtcEnv = 0x23,
	kKeyEmpty = 0x24,
	kKeyGrid = 0x25,
	kKeyHorizontalRamp = 0x26,
	kKeyMagenta = 0x27,
	kKeyI2cLcos = 0x28,
	kKeyR = 0x29,
	kKeyAnf2 = 0x2a,
	kKeyNtcBlue = 0x2b,
	kKeyCyan = 0x2c,
	kKeyAll = 0x2d,
	kKeySize = 0x2e,
	kKeyRdp250h = 0x2f,
	kKeyIdx = 0x30,
	kKeyLightEngine = 0x31,
	kKeyNtcGreen = 0x32,
	kKeyOn = 0x33,
	kKeyTableRear = 0x34,
	kKeyCheckerboard = 0x35,
	kKeySecond = 0x36,
	kKeyB = 0x37,
	kKeyText = 0x38,
	kKeyRegLcos = 0x39,
	kKeyScatteringWheel = 0x3a,
	kKeyCeilingFront = 0x3b,
	kKeyTime = 0x3c,
	kKeyProjector = 0x3d,
	kKeyI2cRed = 0x3e,
	kKeyLightEngineBoard = 0x3f,
	kKeyI2cGreen = 0x40,
} aeKeyName;

extern unsigned short g_atkey_num;
extern asKeyListUnit gsKeyList[];
extern uint16_t gSkey_list_len;


#ifdef __cplusplus
}
#endif

#endif
