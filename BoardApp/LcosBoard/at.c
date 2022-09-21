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
#include "AtCmdHashTable.h"
#include "AtKeyHashTable.h"
#include "AtProtocol.h"
#include "BoardInit.h"
#include "basicApp.h"
#include "eeprom.h"
#include "ulog.h"
#include "utils.h"
#include "utilsAsciiConvert.h"
#include "rti_vc_api.h"
#include <stdio.h>
#include <string.h>

extern asAtProtocol at_obj;
extern const mem_t eeprom_mem[];
extern temperature_t temperature[];
extern file_t eeprom_log;

// TODO: 搞清楚RDC200A的正常运行的判断
IAtOperationRegister(kCmdSystem, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        switch (my_kvs[0].value) {
        case kKeyOn:
            gpio_bit_set(SYS_12V_ON_PORT, SYS_12V_ON_PIN);
            if (get_sig(sys_sig, sig_rdc200a_status) != true) // if rdc200a is not working, then reset it
            {
                gpio_bit_set(RDC200A_RESET_PORT, RDC200A_RESET_PIN);
                vTaskDelay(100);
                gpio_bit_reset(RDC200A_RESET_PORT, RDC200A_RESET_PIN);
            }
            ULOG_DEBUG("system on\n");
            set_sig(sys_sig, sig_lightsource, true);
            set_sig(sys_sig, sig_system, true);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            break;

        case kKeyIdle:
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            clear_sig(sys_sig, sig_update_rdc200a);
            clear_sig(sys_sig, sig_update_firmware);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            break;

        case kKeyOff:
            ULOG_DEBUG("system off\n");
            laser_off();
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            clear_sig(sys_sig, sig_update_rdc200a);
            clear_sig(sys_sig, sig_update_firmware);
            clear_sig(sys_sig, sig_slient_async_msg);
            clear_sig(sys_sig, sig_rdc200a_status);
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

// TODO: add display off and add dispaly on
IAtOperationRegister(kCmdLightSource, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyOn == my_kvs[0].value) {
            set_sig(sys_sig, sig_lightsource, true);
            ULOG_DEBUG("sig_lightsource on\n");
            if (get_sig(sys_sig, sig_rdc200a_status))
                IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            else
                IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n"); // rdc200a must work normal with MIPI input
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
                IAddKeyValueStrTo(pAt_feedback_str, "%s:VER1-%01X.VER0-%01X\n", pAt_Kv_List->pList[i].key.pData,
                    gpio_output_bit_get(HW_VER1_PORT, HW_VER1_PIN), gpio_output_bit_get(HW_VER0_PORT, HW_VER0_PIN));
                break;

                // TODO: add version
            case kKeyRdc200a:
                ULOG_DEBUG("kKeyRdc200a\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, get_rdc200a_version(str_buff, sizeof(str_buff)));
                break;

            case kKeyRdp250h:
                ULOG_DEBUG("kKeyRdp250h\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, get_rdp250h_version(str_buff, sizeof(str_buff)));
                break;

            case kKeyEeprom:
                ULOG_DEBUG("kKeyEeprom\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.version);
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
            case kKeyHour:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.light_source_time / 60);
                break;
            case kKeyMinute:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.light_source_time / 60 % 60);
                break;
            case kKeySecond:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.light_source_time % 60);
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
            // TODO: add raontech lcos flip
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyCeilingRear == my_kvs[0].value) {
            // TODO: add raontech lcos flip
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyTableFront == my_kvs[0].value) {
            // TODO: add raontech lcos flip
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyTableRear == my_kvs[0].value) {
            // TODO: add raontech lcos flip
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
            // TODO: add testpattern show
            rtiVC_GenerateTestPattern(255, 0, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyGreen == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(0, 255, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyBlue == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(0, 0, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyGrey == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(128, 128, 128);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyMagenta == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(255, 0, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyCyan == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(0, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyYellow == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(255, 255, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyBlack == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(0, 0, 0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyWhite == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(255, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyCheckerboard == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(255, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyHorizontalRamp == my_kvs[0].value) {
            rtiVC_GenerateTestPattern(255, 255, 255);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyOff == my_kvs[0].value) {
            // TODO: add testpattern off
            rtiVC_EnableTestPattern(false);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            return;
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
        rtiVC_EnableTestPattern(true);
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
        // TODO:test temperature
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyNtcBlue:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature[blue_sensor].temperature / 10);
                break;
            case kKeyNtcRed:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature[red_sensor].temperature / 10);
                break;
            case kKeyNtcGreen:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature[green_sensor].temperature / 10);
                break;
            case kKeyNtcLcos:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature[lcos_sensor].temperature / 10);
                break;
            case kKeyNtcEnv:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature[evn_sensor].temperature / 10);
                break;
            case kKeyI2cBlue:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature_i2c[blue_sensor].temperature / 10);
                break;
            case kKeyI2cRed:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature_i2c[red_sensor].temperature / 10);
                break;
            case kKeyI2cGreen:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature_i2c[green_sensor].temperature / 10);
                break;
            case kKeyI2cLcos:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature_i2c[lcos_sensor].temperature / 10);
                break;
            case kKeyI2cEnv:
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%.1f\n", pAt_Kv_List->pList[i].key.pData, (float)temperature_i2c[evn_sensor].temperature / 10);
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
            file_remove_all(&eeprom_log);
            eeprom_write(&BL24C64A, eeprom_mem[idx_check_sum].addr, 0);
            init_eeprom(&BL24C64A);
            set_sig(sys_sig, sig_lightsource, true);
            set_sig(sys_sig, sig_system, true);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");

        } else if (kKeyUser == my_kvs[0].value) {
            // TODO: add user reset.
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
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
            line_t line_tmp[32] = { 0 };
            int read_number = my_kvs[0].value > sizeof(line_tmp) ? sizeof(line_tmp) : my_kvs[0].value;
            int feedback_number = file_burst_read(&eeprom_log, line_tmp, read_number);
            for (int i = 0; i < feedback_number; i++) {
                output_printf("AT+LogInfo#Time:%.2d-%02d-%02d,Text:%s", line_tmp[i].time / 60 / 60, (line_tmp[i].time / 60) % 60, line_tmp[i].time % 60, line_tmp[i].text);
            }
        } else if (kKeyClear == my_kvs[0].key) {
            file_remove_all(&eeprom_log);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
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

IAtOperationRegister(kCmdSpiFlash, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    size_t addr, at_crc = 0;
    uint8_t anf_data[256] = { 0 };
    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                sscanf(my_kvs[i].value, "%X", &addr);
                if (addr > 0xFFFF)
                    goto SPIFLASH_VALUE_ERROR;
                break;
            case kKeyCrc:
                sscanf(my_kvs[i].value, "%X", &at_crc);
                ULOG_DEBUG("at_crc: %#x\n", at_crc);
                break;
            case kKeyData:
                if (512 != strlen(my_kvs[i].value))
                    goto SPIFLASH_VALUE_ERROR;
                if (false == AsciiToInt(my_kvs[i].value, anf_data, 1))
                    goto SPIFLASH_VALUE_ERROR;
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }

        if (at_crc != get_MSB_array_crc(anf_data, sizeof(anf_data)))
            goto SPIFLASH_VALUE_ERROR;

        clear_sig(sys_sig, sig_system);

        // TODO: add rdc200a update.
        if (0) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
            clear_sig(sys_sig, sig_update_rdc200a);
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
    return;
SPIFLASH_VALUE_ERROR:
    clear_sig(sys_sig, sig_update_rdc200a);
    IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
}

IAtOperationRegister(kCmdRdc200a, pAt_Kv_List, pAt_feedback_str)
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
                goto RDC200A_VALUE_ERROR;
        }
        if (size > 128)
            goto RDC200A_VALUE_ERROR;

        // if (AsciiToInt(string, data, size) == false)
        //     goto RDC200A_VALUE_ERROR;

        // if (set_reg_block(addr, data, size) != true)
        //     IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        // else
        //     IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");

        // memset(string, 0, size);
        // IntToAscii(data, string, 1, size);
        // ULOG_DEBUG("set_reg_block %#X:%s\n", addr, string);
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
                goto RDC200A_VALUE_ERROR;
        }
        if (size > 256)
            goto RDC200A_VALUE_ERROR;

        // TODO: add rdc200a r w
        //  if (get_reg_block(addr, data, size) != true) {
        //      IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        //  } else {
        //      IntToAscii(data, ascii_output, 1, size);
        //      IAddFeedbackStrTo(pAt_feedback_str, "Data:%s\n", ascii_output);
        //  }
    }
    return;
RDC200A_VALUE_ERROR:
    IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
}

IAtOperationRegister(kCmdRdp250h, pAt_Kv_List, pAt_feedback_str)
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
                goto RDC250H_VALUE_ERROR;
        }
        if (size > 128)
            goto RDC250H_VALUE_ERROR;

        // if (AsciiToInt(string, data, size) == false)
        //     goto RDC250H_VALUE_ERROR;

        // if (set_reg_block(addr, data, size) != true)
        //     IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        // else
        //     IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");

        // memset(string, 0, size);
        // IntToAscii(data, string, 1, size);
        // ULOG_DEBUG("set_reg_block %#X:%s\n", addr, string);
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
                goto RDC250H_VALUE_ERROR;
        }
        if (size > 256)
            goto RDC250H_VALUE_ERROR;

        // TODO: add rdc250h r w
        //  if (get_reg_block(addr, data, size) != true) {
        //      IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        //  } else {
        //      IntToAscii(data, ascii_output, 1, size);
        //      IAddFeedbackStrTo(pAt_feedback_str, "Data:%s\n", ascii_output);
        //  }
    }
    return;
RDC250H_VALUE_ERROR:
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
    mem_t msg = { 0 };

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
                ret = sscanf(my_kvs[i].value, "%512s", ascii_buff);
                break;
            case kKeyClear:
                // TODO: add eeprom clear
                file_remove_all(&eeprom_log);
                eeprom_write(&BL24C64A, eeprom_mem[idx_check_sum].addr, 0);
                init_eeprom(&BL24C64A);
                IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
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

        if (AsciiToInt(ascii_buff, eeprom_data, 1) != true)
            goto EEPROM_VALUE_ERROR;

        set_sig(sys_sig, sig_eeprom_write, true);
        memory_endian_conversion(eeprom_data, size);
        msg.idx = idx_eeprom_write;
        msg.addr = addr;
        msg.size = size;
        msg.pData = eeprom_data;

        xQueueSend(xQueue_eeprom, (void*)&msg, (TickType_t)10);
        IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");

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

IAtOperationRegister(kCmdFlashBootb, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyOn == my_kvs[0].value) {
            // TODO: gpio control
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyOff == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    } else {
        // TODO: gpio control
        if (0) {
            IAddFeedbackStrTo(pAt_feedback_str, "On\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "Off\n");
        }
    }
}
