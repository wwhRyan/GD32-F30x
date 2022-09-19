#ifndef __ATKEYHASHTABLE_H__
#define __ATKEYHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef enum __aeKeyName{
	kKeyStatus = 0x0,
	kKeySourceLight = 0x1,
	kKeyRdp250h = 0x2,
	kKeyEeprom = 0x3,
	kKeyFactory = 0x4,
	kKeyUser = 0x5,
	kKeyNtcEnv = 0x6,
	kKeyLightEngineBoard = 0x7,
	kKeyScatteringWheel = 0x8,
	kKeyProjector = 0x9,
	kKeyHour = 0xa,
	kKeyMcu = 0xb,
	kKeyGrey = 0xc,
	kKeyI2cGreen = 0xd,
	kKeyHorizontalRamp = 0xe,
	kKeyOk = 0xf,
	kKeyI2cRed = 0x10,
	kKeyI2cLcos = 0x11,
	kKeyNtcBlue = 0x12,
	kKeyCrc = 0x13,
	kKeyCheckerboard = 0x14,
	kKeyNtcRed = 0x15,
	kKeyData = 0x16,
	kKeyMagenta = 0x17,
	kKeyWhite = 0x18,
	kKeyNtcLcos = 0x19,
	kKeyAddr = 0x1a,
	kKeyIdx = 0x1b,
	kKeyTableRear = 0x1c,
	kKeyEmpty = 0x1d,
	kKeyY = 0x1e,
	kKeyAnf1 = 0x1f,
	kKeyRed = 0x20,
	kKeyNtcGreen = 0x21,
	kKeyCeilingRear = 0x22,
	kKeyRdc200a = 0x23,
	kKeyTableFront = 0x24,
	kKeyYellow = 0x25,
	kKeyText = 0x26,
	kKeyBlue = 0x27,
	kKeyI2cEnv = 0x28,
	kKeyGrid = 0x29,
	kKeyAnf3 = 0x2a,
	kKeyOn = 0x2b,
	kKeyGreen = 0x2c,
	kKeyCyan = 0x2d,
	kKeyOff = 0x2e,
	kKeyLightEngine = 0x2f,
	kKeyBlack = 0x30,
	kKeyB = 0x31,
	kKeyAnf2 = 0x32,
	kKeyAll = 0x33,
	kKeyCeilingFront = 0x34,
	kKeyI2cBlue = 0x35,
	kKeyG = 0x36,
	kKeyClear = 0x37,
	kKeyOvp921 = 0x38,
	kKeyMinute = 0x39,
	kKeySize = 0x3a,
	kKeyTime = 0x3b,
	kKeySecond = 0x3c,
	kKeyR = 0x3d,
	kKeyVerticalRamp = 0x3e,
	kKeyIdle = 0x3f,
} aeKeyName;

extern unsigned short g_atkey_num;
extern asKeyListUnit gsKeyList[];
extern uint16_t gSkey_list_len;


#ifdef __cplusplus
}
#endif

#endif
