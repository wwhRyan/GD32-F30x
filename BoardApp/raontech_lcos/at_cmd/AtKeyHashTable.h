#ifndef __ATKEYHASHTABLE_H__
#define __ATKEYHASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/At_internal/AtInternalConfig.h"
typedef enum __aeKeyName{
	kKeyEeprom = 0x0,
	kKeyStatus = 0x1,
	kKeyMinute = 0x2,
	kKeyIdx = 0x3,
	kKeyMcu = 0x4,
	kKeyEmpty = 0x5,
	kKeyYellow = 0x6,
	kKeySize = 0x7,
	kKeyCyan = 0x8,
	kKeyScatteringWheel = 0x9,
	kKeyTableRear = 0xa,
	kKeyAnf3 = 0xb,
	kKeyTableFront = 0xc,
	kKeyNtcGreen = 0xd,
	kKeyFactory = 0xe,
	kKeyCheckerboard = 0xf,
	kKeyGrid = 0x10,
	kKeyBlue = 0x11,
	kKeyData = 0x12,
	kKeyNtcBlue = 0x13,
	kKeyNtcRed = 0x14,
	kKeyRdc200a = 0x15,
	kKeyHorizontalRamp = 0x16,
	kKeyRdp250h = 0x17,
	kKeyLightEngineBoard = 0x18,
	kKeyLightEngine = 0x19,
	kKeyVerticalRamp = 0x1a,
	kKeyCrc = 0x1b,
	kKeyCeilingRear = 0x1c,
	kKeyRed = 0x1d,
	kKeyNtcLcos = 0x1e,
	kKeyUser = 0x1f,
	kKeyOvp921 = 0x20,
	kKeyR = 0x21,
	kKeyMagenta = 0x22,
	kKeyGrey = 0x23,
	kKeyOn = 0x24,
	kKeyCeilingFront = 0x25,
	kKeyGreen = 0x26,
	kKeyProjector = 0x27,
	kKeyHour = 0x28,
	kKeyBlack = 0x29,
	kKeyG = 0x2a,
	kKeyB = 0x2b,
	kKeyAll = 0x2c,
	kKeyAnf2 = 0x2d,
	kKeyWhite = 0x2e,
	kKeySourceLight = 0x2f,
	kKeyAnf1 = 0x30,
	kKeyAddr = 0x31,
	kKeySecond = 0x32,
	kKeyIdle = 0x33,
	kKeyOk = 0x34,
	kKeyOff = 0x35,
	kKeyY = 0x36,
	kKeyClear = 0x37,
	kKeyNtcEnv = 0x38,
} aeKeyName;

extern unsigned short g_atkey_num;
extern asKeyListUnit gsKeyList[];
extern uint16_t gSkey_list_len;


#ifdef __cplusplus
}
#endif

#endif
