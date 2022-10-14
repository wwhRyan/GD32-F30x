#ifndef __ATKEYHASHTABLE_H__
#define __ATKEYHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef enum __aeKeyName{
	kKeyB = 0x0,
	kKeyG = 0x1,
	kKeyR = 0x2,
	kKeyY = 0x3,
	kKeyOk = 0x4,
	kKeyOn = 0x5,
	kKeyOvp921 = 0x6,
	kKeyVerticalRamp = 0x7,
	kKeyNtcGreen = 0x8,
	kKeyData = 0x9,
	kKeyI2cLcos = 0xa,
	kKeyEeprom = 0xb,
	kKeyRdp250h = 0xc,
	kKeyHorizontalRamp = 0xd,
	kKeyScatteringWheel = 0xe,
	kKeyText = 0xf,
	kKeyMcu = 0x10,
	kKeyTableFront = 0x11,
	kKeySize = 0x12,
	kKeyBlack = 0x13,
	kKeyNtcBlue = 0x14,
	kKeyAll = 0x15,
	kKeyAnf1 = 0x16,
	kKeyAnf2 = 0x17,
	kKeyAnf3 = 0x18,
	kKeyNtcEnv = 0x19,
	kKeyFactory = 0x1a,
	kKeyProjector = 0x1b,
	kKeyNtcRed = 0x1c,
	kKeyYellow = 0x1d,
	kKeyLightEngineBoard = 0x1e,
	kKeyGreen = 0x1f,
	kKeyGrey = 0x20,
	kKeyGrid = 0x21,
	kKeyOff = 0x22,
	kKeyBlue = 0x23,
	kKeyRegLcos = 0x24,
	kKeyCrc = 0x25,
	kKeyHour = 0x26,
	kKeyAddr = 0x27,
	kKeyCeilingRear = 0x28,
	kKeyNtcLcos = 0x29,
	kKeySourceLight = 0x2a,
	kKeyTime = 0x2b,
	kKeyMinute = 0x2c,
	kKeyTableRear = 0x2d,
	kKeyCeilingFront = 0x2e,
	kKeyClear = 0x2f,
	kKeyLightEngine = 0x30,
	kKeyRed = 0x31,
	kKeyUser = 0x32,
	kKeyStatus = 0x33,
	kKeyMagenta = 0x34,
	kKeyRdc200a = 0x35,
	kKeyCheckerboard = 0x36,
	kKeyCyan = 0x37,
	kKeyWhite = 0x38,
	kKeyEmpty = 0x39,
	kKeySecond = 0x3a,
	kKeyIdle = 0x3b,
	kKeyI2cBlue = 0x3c,
	kKeyI2cEnv = 0x3d,
	kKeyIdx = 0x3e,
	kKeyI2cRed = 0x3f,
	kKeyI2cGreen = 0x40,
} aeKeyName;

extern unsigned short g_atkey_num;
extern asKeyListUnit gsKeyList[];
extern uint16_t gSkey_list_len;


#ifdef __cplusplus
}
#endif

#endif
