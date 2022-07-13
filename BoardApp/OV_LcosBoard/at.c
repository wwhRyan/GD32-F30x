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
#include "utilsAsciiConvert.h"

extern asAtProtocol at_obj;
extern const unit_t eeprom_msg[];

// testcase AT+System=On
IAtOperationRegister(kCmdSystem, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        switch (my_kvs[0].value)
        {
        case kKeyOn:
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
            break;

        case kKeyIdle:
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            clear_sig(sys_sig, sig_update_anf);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
            break;

        case kKeyOff:
            debug_printf("system off\n");
            laser_off();
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            clear_sig(sys_sig, sig_update_anf);
            debug_printf("sig_lightsource off\n");
            gpio_bit_reset(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
            break;

        default:
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
            break;
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
        else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
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
        else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
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
        else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
}

IAtOperationRegister(kCmdVersion, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);
    char str_buff[32] = {0};
    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        debug_printf("no support setting!\n");
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    else
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyMcu:
                debug_printf("kKeyMcu\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, MCU_VERSION);
                break;
            case kKeyLightEngineBoard:
                debug_printf("kKeyLightEngineBoard\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:VER1%01X.VER0%01X\n", pAt_Kv_List->pList[i].key.pData,
                                  gpio_output_bit_get(HW_VER1_PORT, HW_VER1_PIN), gpio_output_bit_get(HW_VER0_PORT, HW_VER0_PIN));
                break;
            case kKeyAnf1:
                debug_printf("kKeyAnf1\n");
                get_anf_version(str_buff, 1);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            case kKeyAnf2:
                debug_printf("kKeyAnf2\n");
                get_anf_version(str_buff, 2);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            case kKeyAnf3:
                debug_printf("kKeyAnf3\n");
                get_anf_version(str_buff, 3);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            case kKeyOvp921:
                debug_printf("kKeyOvp921\n");
                get_firmware_version(str_buff);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }
    }
}

IAtOperationRegister(kCmdSn, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);
    char str_buff[32] = {0};

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    else
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyLightEngine:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n",
                                  pAt_Kv_List->pList[i].key.pData, get_sn(0, str_buff));
                break;
            case kKeySourceLight:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n",
                                  pAt_Kv_List->pList[i].key.pData, get_sn(1, str_buff));
                break;
            case kKeyProjector:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n",
                                  pAt_Kv_List->pList[i].key.pData, get_sn(2, str_buff));
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
}

IAtOperationRegister(kCmdLightSourceTime, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        debug_printf("no support setting!\n");
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    else
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyMinute:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.light_source_time / 60);
                break;
            case kKeyHour:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.light_source_time / 60 / 60);
                break;
            case kKeySecond:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.light_source_time);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
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
        else
        {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    }
    else
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
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
        else
        {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    }
    else
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdTemperature, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    else
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyNtcBlue:
                // todo
                break;
            case kKeyNtcRed:
                // todo
                break;
            case kKeyNtcGreen:
                // todo
                break;
            case kKeyNtcLcos:
                // todo
                break;
            case kKeyNtcEnv:
                // todo
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
}

IAtOperationRegister(kCmdCwSpeed, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[1];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    else
    {
        if (kKeyScatteringWheel == my_kvs[0].key)
        {
            // todo
        }
        else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
}

IAtOperationRegister(kCmdReset, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyFactory == my_kvs[0].value)
        {
            // todo
        }
        else if (kKeyUser == my_kvs[0].value)
        {
            // todo
        }
    }
    else
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdError, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[1];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
    else
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
}

IAtOperationRegister(kCmdLogInfo, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Int my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueInt, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            // todo
        }
        else if (kKeyClear == my_kvs[0].key)
        {
            // todo
        }
        else
        {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    }
}

IAtOperationRegister(kCmdCurrent, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        float current;

        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyR:
                sscanf(my_kvs[i].value, "%f", &current);
                // check current
                eeprom.red = current;
                xQueueSend(xQueue_eeprom, (void *)&eeprom_msg[idx_red], (TickType_t)10);
                break;
            case kKeyG:
                sscanf(my_kvs[i].value, "%f", &current);
                // check current
                eeprom.green = current;
                xQueueSend(xQueue_eeprom, (void *)&eeprom_msg[idx_green], (TickType_t)10);
                break;
            case kKeyB:
                sscanf(my_kvs[i].value, "%f", &current);
                // check current
                eeprom.blue = current;
                xQueueSend(xQueue_eeprom, (void *)&eeprom_msg[idx_blue], (TickType_t)10);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyR:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%f\n",
                                  pAt_Kv_List->pList[i].key.pData, eeprom.red);
                break;
            case kKeyG:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%f\n",
                                  pAt_Kv_List->pList[i].key.pData, eeprom.green);
                break;
            case kKeyB:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%f\n",
                                  pAt_Kv_List->pList[i].key.pData, eeprom.blue);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
}

IAtOperationRegister(kCmdUpgradeOvp921Anf, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    size_t anf_idx = 0;
    uint32_t at_crc = 0;
    uint8_t anf_data[256] = {0};
    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyIdx:
                sscanf(my_kvs[i].value, "%X", &anf_idx);
                if (anf_idx > 9 || anf_idx == 0)
                    goto VALUE_ERROR;
                break;
            case kKeyCrc:
                sscanf(my_kvs[i].value, "%X", &at_crc);
                debug_printf("at_crc: %#x\n", at_crc);
                break;
            case kKeyData:
                if (512 != strlen(my_kvs[i].value))
                    goto VALUE_ERROR;
                if (false == AsciiToInt(my_kvs[i].value, anf_data, 1))
                    goto VALUE_ERROR;
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }

        if (at_crc != get_array_crc(anf_data, sizeof(anf_data)))
            goto VALUE_ERROR;

        clear_sig(sys_sig, sig_system);

        if (!update_anf(anf_idx, anf_data, sizeof(anf_data)))
        {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
            clear_sig(sys_sig, sig_update_anf);
            return;
        }
        else
        {
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            return;
        }

    VALUE_ERROR:
        clear_sig(sys_sig, sig_update_anf);
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
        return;
    }
    else
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdUpgradeOvp921Firmware, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyCrc:
                // todo
                break;
            case kKeyData:
                // todo
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
    else
    {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdOvp921, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyAddr:
                // todo
                break;
            case kKeySize:
                // todo
                break;
            case kKeyData:
                // todo
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyAddr:
                // todo
                break;
            case kKeySize:
                // todo
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
}

IAtOperationRegister(kCmdEeprom, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyAddr:
                // todo
                break;
            case kKeySize:
                // todo
                break;
            case kKeyData:
                // todo
                break;
            case kKeyClear:
                // todo
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < pAt_Kv_List->size; i++)
        {
            switch (my_kvs[i].key)
            {
            case kKeyAddr:
                // todo
                break;
            case kKeySize:
                // todo
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                break;
            }
        }
    }
}

IAtOperationRegister(kCmdSilentAsyncMessages, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            // todo
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            // todo
        }
    }
    else
    {
        // todo
    }
}
