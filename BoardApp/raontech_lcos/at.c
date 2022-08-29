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
#include "basicApp.h"
#include "ulog.h"

extern asAtProtocol at_obj;
extern const mem_t eeprom_mem[];

// TODO: add NTC read.: 搞清楚RDC200A的正常运行的判断
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
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
            break;

        case kKeyIdle:
            clear_sig(sys_sig, sig_lightsource);
            clear_sig(sys_sig, sig_system);
            clear_sig(sys_sig, sig_update_rdc200a);
            clear_sig(sys_sig, sig_update_firmware);
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
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
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
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

// TODO: add NTC read.: add display off and add dispaly on
IAtOperationRegister(kCmdLightSource, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (kKeyOn == my_kvs[0].value) {
            set_sig(sys_sig, sig_lightsource, true);
            ULOG_DEBUG("sig_lightsource on\n");
            if (get_sig(sys_sig, sig_rdc200a_status))
                IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
            else
                IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n"); // rdc200a must work normal with MIPI input
        } else if (kKeyOff == my_kvs[0].value) {
            clear_sig(sys_sig, sig_lightsource);
            ULOG_DEBUG("sig_lightsource off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
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

                // TODO: add NTC read.: add version
            case kKeyRdc200a:
                ULOG_DEBUG("kKeyRdc200a\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;

                // TODO: add NTC read.: add version
            case kKeyRdp250h:
                ULOG_DEBUG("kKeyRdp250h\n");
                IAddKeyValueStrTo(pAt_feedback_str, "%s:%s\n", pAt_Kv_List->pList[i].key.pData, str_buff);
                break;

                // TODO: add NTC read.: add version
            case kKeyEeprom:
                ULOG_DEBUG("kKeyAnf3\n");
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
    char str_buff[32] = { 0 };

    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
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
            // TODO: add NTC read.: add raontech lcos flip
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyCeilingRear == my_kvs[0].value) {
            // TODO: add NTC read.: add raontech lcos flip
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyTableFront == my_kvs[0].value) {
            // TODO: add NTC read.: add raontech lcos flip
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyTableRear == my_kvs[0].value) {
            // TODO: add NTC read.: add raontech lcos flip
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
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
            // TODO: add NTC read.: add testpattern show
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyGreen == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyBlue == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyGrey == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyMagenta == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyCyan == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyYellow == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyBlack == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyWhite == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyCheckerboard == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyHorizontalRamp == my_kvs[0].value) {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        } else if (kKeyOff == my_kvs[0].value) {
            // TODO: add NTC read.: add testpattern off
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
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
                // TODO: add NTC read.
                break;
            case kKeyNtcRed:
                // TODO: add NTC read.
                break;
            case kKeyNtcGreen:
                // TODO: add NTC read.
                break;
            case kKeyNtcLcos:
                // TODO: add NTC read.
                break;
            case kKeyNtcEnv:
                // TODO: add NTC read.
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
        IAddFeedbackStrTo(pAt_feedback_str, "InvalidOperator\n");
    } else {
        // TODO: test read speed.
        if (kKeyScatteringWheel == my_kvs[0].key) {
            IAddKeyValueStrTo(pAt_feedback_str, "%s:%d\n", pAt_Kv_List->pList[0].key.pData, Get_fan_timer_FG(&cw_wheel_fg));
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
            // TODO: add user reset.
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
            // TODO: add log read
        } else if (kKeyClear == my_kvs[0].key) {
            // TODO: add log read.
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
        float current;

        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyR:
                sscanf(my_kvs[i].value, "%f", &current);
                // TODO:check current
                eeprom.red = current;
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_red], (TickType_t)10);
                break;
            case kKeyG:
                sscanf(my_kvs[i].value, "%f", &current);
                // check current
                eeprom.green = current;
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_green], (TickType_t)10);
                break;
            case kKeyB:
                sscanf(my_kvs[i].value, "%f", &current);
                // check current
                eeprom.blue = current;
                xQueueSend(xQueue_eeprom, (void*)&eeprom_mem[idx_blue], (TickType_t)10);
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
        }
    } else {
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
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

    size_t addr, size = 0;
    int ret = 0;
    msg_t msg;
    msg.pData = eeprom_data;
    if (kAtControlType == IGetAtCmdType(&at_obj)) {
        if (get_sig(sys_sig, sig_eeprom_write)) {
            IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
            return;
        }
        memset(eeprom_data, 0, sizeof(eeprom_data));
        for (size_t i = 0; i < pAt_Kv_List->size; i++) {
            switch (my_kvs[i].key) {
            case kKeyAddr:
                ret = sscanf(my_kvs[i].value, "%2X", &addr);
                break;
            case kKeySize:
                ret = sscanf(my_kvs[i].value, "%1X", &size);
                break;
            case kKeyData:
                ret = sscanf(my_kvs[i].value, "%256s", eeprom_data);
                break;
            case kKeyClear:
                // TODO: add NTC read.
                break;
            default:
                IAddFeedbackStrTo(pAt_feedback_str, "InvalidKey\n");
                return;
            }
            if (ret == EOF || ret == 0)
                goto EEPROM_VALUE_ERROR;
        }
        if (size > 256)
            goto EEPROM_VALUE_ERROR;

        // set_sig(sys_sig, sig_eeprom_write, true);
        // msg.idx = idx_eeprom_write;
        // msg.addr = addr;
        // msg.size = size;
        // xQueueSend(xQueue_eeprom, (void*)&msg, (TickType_t)10);
    } else {
        char ascii_output[512 + 1] = { 0 };
        uint8_t data_output[256] = { 0 };
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
        if (size > 256)
            goto EEPROM_VALUE_ERROR;

        // if (true == eeprom_block_read(&BL24C64A, addr, (uint8_t*)data_output, size)) {
        //     IntToAscii(data_output, ascii_output, 1, size);
        //     IAddFeedbackStrTo(pAt_feedback_str, "%s\n", ascii_output);
        // } else {
        //     IAddFeedbackStrTo(pAt_feedback_str, "ExecuteFailed\n");
        // }
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
