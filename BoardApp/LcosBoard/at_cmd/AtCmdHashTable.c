#include "AtCmdHashTable.h"

unsigned short g_atcmd_num = 23;		/* AtCmd Num */
asCmdListUnit gsCmdList[] = {
/*00*/	{ 0X0010A1BF, NULL ,"Sn"}, /* Sn */
/*01*/	{ 0X00592743, NULL ,"Ovp921"}, /* Ovp921 */
/*02*/	{ 0X02801EAB, NULL ,"System"}, /* System */
/*03*/	{ 0X030B85E1, NULL ,"InstallationMode"}, /* InstallationMode */
/*04*/	{ 0X08563BBC, NULL ,"Version"}, /* Version */
/*05*/	{ 0X095B4C06, NULL ,"UpgradeOvp921Firmware"}, /* UpgradeOvp921Firmware */
/*06*/	{ 0X0BBED600, NULL ,"Eeprom"}, /* Eeprom */
/*07*/	{ 0X0C9AA399, NULL ,"Rdp250h"}, /* Rdp250h */
/*08*/	{ 0X1FED8839, NULL ,"LightSource"}, /* LightSource */
/*09*/	{ 0X2CCB5EC5, NULL ,"Current"}, /* Current */
/*10*/	{ 0X2D00BBF3, NULL ,"CwSpeed"}, /* CwSpeed */
/*11*/	{ 0X2F13D444, NULL ,"Error"}, /* Error */
/*12*/	{ 0X41B4E980, NULL ,"Temperature"}, /* Temperature */
/*13*/	{ 0X4668F118, NULL ,"SpiFlash"}, /* SpiFlash */
/*14*/	{ 0X4C58DF1A, NULL ,"LightSourceTime"}, /* LightSourceTime */
/*15*/	{ 0X4DA43177, NULL ,"Reset"}, /* Reset */
/*16*/	{ 0X4DCCF208, NULL ,"FlashBootb"}, /* FlashBootb */
/*17*/	{ 0X546C1938, NULL ,"Rdc200a"}, /* Rdc200a */
/*18*/	{ 0X5BA10D06, NULL ,"TestPattern"}, /* TestPattern */
/*19*/	{ 0X6BABF562, NULL ,"SpiFlashErase"}, /* SpiFlashErase */
/*20*/	{ 0X73BFD97F, NULL ,"SilentAsyncMessages"}, /* SilentAsyncMessages */
/*21*/	{ 0X786E80BE, NULL ,"UpgradeOvp921Anf"}, /* UpgradeOvp921Anf */
/*22*/	{ 0X7CBE16F2, NULL ,"LogInfo"}, /* LogInfo */
};
uint16_t gScmd_list_len = sizeof(gsCmdList) / sizeof(asCmdListUnit);
