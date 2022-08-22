#include "AtKeyHashTable.h"

unsigned short g_atkey_num = 55;		/* AtKey Num */
asKeyListUnit gsKeyList[] = {
/*00*/{ 0X32B73B08, 0 ,"Hour"}, /* Hour */
/*01*/{ 0X4C503293, 0 ,"User"}, /* User */
/*02*/{ 0X1A181EEB, 0 ,"Black"}, /* Black */
/*03*/{ 0X099512BE, 0 ,"NtcGreen"}, /* NtcGreen */
/*04*/{ 0X4420343C, 0 ,"LightEngine"}, /* LightEngine */
/*05*/{ 0X38B57CA1, 0 ,"CeilingRear"}, /* CeilingRear */
/*06*/{ 0X1D147AC6, 0 ,"Anf3"}, /* Anf3 */
/*07*/{ 0X40B578D2, 0 ,"TableRear"}, /* TableRear */
/*08*/{ 0X1555835B, 0 ,"ScatteringWheel"}, /* ScatteringWheel */
/*09*/{ 0X1D147AC4, 0 ,"Anf1"}, /* Anf1 */
/*10*/{ 0X1D147AC5, 0 ,"Anf2"}, /* Anf2 */
/*11*/{ 0X40F3631C, 0 ,"CeilingFront"}, /* CeilingFront */
/*12*/{ 0X000FD493, 0 ,"On"}, /* On */
/*13*/{ 0X51349156, 0 ,"Status"}, /* Status */
/*14*/{ 0X5B4D76E1, 0 ,"White"}, /* White */
/*15*/{ 0X1E906396, 0 ,"Factory"}, /* Factory */
/*16*/{ 0X00592743, 0 ,"Ovp921"}, /* Ovp921 */
/*17*/{ 0X5957B811, 0 ,"Checkerboard"}, /* Checkerboard */
/*18*/{ 0X67DA0698, 0 ,"Idle"}, /* Idle */
/*19*/{ 0X2B587A6E, 0 ,"Grid"}, /* Grid */
/*20*/{ 0X1A78950B, 0 ,"NtcBlue"}, /* NtcBlue */
/*21*/{ 0X30A9BDF4, 0 ,"Crc"}, /* Crc */
/*22*/{ 0X2EBACD5E, 0 ,"Blue"}, /* Blue */
/*23*/{ 0X364E64B5, 0 ,"Addr"}, /* Addr */
/*24*/{ 0X2B57AD57, 0 ,"Grey"}, /* Grey */
/*25*/{ 0X0E33430E, 0 ,"HorizontalRamp"}, /* HorizontalRamp */
/*26*/{ 0X2BFAF61F, 0 ,"Off"}, /* Off */
/*27*/{ 0X240FC8A4, 0 ,"NtcRed"}, /* NtcRed */
/*28*/{ 0X1E76FEB4, 0 ,"NtcEnv"}, /* NtcEnv */
/*29*/{ 0X64AA4164, 0 ,"Second"}, /* Second */
/*30*/{ 0X5B90A011, 0 ,"Empty"}, /* Empty */
/*31*/{ 0X18300877, 0 ,"TableFront"}, /* TableFront */
/*32*/{ 0X3FD10810, 0 ,"Minute"}, /* Minute */
/*33*/{ 0X2A341C0F, 0 ,"Green"}, /* Green */
/*34*/{ 0X25EB3076, 0 ,"LightEngineBoard"}, /* LightEngineBoard */
/*35*/{ 0X1C1A9A49, 0 ,"All"}, /* All */
/*36*/{ 0X6E50BFC5, 0 ,"Idx"}, /* Idx */
/*37*/{ 0X00000047, 0 ,"G"}, /* G */
/*38*/{ 0X00000052, 0 ,"R"}, /* R */
/*39*/{ 0X3BE53314, 0 ,"NtcLcos"}, /* NtcLcos */
/*40*/{ 0X0A107442, 0 ,"Data"}, /* Data */
/*41*/{ 0X192C360D, 0 ,"Size"}, /* Size */
/*42*/{ 0X0BBED600, 0 ,"Eeprom"}, /* Eeprom */
/*43*/{ 0X00000059, 0 ,"Y"}, /* Y */
/*44*/{ 0X176C6C5B, 0 ,"Mcu"}, /* Mcu */
/*45*/{ 0X000FD490, 0 ,"Ok"}, /* Ok */
/*46*/{ 0X43920871, 0 ,"Clear"}, /* Clear */
/*47*/{ 0X25AC9D84, 0 ,"Yellow"}, /* Yellow */
/*48*/{ 0X53639341, 0 ,"Magenta"}, /* Magenta */
/*49*/{ 0X3E75857F, 0 ,"SourceLight"}, /* SourceLight */
/*50*/{ 0X00A64EE4, 0 ,"VerticalRamp"}, /* VerticalRamp */
/*51*/{ 0X213413B4, 0 ,"Projector"}, /* Projector */
/*52*/{ 0X00000042, 0 ,"B"}, /* B */
/*53*/{ 0X5A82A423, 0 ,"Cyan"}, /* Cyan */
/*54*/{ 0X4ACFAABD, 0 ,"Red"}, /* Red */
};
uint16_t gSkey_list_len = sizeof(gsKeyList) / sizeof(asKeyListUnit);
