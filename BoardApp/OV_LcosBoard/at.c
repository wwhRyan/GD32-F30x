/**
 * @file at.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-02-15
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */
#include "main.h"
#include "AtProtocol.h"
#include "basicApp.h"
#include "ovp921.h"

extern asAtProtocol at_obj;

// testcase AT+System=On
IAtOperationRegister(kCmdSystem, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {

            gpio_bit_set(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            if (get_sig(sys_sig, sig_ovp921_status) != true) // if ovp921 is not working, then reset it
            {
                gpio_bit_set(OVP921_RESET_PORT, OVP921_RESET_PIN);
                vTaskDelay(1000);
                gpio_bit_reset(OVP921_RESET_PORT, OVP921_RESET_PIN);
            }
            debug_printf("system on\n");
            set_sig(sys_sig, sig_lightsource, true);
            set_sig(sys_sig, sig_system, true);
            debug_printf("sig_lightsource on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            laser_off();
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            debug_printf("sig_lightsource off\n");
            gpio_bit_reset(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            if (gpio_output_bit_get(SYS_12V_ON_PORT, SYS_12V_ON_PIN))
            {
                IAddFeedbackStrTo(pAt_feedback_str, "On\n");
            }
            else
            {
                IAddFeedbackStrTo(pAt_feedback_str, "Off\n");
            }
        }
    }
}

IAtOperationRegister(kCmdLightSource, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            set_sig(sys_sig, sig_lightsource, true);
            debug_printf("sig_lightsource on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            clear_sig(sys_sig, sig_lightsource);
            debug_printf("sig_lightsource off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            if (gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN))
            {
                IAddFeedbackStrTo(pAt_feedback_str, "On\n");
            }
            else
            {
                IAddFeedbackStrTo(pAt_feedback_str, "Off\n");
            }
        }
    }
}

IAtOperationRegister(kCmdVersion, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    char version_str[32] = {0};
    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        debug_printf("system value error\n");
    }
    else
    {
        if (kKeyMcu == my_kvs[0].value)
        {
            debug_printf("kKeyMcu\n");
            IAddFeedbackStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList->key.pData, MCU_VERSION);
        }
        else if (kKeyAnf1 == my_kvs[0].value)
        {
            debug_printf("kKeyAnf1\n");
            get_anf_version(version_str, 1);
            IAddFeedbackStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList->key.pData, version_str);
        }
        else if (kKeyAnf2 == my_kvs[0].value)
        {
            debug_printf("kKeyAnf2\n");
            get_anf_version(version_str, 2);
            IAddFeedbackStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList->key.pData, version_str);
        }
        else if (kKeyOvp921 == my_kvs[0].value)
        {
            debug_printf("kKeyOvp921\n");
            get_firmware_version(version_str);
            IAddFeedbackStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList->key.pData, version_str);
        }
    }
}

IAtOperationRegister(kCmdLightSourceTime, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        debug_printf("system value error\n");
    }
    else
    {
        if (kKeyMinute == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "Minute:%d\n", eeprom.light_source_time / 60);
        }
    }
}

IAtOperationRegister(kCmdInstallationMode, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyCeilingFront == my_kvs[0].value)
        {

            horizontal_flip(true);
            vertical_flip(false);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyCeilingRear == my_kvs[0].value)
        {
            horizontal_flip(false);
            vertical_flip(false);

            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyTableFront == my_kvs[0].value)
        {
            horizontal_flip(true);
            vertical_flip(true);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyTableRear == my_kvs[0].value)
        {
            horizontal_flip(false);
            vertical_flip(true);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdTestPattern, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyRed == my_kvs[0].value)
        {
            show_solid_color_pattern(255, 0, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyGreen == my_kvs[0].value)
        {
            show_solid_color_pattern(0, 255, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyBlue == my_kvs[0].value)
        {
            show_solid_color_pattern(0, 0, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyGrey == my_kvs[0].value)
        {
            show_solid_color_pattern(127, 127, 127);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyMagenta == my_kvs[0].value)
        {
            show_solid_color_pattern(255, 0, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyCyan == my_kvs[0].value)
        {
            show_solid_color_pattern(0, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyYellow == my_kvs[0].value)
        {
            show_solid_color_pattern(255, 255, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyBlack == my_kvs[0].value)
        {
            show_solid_color_pattern(0, 0, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyWhite == my_kvs[0].value)
        {
            show_solid_color_pattern(255, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyCheckerboard == my_kvs[0].value)
        {
            checkerboard_pattern();
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyHorizontalRamp == my_kvs[0].value)
        {
            gray_ramp_pattern();
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }

        else if (kKeyOff == my_kvs[0].value)
        {
            off_pattern();
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdTemperature, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdCwSpeed, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdReset, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdError, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdLogInfo, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdCurrent, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdSn, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}
