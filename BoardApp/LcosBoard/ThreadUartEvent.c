/**
 * @file ThreadUartDecode.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-02-14
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#include "AtProtocol.h"
#include "BoardInit.h"
#include "CmdLine.h"
#include "Common.h"
#include "basicApp.h"
#include "gd32f30x.h"
#include "main.h"
#include "rti_vc_api.h"
#include "ulog.h"

asAtProtocol at_obj;
asAtProtocol output_obj;
extern const Uarter uart0_output;
extern const Uarter uart1_debug;

void ThreadUartEvent(void* pvParameters)
{
    system_ipc_init();
    log_init(&eeprom_log);
    ULOG_INFO("log enable\n");
    ULOG_DEBUG("%s\n", __func__);

    vTaskDelay(1);
    ULOG_INFO("rdc200a_reset_pin pull up\n");
    gpio_bit_set(RDC200A_RESET_PORT, RDC200A_RESET_PIN);

    ULOG_INFO("eeprom init\n");
    init_eeprom(&BL24C64A);

    // TODO: verify it
    ULOG_INFO("RDC200A init\n");
    int ret;
    ret = rtiVC_Initialize(RDC200A_ADDR);
    if (ret != 0)
        ULOG_ERROR("VC init error (%d)\n", ret);

    // Open device
    ret = rtiVC_OpenDevice();
    if (ret)
        ULOG_ERROR("VC open device error (%d)\n", ret);

    ULOG_INFO("AtLib init\n");
    IInitAtLib(&at_obj, kAtNormalMode, NULL, debug_printf);
    IInitAtLib(&output_obj, kAtNormalMode, NULL, output_printf);

    ULOG_INFO("init complete\n");
    set_sig(sys_sig, sig_mcu_init_ok, true);
    while (1) {

        if (0 != GetRxlen(&uart0_output)) {
            ULOG_TRACE("<uart0_output rx> %s\n", GetRxData(&uart0_output));
            // ICmdLinesInput(GetRxData(&uart0_output));
            IAtCmdDecodeAndRun(&output_obj, GetRxData(&uart0_output));
            ClearRxData(&uart0_output);
        }

        if (0 != GetRxlen(&uart1_debug)) {
            // ULOG_TRACE("cmd rec->:%s\r\n", GetRxData(&uart1_debug));
            // ICmdLinesInput(GetRxData(&uart1_debug));
            ULOG_TRACE("<uart1_debug rx> %s\n", GetRxData(&uart1_debug));
            IAtCmdDecodeAndRun(&at_obj, GetRxData(&uart1_debug));
            ClearRxData(&uart1_debug);
        }
        vTaskDelay(20);
    }
}
