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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {

            gpio_bit_set(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            if (get_sig(sys_sig, ovp921_status) != true) // if ovp921 is not working, then reset it
            {
                gpio_bit_set(OVP921_RESET_PORT, OVP921_RESET_PIN);
                vTaskDelay(1000);
                gpio_bit_reset(OVP921_RESET_PORT, OVP921_RESET_PIN);
            }
            debug_printf("system on\n");
            set_sig(sys_sig, at_lightsource,true);
            debug_printf("at_lightsource on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            laser_off();
            clear_sig(sys_sig, at_lightsource);
            debug_printf("at_lightsource off\n");
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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            set_sig(sys_sig, at_lightsource,true);
            debug_printf("at_lightsource on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            clear_sig(sys_sig, at_lightsource);
            debug_printf("at_lightsource off\n");
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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        debug_printf("system value error\n");
    }
    else
    {
        if (kKeyMcuSoftware == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, MCU_VERSION "\n");
            update_anf_ovp2200_921_pgen_v4_05();
        }
    }
}

IAtOperationRegister(kCmdLightSourceTime, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        debug_printf("system value error\n");
    }
    else
    {
        if (kKeyMinute == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "Minute:%d\n", eeprom.light_source_time);
        }
    }
}

IAtOperationRegister(kCmdInstallationMode, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

IAtOperationRegister(kCmdVerticalKeystone, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

IAtOperationRegister(kCmdHorizonKeystone, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

IAtOperationRegister(kCmdKeystone2D, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

IAtOperationRegister(kCmdTestPattern, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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
        else if (kKeyMegenta == my_kvs[0].value)
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
        else if (kKeyCheckErboard == my_kvs[0].value)
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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

IAtOperationRegister(kCmdEeprom, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

IAtOperationRegister(kCmdSetCurrent, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

IAtOperationRegister(kCmdGetCurrent, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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

    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

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
