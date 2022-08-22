#include "AtCmdHashTable.h"

unsigned short g_atcmd_num = 18;		/* AtCmd Num */
asCmdListUnit gsCmdList[] = {
/*00*/	{ 0X2D00BBF3, NULL ,"CwSpeed"}, /* CwSpeed */
/*01*/	{ 0X4C58DF1A, NULL ,"LightSourceTime"}, /* LightSourceTime */
/*02*/	{ 0X00592743, NULL ,"Ovp921"}, /* Ovp921 */
/*03*/	{ 0X4DA43177, NULL ,"Reset"}, /* Reset */
/*04*/	{ 0X02801EAB, NULL ,"System"}, /* System */
/*05*/	{ 0X2F13D444, NULL ,"Error"}, /* Error */
/*06*/	{ 0X095B4C06, NULL ,"UpgradeOvp921Firmware"}, /* UpgradeOvp921Firmware */
/*07*/	{ 0X0BBED600, NULL ,"Eeprom"}, /* Eeprom */
/*08*/	{ 0X786E80BE, NULL ,"UpgradeOvp921Anf"}, /* UpgradeOvp921Anf */
/*09*/	{ 0X7CBE16F2, NULL ,"LogInfo"}, /* LogInfo */
/*10*/	{ 0X1FED8839, NULL ,"LightSource"}, /* LightSource */
/*11*/	{ 0X08563BBC, NULL ,"Version"}, /* Version */
/*12*/	{ 0X2CCB5EC5, NULL ,"Current"}, /* Current */
/*13*/	{ 0X030B85E1, NULL ,"InstallationMode"}, /* InstallationMode */
/*14*/	{ 0X73BFD97F, NULL ,"SilentAsyncMessages"}, /* SilentAsyncMessages */
/*15*/	{ 0X5BA10D06, NULL ,"TestPattern"}, /* TestPattern */
/*16*/	{ 0X0010A1BF, NULL ,"Sn"}, /* Sn */
/*17*/	{ 0X41B4E980, NULL ,"Temperature"}, /* Temperature */
};
uint16_t gScmd_list_len = sizeof(gsCmdList) / sizeof(asCmdListUnit);
