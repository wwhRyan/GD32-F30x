#include "AtKeyHashTable.h"

unsigned short g_atkey_num = 57;		/* AtKey Num */
asKeyListUnit gsKeyList[] = {
/*00*/{ 0X0BBED600, 0 ,"Eeprom"}, /* Eeprom */
/*01*/{ 0X51349156, 0 ,"Status"}, /* Status */
/*02*/{ 0X3FD10810, 0 ,"Minute"}, /* Minute */
/*03*/{ 0X6E50BFC5, 0 ,"Idx"}, /* Idx */
/*04*/{ 0X176C6C5B, 0 ,"Mcu"}, /* Mcu */
/*05*/{ 0X5B90A011, 0 ,"Empty"}, /* Empty */
/*06*/{ 0X25AC9D84, 0 ,"Yellow"}, /* Yellow */
/*07*/{ 0X192C360D, 0 ,"Size"}, /* Size */
/*08*/{ 0X5A82A423, 0 ,"Cyan"}, /* Cyan */
/*09*/{ 0X1555835B, 0 ,"ScatteringWheel"}, /* ScatteringWheel */
/*10*/{ 0X40B578D2, 0 ,"TableRear"}, /* TableRear */
/*11*/{ 0X1D147AC6, 0 ,"Anf3"}, /* Anf3 */
/*12*/{ 0X18300877, 0 ,"TableFront"}, /* TableFront */
/*13*/{ 0X099512BE, 0 ,"NtcGreen"}, /* NtcGreen */
/*14*/{ 0X1E906396, 0 ,"Factory"}, /* Factory */
/*15*/{ 0X5957B811, 0 ,"Checkerboard"}, /* Checkerboard */
/*16*/{ 0X2B587A6E, 0 ,"Grid"}, /* Grid */
/*17*/{ 0X2EBACD5E, 0 ,"Blue"}, /* Blue */
/*18*/{ 0X0A107442, 0 ,"Data"}, /* Data */
/*19*/{ 0X1A78950B, 0 ,"NtcBlue"}, /* NtcBlue */
/*20*/{ 0X240FC8A4, 0 ,"NtcRed"}, /* NtcRed */
/*21*/{ 0X546C1938, 0 ,"Rdc200a"}, /* Rdc200a */
/*22*/{ 0X0E33430E, 0 ,"HorizontalRamp"}, /* HorizontalRamp */
/*23*/{ 0X0C9AA399, 0 ,"Rdp250h"}, /* Rdp250h */
/*24*/{ 0X25EB3076, 0 ,"LightEngineBoard"}, /* LightEngineBoard */
/*25*/{ 0X4420343C, 0 ,"LightEngine"}, /* LightEngine */
/*26*/{ 0X00A64EE4, 0 ,"VerticalRamp"}, /* VerticalRamp */
/*27*/{ 0X30A9BDF4, 0 ,"Crc"}, /* Crc */
/*28*/{ 0X38B57CA1, 0 ,"CeilingRear"}, /* CeilingRear */
/*29*/{ 0X4ACFAABD, 0 ,"Red"}, /* Red */
/*30*/{ 0X3BE53314, 0 ,"NtcLcos"}, /* NtcLcos */
/*31*/{ 0X4C503293, 0 ,"User"}, /* User */
/*32*/{ 0X00592743, 0 ,"Ovp921"}, /* Ovp921 */
/*33*/{ 0X00000052, 0 ,"R"}, /* R */
/*34*/{ 0X53639341, 0 ,"Magenta"}, /* Magenta */
/*35*/{ 0X2B57AD57, 0 ,"Grey"}, /* Grey */
/*36*/{ 0X000FD493, 0 ,"On"}, /* On */
/*37*/{ 0X40F3631C, 0 ,"CeilingFront"}, /* CeilingFront */
/*38*/{ 0X2A341C0F, 0 ,"Green"}, /* Green */
/*39*/{ 0X213413B4, 0 ,"Projector"}, /* Projector */
/*40*/{ 0X32B73B08, 0 ,"Hour"}, /* Hour */
/*41*/{ 0X1A181EEB, 0 ,"Black"}, /* Black */
/*42*/{ 0X00000047, 0 ,"G"}, /* G */
/*43*/{ 0X00000042, 0 ,"B"}, /* B */
/*44*/{ 0X1C1A9A49, 0 ,"All"}, /* All */
/*45*/{ 0X1D147AC5, 0 ,"Anf2"}, /* Anf2 */
/*46*/{ 0X5B4D76E1, 0 ,"White"}, /* White */
/*47*/{ 0X3E75857F, 0 ,"SourceLight"}, /* SourceLight */
/*48*/{ 0X1D147AC4, 0 ,"Anf1"}, /* Anf1 */
/*49*/{ 0X364E64B5, 0 ,"Addr"}, /* Addr */
/*50*/{ 0X64AA4164, 0 ,"Second"}, /* Second */
/*51*/{ 0X67DA0698, 0 ,"Idle"}, /* Idle */
/*52*/{ 0X000FD490, 0 ,"Ok"}, /* Ok */
/*53*/{ 0X2BFAF61F, 0 ,"Off"}, /* Off */
/*54*/{ 0X00000059, 0 ,"Y"}, /* Y */
/*55*/{ 0X43920871, 0 ,"Clear"}, /* Clear */
/*56*/{ 0X1E76FEB4, 0 ,"NtcEnv"}, /* NtcEnv */
};
uint16_t gSkey_list_len = sizeof(gsKeyList) / sizeof(asKeyListUnit);
