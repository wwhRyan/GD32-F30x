#include "AtKeyHashTable.h"

unsigned short g_atkey_num = 64;		/* AtKey Num */
asKeyListUnit gsKeyList[] = {
/*00*/{ 0X1555835B, 0 ,"ScatteringWheel"}, /* ScatteringWheel */
/*01*/{ 0X25AC9D84, 0 ,"Yellow"}, /* Yellow */
/*02*/{ 0X5B90A011, 0 ,"Empty"}, /* Empty */
/*03*/{ 0X000FD490, 0 ,"Ok"}, /* Ok */
/*04*/{ 0X2B587A6E, 0 ,"Grid"}, /* Grid */
/*05*/{ 0X6A06EB4B, 0 ,"I2cEnv"}, /* I2cEnv */
/*06*/{ 0X00000059, 0 ,"Y"}, /* Y */
/*07*/{ 0X1E906396, 0 ,"Factory"}, /* Factory */
/*08*/{ 0X0B319851, 0 ,"I2cLcos"}, /* I2cLcos */
/*09*/{ 0X2EBACD5E, 0 ,"Blue"}, /* Blue */
/*10*/{ 0X3FD10810, 0 ,"Minute"}, /* Minute */
/*11*/{ 0X40F3631C, 0 ,"CeilingFront"}, /* CeilingFront */
/*12*/{ 0X1A181EEB, 0 ,"Black"}, /* Black */
/*13*/{ 0X00000042, 0 ,"B"}, /* B */
/*14*/{ 0X3BE53314, 0 ,"NtcLcos"}, /* NtcLcos */
/*15*/{ 0X4420343C, 0 ,"LightEngine"}, /* LightEngine */
/*16*/{ 0X40B578D2, 0 ,"TableRear"}, /* TableRear */
/*17*/{ 0X1D147AC5, 0 ,"Anf2"}, /* Anf2 */
/*18*/{ 0X3E75857F, 0 ,"SourceLight"}, /* SourceLight */
/*19*/{ 0X53639341, 0 ,"Magenta"}, /* Magenta */
/*20*/{ 0X18300877, 0 ,"TableFront"}, /* TableFront */
/*21*/{ 0X240FC8A4, 0 ,"NtcRed"}, /* NtcRed */
/*22*/{ 0X1D147AC4, 0 ,"Anf1"}, /* Anf1 */
/*23*/{ 0X32B73B08, 0 ,"Hour"}, /* Hour */
/*24*/{ 0X38B57CA1, 0 ,"CeilingRear"}, /* CeilingRear */
/*25*/{ 0X2B57AD57, 0 ,"Grey"}, /* Grey */
/*26*/{ 0X4ACFAABD, 0 ,"Red"}, /* Red */
/*27*/{ 0X67DA0698, 0 ,"Idle"}, /* Idle */
/*28*/{ 0X6F9FB53B, 0 ,"I2cRed"}, /* I2cRed */
/*29*/{ 0X4C503293, 0 ,"User"}, /* User */
/*30*/{ 0X25EB3076, 0 ,"LightEngineBoard"}, /* LightEngineBoard */
/*31*/{ 0X0BBED600, 0 ,"Eeprom"}, /* Eeprom */
/*32*/{ 0X0A107442, 0 ,"Data"}, /* Data */
/*33*/{ 0X176C6C5B, 0 ,"Mcu"}, /* Mcu */
/*34*/{ 0X00592743, 0 ,"Ovp921"}, /* Ovp921 */
/*35*/{ 0X1D147AC6, 0 ,"Anf3"}, /* Anf3 */
/*36*/{ 0X30A9BDF4, 0 ,"Crc"}, /* Crc */
/*37*/{ 0X192C360D, 0 ,"Size"}, /* Size */
/*38*/{ 0X546C1938, 0 ,"Rdc200a"}, /* Rdc200a */
/*39*/{ 0X51349156, 0 ,"Status"}, /* Status */
/*40*/{ 0X5B4D76E1, 0 ,"White"}, /* White */
/*41*/{ 0X5957B811, 0 ,"Checkerboard"}, /* Checkerboard */
/*42*/{ 0X69C4FA48, 0 ,"I2cBlue"}, /* I2cBlue */
/*43*/{ 0X43920871, 0 ,"Clear"}, /* Clear */
/*44*/{ 0X0C9AA399, 0 ,"Rdp250h"}, /* Rdp250h */
/*45*/{ 0X2BFAF61F, 0 ,"Off"}, /* Off */
/*46*/{ 0X000FD493, 0 ,"On"}, /* On */
/*47*/{ 0X1E76FEB4, 0 ,"NtcEnv"}, /* NtcEnv */
/*48*/{ 0X00000047, 0 ,"G"}, /* G */
/*49*/{ 0X1C1A9A49, 0 ,"All"}, /* All */
/*50*/{ 0X16413095, 0 ,"Text"}, /* Text */
/*51*/{ 0X00000052, 0 ,"R"}, /* R */
/*52*/{ 0X099512BE, 0 ,"NtcGreen"}, /* NtcGreen */
/*53*/{ 0X2A341C0F, 0 ,"Green"}, /* Green */
/*54*/{ 0X7D21E29D, 0 ,"I2cGreen"}, /* I2cGreen */
/*55*/{ 0X6E50BFC5, 0 ,"Idx"}, /* Idx */
/*56*/{ 0X364E64B5, 0 ,"Addr"}, /* Addr */
/*57*/{ 0X213413B4, 0 ,"Projector"}, /* Projector */
/*58*/{ 0X5A82A423, 0 ,"Cyan"}, /* Cyan */
/*59*/{ 0X1A78950B, 0 ,"NtcBlue"}, /* NtcBlue */
/*60*/{ 0X0E33430E, 0 ,"HorizontalRamp"}, /* HorizontalRamp */
/*61*/{ 0X64AA4164, 0 ,"Second"}, /* Second */
/*62*/{ 0X00A64EE4, 0 ,"VerticalRamp"}, /* VerticalRamp */
/*63*/{ 0X3F5ADC31, 0 ,"Time"}, /* Time */
};
uint16_t gSkey_list_len = sizeof(gsKeyList) / sizeof(asKeyListUnit);
