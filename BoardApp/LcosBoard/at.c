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
#include "AtKeyHashTable.h"
#include "AtProtocol.h"
#include "BoardInit.h"
#include "basicApp.h"
#include "eeprom.h"
#include "main.h"
#include "ovp921.h"
#include "ulog.h"
#include "utilsAsciiConvert.h"
#include <string.h>

extern asAtProtocol at_obj;
extern const mem_t eeprom_mem[];

// testcase AT+System=On
IAtOperationRegister(kCmdSystem, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        switch (my_kvs[0].value) {
        case kKeyOn:
            gpio_bit_set(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            if (get_sig(sys_sig, sig_ovp921_status) != true) // if ovp921 is not working, then reset it
            {
                gpio_bit_set(OVP921_RESET_PORT, OVP921_RESET_PIN);
                vTaskDelay(100);
                gpio_bit_reset(OVP921_RESET_PORT, OVP921_RESET_PIN);
            }
            ULOG_DEBUG("system on\n");
            set_sig(sys_sig, sig_lightsource, true);
            set_sig(sys_sig, sig_system, true);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            break;

        case kKeyIdle:
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            clear_sig(sys_sig, sig_update_anf);
            clear_sig(sys_sig, sig_update_firmware);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            break;

        case kKeyOff:
            ULOG_DEBUG("system off\n");
            laser_off();
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            clear_sig(sys_sig, sig_update_anf);
            clear_sig(sys_sig, sig_update_firmware);
            clear_sig(sys_sig, sig_slient_async_msg);
            clear_sig(sys_sig, sig_ovp921_status);
            ULOG_DEBUG("sig_lightsource off\n");
            gpio_bit_reset(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            break;

        default:
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
            return;
        }
    } else {
        if (kKeyStatus == my_kvs[0].key) {
            if (gpio_output_bit_get(SYS_12V_ON_PORT, SYS_12V_ON_PIN)) {
                if (get_sig(sys_sig, sig_system))
                    IAddFeedbackStrTo(pAt_feedback_str, "On\n");
                else
                    IAddFeedbackStrTo(pAt_feedback_str, "Idle\n");
            } else {
                IAddFeedbackStrTo(pAt_feedback_str, "Off\n");
            }
        } else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
}

IAtOperationRegister(kCmdLightSource, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyOn == my_kvs[0].value) {
            set_sig(sys_sig, sig_lightsource, true);
            ULOG_DEBUG("sig_lightsource on\n");
            if (get_sig(sys_sig, sig_ovp921_status))
                IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            else
                IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n"); // ovp921 must work normal with MIPI input
        } else if (kKeyOff == my_kvs[0].value) {
            clear_sig(sys_sig, sig_lightsource);
            ULOG_DEBUG("sig_lightsource off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    } else {
        if (kKeyStatus == my_kvs[0].key) {
            if (gpio_output_bit_get(LD_EN_H_PORT, LD_EN_H_PIN)) {
                IAddFeedbackStrTo(pAt_feedback_str, "On\n");
            } else {
                IAddFeedbackStrTo(pAt_feedback_str, "Off\n");
            }
        } else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
}

IAtOperationRegister(kCmdVersion, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);
    char str_buff[32] = { 0 };
    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        ULOG_DEBUG("no support setting!\n");
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyMcu:
                ULOG_DEBUG("kKeyMcu\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, MCU_VERSION);
                break;
            case kKeyLightEngineBoard:
                ULOG_DEBUG("kKeyLightEngineBoard\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:VER1%01X.VER0%01X\n", pAt_Kv_List->pList[i].key.pData,
                    gpio_output_bit_get(HW_VER1_PORT, HW_VER1_PIN), gpio_output_bit_get(HW_VER0_PORT, HW_VER0_PIN));
                break;
            case kKeyAnf1:
                ULOG_DEBUG("kKeyAnf1\n");
                get_anf_version(str_buff, 1);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            case kKeyAnf2:
                ULOG_DEBUG("kKeyAnf2\n");
                get_anf_version(str_buff, 2);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            case kKeyAnf3:
                ULOG_DEBUG("kKeyAnf3\n");
                get_anf_version(str_buff, 3);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            case kKeyOvp921:
                ULOG_DEBUG("kKeyOvp921\n");
                get_firmware_version(str_buff);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;
            case kKeyEeprom:
                ULOG_DEBUG("kKeyEeprom\n");
                get_firmware_version(str_buff);
                IAddKeyValueStrTo(pAt_feedback_str, "%s:V%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.version);
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
    char str_buff[32] = { 0 };

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        // IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyLightEngine:
                strncpy(eeprom.Sn_LightEngine, my_kvs[i].value, 32);
                eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_Sn_LightEngine], (TickType_t)10);
                break;
            case kKeySourceLight:
                strncpy(eeprom.Sn_SourceLight, my_kvs[i].value, 32);
                eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_Sn_SourceLight], (TickType_t)10);
                break;
            case kKeyProjector:
                strncpy(eeprom.Sn_Projector, my_kvs[i].value, 32);
                eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_Sn_Projector], (TickType_t)10);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }
        IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
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
                return;
            }
        }
    }
}

IAtOperationRegister(kCmdLightSourceTime, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        ULOG_DEBUG("no support setting!\n");
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
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

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyCeilingFront == my_kvs[0].value) {

            horizontal_flip(true);
            vertical_flip(false);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyCeilingRear == my_kvs[0].value) {
            horizontal_flip(false);
            vertical_flip(false);

            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyTableFront == my_kvs[0].value) {
            horizontal_flip(true);
            vertical_flip(true);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyTableRear == my_kvs[0].value) {
            horizontal_flip(false);
            vertical_flip(true);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdTestPattern, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyRed == my_kvs[0].value) {
            show_solid_color_pattern(255, 0, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyGreen == my_kvs[0].value) {
            show_solid_color_pattern(0, 255, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyBlue == my_kvs[0].value) {
            show_solid_color_pattern(0, 0, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyGrey == my_kvs[0].value) {
            show_solid_color_pattern(127, 127, 127);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyMagenta == my_kvs[0].value) {
            show_solid_color_pattern(255, 0, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyCyan == my_kvs[0].value) {
            show_solid_color_pattern(0, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyYellow == my_kvs[0].value) {
            show_solid_color_pattern(255, 255, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyBlack == my_kvs[0].value) {
            show_solid_color_pattern(0, 0, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyWhite == my_kvs[0].value) {
            show_solid_color_pattern(255, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyCheckerboard == my_kvs[0].value) {
            checkerboard_pattern();
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyHorizontalRamp == my_kvs[0].value) {
            gray_ramp_pattern();
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyOff == my_kvs[0].value) {
            off_pattern();
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdTemperature, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
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
                return;
            }
        }
    }
}

IAtOperationRegister(kCmdCwSpeed, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[1];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        // IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
        extern int target_speed;
        if (kKeyScatteringWheel == my_kvs[0].key) {
            sscanf(my_kvs[0].value, "%d", &target_speed);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    } else {
        if (kKeyScatteringWheel == my_kvs[0].key) {
            IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[0].key.pData, Get_fan_timer_FG(&cw_wheel_fg) * 30);
        } else
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
}

IAtOperationRegister(kCmdReset, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyFactory == my_kvs[0].value) {
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            eeprom_write(&BL24C64A, eeprom_mem[idx_check_sum].addr, 0);
            init_eeprom(&BL24C64A);
            set_sig(sys_sig, sig_lightsource, true);
            set_sig(sys_sig, sig_system, true);
        } else if (kKeyUser == my_kvs[0].value) {
            // todo
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdError, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[1];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
    }
}

IAtOperationRegister(kCmdLogInfo, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Int my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueInt, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    } else {
        if (kKeyStatus == my_kvs[0].key) {
            // todo
        } else if (kKeyClear == my_kvs[0].key) {
            // todo
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    }
}

IAtOperationRegister(kCmdCurrent, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        int current;

        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyR:
                sscanf(my_kvs[i].value, "%d", &current);
                // check current
                eeprom.red = (float)current / 100;
                eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_red], (TickType_t)10);
                break;
            case kKeyG:
                sscanf(my_kvs[i].value, "%d", &current);
                // check current
                eeprom.green = (float)current / 100;
                eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_green], (TickType_t)10);
                break;
            case kKeyB:
                sscanf(my_kvs[i].value, "%d", &current);
                // check current
                eeprom.blue = (float)current / 100;
                eeprom.check_sum = get_LSB_array_crc((uint8_t*)(&eeprom.magic_num), sizeof(eeprom_t) - sizeof(uint32_t));
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_blue], (TickType_t)10);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }
        IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");

    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyR:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n",
                    pAt_Kv_List->pList[i].key.pData, (int)(eeprom.red * 100));
                break;
            case kKeyG:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n",
                    pAt_Kv_List->pList[i].key.pData, (int)(eeprom.green * 100));
                break;
            case kKeyB:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n",
                    pAt_Kv_List->pList[i].key.pData, (int)(eeprom.blue * 100));
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }
    }
}

IAtOperationRegister(kCmdUpgradeOvp921Anf, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    size_t anf_idx, at_crc = 0;
    uint8_t anf_data[256] = { 0 };
    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyIdx:
                sscanf(my_kvs[i].value, "%X", &anf_idx);
                if (anf_idx > 9 || anf_idx == 0)
                    goto OVP921ANF_VALUE_ERROR;
                break;
            case kKeyCrc:
                sscanf(my_kvs[i].value, "%X", &at_crc);
                ULOG_DEBUG("at_crc: %#x\n", at_crc);
                break;
            case kKeyData:
                if (512 != strlen(my_kvs[i].value))
                    goto OVP921ANF_VALUE_ERROR;
                if (false == AsciiToInt(my_kvs[i].value, anf_data, 1))
                    goto OVP921ANF_VALUE_ERROR;
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }

        if (at_crc != get_MSB_array_crc(anf_data, sizeof(anf_data)))
            goto OVP921ANF_VALUE_ERROR;

        clear_sig(sys_sig, sig_system);

        if (!update_anf(anf_idx, anf_data, sizeof(anf_data))) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
            clear_sig(sys_sig, sig_update_anf);
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    return;
OVP921ANF_VALUE_ERROR:
    clear_sig(sys_sig, sig_update_anf);
    IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
}

IAtOperationRegister(kCmdUpgradeOvp921Firmware, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    size_t at_crc = 0;
    uint8_t firmware_data[256] = { 0 };
    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyCrc:
                sscanf(my_kvs[i].value, "%X", &at_crc);
                ULOG_DEBUG("at_crc: %#x\n", at_crc);
                break;
            case kKeyData:
                if (512 != strlen(my_kvs[i].value))
                    goto OVP921FIRMWARE_VALUE_ERROR;
                if (false == AsciiToInt(my_kvs[i].value, firmware_data, 1))
                    goto OVP921FIRMWARE_VALUE_ERROR;
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }

        if (at_crc != get_MSB_array_crc(firmware_data, sizeof(firmware_data)))
            goto OVP921FIRMWARE_VALUE_ERROR;

        clear_sig(sys_sig, sig_system);
        if (!update_firmware(firmware_data, sizeof(firmware_data))) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
            clear_sig(sys_sig, sig_update_firmware);
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    return;
OVP921FIRMWARE_VALUE_ERROR:
    IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
    clear_sig(sys_sig, sig_update_firmware);
}

IAtOperationRegister(kCmdOvp921, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    size_t size = 0;
    size_t addr = 0;
    char string[256] = { 0 };
    uint8_t data[128] = { 0 };
    int ret;

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                ret = sscanf(my_kvs[i].value, "%X", &addr);
                break;
            case kKeySize:
                ret = sscanf(my_kvs[i].value, "%X", &size);
                break;
            case kKeyData:
                ret = sscanf(my_kvs[i].value, "%256s", string);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            if (ret == EOF || ret == 0)
                goto OVP921_VALUE_ERROR;
        }
        if (size > 128)
            goto OVP921_VALUE_ERROR;

        if (AsciiToInt(string, data, size) == false)
            goto OVP921_VALUE_ERROR;

        if (set_reg_block(addr, data, size) != true)
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        else
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");

        memset(string, 0, size);
        IntToAscii(data, string, 1, size);
        ULOG_DEBUG("set_reg_block %#X:%s\n", addr, string);
    } else {
        char ascii_output[512 + 1] = { 0 };
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                ret = sscanf(my_kvs[i].value, "%X", &addr);
                break;
            case kKeySize:
                ret = sscanf(my_kvs[i].value, "%X", &size);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            if (ret == EOF || ret == 0)
                goto OVP921_VALUE_ERROR;
        }
        if (size > 256)
            goto OVP921_VALUE_ERROR;

        if (get_reg_block(addr, data, size) != true) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        } else {
            IntToAscii(data, ascii_output, 1, size);
            IAddFeedbackStrTo(pAt_feedback_str, "Data:%s\n", ascii_output);
        }
    }
    return;
OVP921_VALUE_ERROR:
    IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
}

uint8_t eeprom_data[256] = { 0 };
IAtOperationRegister(kCmdEeprom, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    char ascii_buff[512 + 1] = { 0 };
    size_t addr, size = 0;
    int ret = 0;
    mem_t msg;

    if (get_sig(sys_sig, sig_eeprom_write)) {
        IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        return;
    }
    memset(eeprom_data, 0, sizeof(eeprom_data));
    if (kAtControlType == IGetAtCmdType(&at_obj)) {

        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                ret = sscanf(my_kvs[i].value, "%X", &addr);
                break;
            case kKeySize:
                ret = sscanf(my_kvs[i].value, "%X", &size);
                break;
            case kKeyData:
                ret = sscanf(my_kvs[i].value, "%256s", ascii_buff);
                break;
            case kKeyClear:
                // todo
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            if (ret == EOF || ret == 0)
                goto EEPROM_VALUE_ERROR;
        }

        if (size > 256 && size % 4 != 0)
            goto EEPROM_VALUE_ERROR;
        if (addr % 4 != 0)
            goto EEPROM_VALUE_ERROR;

        AsciiToInt(ascii_buff, eeprom_data, 1);
        set_sig(sys_sig, sig_eeprom_write, true);
        memory_endian_conversion(eeprom_data, size);
        msg.idx = idx_eeprom_write;
        msg.addr = addr;
        msg.size = size;
        msg.pData = eeprom_data;
        xQueueSend(xQueue_eeprom, (void*)&msg, (TickType_t)10);
        IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
#if 0
        if (true == eeprom_block_write(&BL24C64A, msg.addr, (uint8_t*)msg.pData, msg.size))
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        else
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
#endif
    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                ret = sscanf(my_kvs[i].value, "%X", &addr);
                break;
            case kKeySize:
                ret = sscanf(my_kvs[i].value, "%X", &size);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            if (ret == EOF || ret == 0)
                goto EEPROM_VALUE_ERROR;
        }
        if (size > 256 && size % 4 != 0)
            goto EEPROM_VALUE_ERROR;
        if (addr % 4 != 0)
            goto EEPROM_VALUE_ERROR;

        memset(eeprom_data, 0, sizeof(eeprom_data));
        if (true == eeprom_block_read(&BL24C64A, addr, (uint8_t*)eeprom_data, size)) {
            memory_endian_conversion(eeprom_data, size);
            IntToAscii(eeprom_data, ascii_buff, 1, size);
            IAddFeedbackStrTo(pAt_feedback_str, "%s\n", ascii_buff);
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        }
    }
    return;
EEPROM_VALUE_ERROR:
    IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
}

IAtOperationRegister(kCmdSilentAsyncMessages, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyOn == my_kvs[0].value) {
            set_sig(sys_sig, sig_slient_async_msg, true);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyOff == my_kvs[0].value) {
            set_sig(sys_sig, sig_slient_async_msg, false);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    } else {
        if (get_sig(sys_sig, sig_slient_async_msg) == true) {
            IAddFeedbackStrTo(pAt_feedback_str, "On\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "Off\n");
        }
    }
}
