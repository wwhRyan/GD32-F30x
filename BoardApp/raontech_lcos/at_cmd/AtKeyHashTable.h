#ifndef __ATKEYHASHTABLE_H__
#define __ATKEYHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef enum __aeKeyName{
	kKeyHour = 0x0,
	kKeyUser = 0x1,
	kKeyBlack = 0x2,
	kKeyNtcGreen = 0x3,
	kKeyLightEngine = 0x4,
	kKeyCeilingRear = 0x5,
	kKeyAnf3 = 0x6,
	kKeyTableRear = 0x7,
	kKeyScatteringWheel = 0x8,
	kKeyAnf1 = 0x9,
	kKeyAnf2 = 0xa,
	kKeyCeilingFront = 0xb,
	kKeyOn = 0xc,
	kKeyStatus = 0xd,
	kKeyWhite = 0xe,
	kKeyFactory = 0xf,
	kKeyOvp921 = 0x10,
	kKeyCheckerboard = 0x11,
	kKeyIdle = 0x12,
	kKeyGrid = 0x13,
	kKeyNtcBlue = 0x14,
	kKeyCrc = 0x15,
	kKeyBlue = 0x16,
	kKeyAddr = 0x17,
	kKeyGrey = 0x18,
	kKeyHorizontalRamp = 0x19,
	kKeyOff = 0x1a,
	kKeyNtcRed = 0x1b,
	kKeyNtcEnv = 0x1c,
	kKeySecond = 0x1d,
	kKeyEmpty = 0x1e,
	kKeyTableFront = 0x1f,
	kKeyMinute = 0x20,
	kKeyGreen = 0x21,
	kKeyLightEngineBoard = 0x22,
	kKeyAll = 0x23,
	kKeyIdx = 0x24,
	kKeyG = 0x25,
	kKeyR = 0x26,
	kKeyNtcLcos = 0x27,
	kKeyData = 0x28,
	kKeySize = 0x29,
	kKeyEeprom = 0x2a,
	kKeyY = 0x2b,
	kKeyMcu = 0x2c,
	kKeyOk = 0x2d,
	kKeyClear = 0x2e,
	kKeyYellow = 0x2f,
	kKeyMagenta = 0x30,
	kKeySourceLight = 0x31,
	kKeyVerticalRamp = 0x32,
	kKeyProjector = 0x33,
	kKeyB = 0x34,
	kKeyCyan = 0x35,
	kKeyRed = 0x36,
} aeKeyName;

extern unsigned short g_atkey_num;
extern asKeyListUnit gsKeyList[];
extern uint16_t gSkey_list_len;


#ifdef __cplusplus
}
#endif

#endif
