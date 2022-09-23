#include "AtCmdHashTable.h"

unsigned short g_atcmd_num = 23;		/* AtCmd Num */
asCmdListUnit gsCmdList[] = {
/*00*/	{ 0X02801EAB, NULL ,"System"}, /* System */
/*01*/	{ 0X1FED8839, NULL ,"LightSource"}, /* LightSource */
/*02*/	{ 0X786E80BE, NULL ,"UpgradeOvp921Anf"}, /* UpgradeOvp921Anf */
/*03*/	{ 0X546C1938, NULL ,"Rdc200a"}, /* Rdc200a */
/*04*/	{ 0X4668F118, NULL ,"SpiFlash"}, /* SpiFlash */
/*05*/	{ 0X5BA10D06, NULL ,"TestPattern"}, /* TestPattern */
/*06*/	{ 0X2F13D444, NULL ,"Error"}, /* Error */
/*07*/	{ 0X0BBED600, NULL ,"Eeprom"}, /* Eeprom */
/*08*/	{ 0X7CBE16F2, NULL ,"LogInfo"}, /* LogInfo */
/*09*/	{ 0X030B85E1, NULL ,"InstallationMode"}, /* InstallationMode */
/*10*/	{ 0X73BFD97F, NULL ,"SilentAsyncMessages"}, /* SilentAsyncMessages */
/*11*/	{ 0X4DCCF208, NULL ,"FlashBootb"}, /* FlashBootb */
/*12*/	{ 0X0C9AA399, NULL ,"Rdp250h"}, /* Rdp250h */
/*13*/	{ 0X00592743, NULL ,"Ovp921"}, /* Ovp921 */
/*14*/	{ 0X4C58DF1A, NULL ,"LightSourceTime"}, /* LightSourceTime */
/*15*/	{ 0X6BABF562, NULL ,"SpiFlashErase"}, /* SpiFlashErase */
/*16*/	{ 0X095B4C06, NULL ,"UpgradeOvp921Firmware"}, /* UpgradeOvp921Firmware */
/*17*/	{ 0X0010A1BF, NULL ,"Sn"}, /* Sn */
/*18*/	{ 0X4DA43177, NULL ,"Reset"}, /* Reset */
/*19*/	{ 0X08563BBC, NULL ,"Version"}, /* Version */
/*20*/	{ 0X2CCB5EC5, NULL ,"Current"}, /* Current */
/*21*/	{ 0X2D00BBF3, NULL ,"CwSpeed"}, /* CwSpeed */
/*22*/	{ 0X41B4E980, NULL ,"Temperature"}, /* Temperature */
};
uint16_t gScmd_list_len = sizeof(gsCmdList) / sizeof(asCmdListUnit);
