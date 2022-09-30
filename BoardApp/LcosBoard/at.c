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
#include "gd32f30x_gpio.h"
#include "rti_vc_api.h"
#include "ulog.h"
#include "utils.h"
#include "utilsAsciiConvert.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern asAtProtocol at_obj;
extern const mem_t eeprom_mem[];
extern temperature_t temperature[];
extern file_t eeprom_log;

IAtOperationRegister(kCmdSystem, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        switch (my_kvs[0].value) {
        case kKeyOn:
            if (power_resume() == true) {
                set_sig(sys_sig, sig_lightsource, true);
                set_sig(sys_sig, sig_system, true);
                IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");

            } else {
                set_sig(sys_sig, sig_lightsource, false);
                set_sig(sys_sig, sig_system, false);
                IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
                EXCUTE_ONCE(ULOG_ERROR("error to power on"));
            }
            break;

        case kKeyIdle:
            if (gpio_output_bit_get(SYS_12V_ON_PORT, SYS_12V_ON_PIN)) {
                clear_sig(sys_sig, sig_lightsource);
                clear_sig(sys_sig, sig_system);
                clear_sig(sys_sig, sig_update_rdc200a);
                clear_sig(sys_sig, sig_update_firmware);
                IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            } else {
                IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
            }
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
                eeprom_block_write(&BL24C64A, (const mem_t*)&eeprom_mem[idx_Sn_LightEngine], true);
                break;
            case kKeySourceLight:
                strncpy(eeprom.Sn_SourceLight, my_kvs[i].value, 32);
                eeprom_block_write(&BL24C64A, (const mem_t*)&eeprom_mem[idx_Sn_SourceLight], true);
                break;
            case kKeyProjector:
                strncpy(eeprom.Sn_Projector, my_kvs[i].value, 32);
                eeprom_block_write(&BL24C64A, (const mem_t*)&eeprom_mem[idx_Sn_Projector], true);
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
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[i].key.pData, eeprom.light_source_time / 60 / 60);
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
            h_v_flip_set(v_0_h_0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyCeilingRear == my_kvs[0].value) {
            h_v_flip_set(v_0_h_1);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyTableFront == my_kvs[0].value) {
            h_v_flip_set(v_1_h_0);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyTableRear == my_kvs[0].value) {
            h_v_flip_set(v_1_h_1);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    } else {
        flip_t tmp = h_v_flip_get();
        switch (tmp) {
        case v_0_h_0:
            IAddFeedbackStrTo(pAt_feedback_str, "CeilingFront\n");
            break;
        case v_0_h_1:
            IAddFeedbackStrTo(pAt_feedback_str, "CeilingRear\n");
            break;
        case v_1_h_0:
            IAddFeedbackStrTo(pAt_feedback_str, "TableFront\n");
            break;
        case v_1_h_1:
            IAddFeedbackStrTo(pAt_feedback_str, "TableRear\n");
            break;
        default:
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
            return;
        }
    }
}

IAtOperationRegister(kCmdTestPattern, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyOff != my_kvs[0].value) {
            switch (my_kvs[0].value) {
            case kKeyRed:
                Border(255, 0, 0);
                break;

            case kKeyGreen:
                Border(0, 255, 0);
                break;

            case kKeyBlue:
                Border(0, 0, 255);
                break;

            case kKeyGrey:
                Border(128, 128, 128);
                break;

            case kKeyMagenta:
                Border(255, 0, 255);
                break;

            case kKeyCyan:
                Border(0, 255, 255);
                break;

            case kKeyYellow:
                Border(255, 255, 0);
                break;

            case kKeyBlack:
                Border(0, 0, 0);
                break;

            case kKeyWhite:
                Border(255, 255, 255);
                break;

            case kKeyCheckerboard:
                check_box();
                break;

            case kKeyHorizontalRamp:
                vertical_gradation();
                break;

            case kKeyVerticalRamp:
                horizontal_gradation();
                break;

            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyOff == my_kvs[0].value) {
            off_testpattern();
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
            return;
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
            return;
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "%s\n", get_test_pattern());
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
            eeprom.light_source_time = 0;
            eeprom_block_write(&BL24C64A, &eeprom_mem[idx_light_source_time], true);
            set_sig(sys_sig, sig_lightsource, true);
            set_sig(sys_sig, sig_system, true);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyUser == my_kvs[0].value) {
            // TODO: add user reset.
            /* 屏蔽打印信息 */
            extern void my_console_logger(ulog_level_t severity, char* msg);
            ULOG_SUBSCRIBE(my_console_logger, ULOG_CRITICAL_LEVEL);
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
        if (kKeyClear == my_kvs[0].key) {
            file_remove_all(&eeprom_log);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    } else {
        if (kKeyStatus == my_kvs[0].key) {
            line_t line_tmp[32] = { 0 };
            int read_number = my_kvs[0].value > sizeof(line_tmp) ? sizeof(line_tmp) : my_kvs[0].value;
            int feedback_number = file_burst_read(&eeprom_log, line_tmp, read_number);
            /* Text:%s include '\n' */
            if (feedback_number != 0) {
                for (int i = 0; i < feedback_number; i++) {
                    output_printf("AT+LogInfo#Time:%.2d-%02d-%02d,Text:%s", line_tmp[i].time / 60 / 60, (line_tmp[i].time / 60) % 60, line_tmp[i].time % 60, line_tmp[i].text);
                    debug_printf("AT+LogInfo#Time:%.2d-%02d-%02d,Text:%s", line_tmp[i].time / 60 / 60, (line_tmp[i].time / 60) % 60, line_tmp[i].time % 60, line_tmp[i].text);
                }
            } else {
                output_printf("AT+LogInfo#Time:%.2d-%02d-%02d,Text:%s", 0, 0, 0, "Null");
                debug_printf("AT+LogInfo#Time:%.2d-%02d-%02d,Text:%s", 0, 0, 0, "Null");
            }
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
                eeprom_block_write(&BL24C64A, (const mem_t*)&eeprom_mem[idx_red], true);
                break;
            case kKeyG:
                sscanf(my_kvs[i].value, "%d", &current);
                // check current
                eeprom.green = (float)current / 100;
                eeprom_block_write(&BL24C64A, (const mem_t*)&eeprom_mem[idx_green], true);
                break;
            case kKeyB:
                sscanf(my_kvs[i].value, "%d", &current);
                // check current
                eeprom.blue = (float)current / 100;
                eeprom_block_write(&BL24C64A, (const mem_t*)&eeprom_mem[idx_blue], true);

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

    size_t addr, at_crc, size = 0;
    uint8_t buff[256] = { 0 };
    char string[512 + 1] = { 0 };
    int ret = 0;
    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                ret = sscanf(my_kvs[i].value, "%X", &addr);
                if (addr > 0xFFFF)
                    goto SPIFLASH_VALUE_ERROR;
                break;
            case kKeyCrc:
                ret = sscanf(my_kvs[i].value, "%X", &at_crc);
                ULOG_TRACE("at_crc: %#x\n", at_crc);
                break;
            case kKeyData:
                ret = sscanf(my_kvs[i].value, "%512s", string);
                if (512 != strlen(my_kvs[i].value))
                    goto SPIFLASH_VALUE_ERROR;
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            if (ret == EOF || ret == 0)
                goto SPIFLASH_VALUE_ERROR;
        }
        if (false == AsciiToInt(string, buff, 1))
            goto SPIFLASH_VALUE_ERROR;

        if (at_crc != get_MSB_array_crc(buff, sizeof(buff)))
            goto SPIFLASH_VALUE_ERROR;

        set_sig(sys_sig, sig_update_rdc200a, true);
        if (0 != rtiVC_ProgramFLASH(addr, buff, 256)) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
            clear_sig(sys_sig, sig_update_rdc200a);
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        }
    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                ret = sscanf(my_kvs[i].value, "%X", &addr);
                if (addr > 0xFFFF)
                    goto SPIFLASH_VALUE_ERROR;
                break;
            case kKeySize:
                ret = sscanf(my_kvs[i].value, "%X", &size);
                if (size > 256)
                    goto SPIFLASH_VALUE_ERROR;
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            if (ret == EOF || ret == 0)
                goto SPIFLASH_VALUE_ERROR;
        }

        if (rtiVC_ReadFLASH(addr, buff, size) != 0) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        } else {
            IntToAscii(buff, string, 1, size);
            IAddFeedbackStrTo(pAt_feedback_str, "Data:%s\n", string);
        }
    }
    return;
SPIFLASH_VALUE_ERROR:
    clear_sig(sys_sig, sig_update_rdc200a);
    IAddFeedbackStrTo(pAt_feedback_str, "InvalidValue\n");
}

IAtOperationRegister(kCmdSpiFlashErase, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    size_t addr, size = 0;
    int ret = 0;
    if (kAtControlType == IGetAtCmdType(&at_obj)) {

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
            if (ret == EOF || ret == 0) {
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }

        if (spi_flash_erase(addr, size) == true)
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        else {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        }
    } else {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    }
}

IAtOperationRegister(kCmdRdc200a, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Str my_kvs[MAX_KV_COUPLES_NUM];
    ICastAtKvListTo(kAtValueStr, pAt_Kv_List, my_kvs);

    size_t size = 0;
    size_t addr = 0;
    char string[512 + 1] = { 0 };
    uint8_t data[256] = { 0 };
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
                ret = sscanf(my_kvs[i].value, "%512s", string);
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

        if (AsciiToInt(string, data, 1) == false)
            goto RDC200A_VALUE_ERROR;

        if (set_reg_block(RDC200A_ADDR, addr, data, size) != true)
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        else
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
                goto RDC200A_VALUE_ERROR;
        }
        if (size > 256)
            goto RDC200A_VALUE_ERROR;

        if (get_reg_block(RDC200A_ADDR, addr, data, size) != true) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        } else {
            IntToAscii(data, string, 1, size);
            IAddFeedbackStrTo(pAt_feedback_str, "Data:%s\n", string);
        }
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
    char string[512 + 1] = { 0 };
    uint8_t data[256] = { 0 };
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
        if (size > 256)
            goto RDC250H_VALUE_ERROR;

        if (AsciiToInt(string, data, 1) == false)
            goto RDC250H_VALUE_ERROR;

        set_panel_reg_block(addr, data, size);
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
                goto RDC250H_VALUE_ERROR;
        }
        if (size > 256)
            goto RDC250H_VALUE_ERROR;

        get_panel_reg_block(addr, data, size);
        IntToAscii(data, string, 1, size);
        IAddFeedbackStrTo(pAt_feedback_str, "Data:%s\n", string);
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
                file_remove_all(&eeprom_log);
                eeprom_block_clear();
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

        if (eeprom_block_write(&BL24C64A, (const mem_t*)&msg, true))
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        else
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");

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
        if (true == i2c_muti_read(&BL24C64A, addr, (uint8_t*)eeprom_data, size)) {
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
            gpio_bit_set(RDC200A_BOOTB_IN_PORT, RDC200A_BOOTB_IN_PIN);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else if (kKeyOff == my_kvs[0].value) {
            gpio_bit_reset(RDC200A_BOOTB_IN_PORT, RDC200A_BOOTB_IN_PIN);
            IAddFeedbackStrTo(pAt_feedback_str, "Ok\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    } else {
        if (kKeyStatus == my_kvs[0].value) {
            if (SET == gpio_output_bit_get(RDC200A_BOOTB_IN_PORT, RDC200A_BOOTB_IN_PIN))
                IAddFeedbackStrTo(pAt_feedback_str, "On\n");
            else
                IAddFeedbackStrTo(pAt_feedback_str, "Off\n");
        } else {
            IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
        }
    }
}
