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

asAtProtocol at_obj;
asAtProtocol output_obj;
extern const Uarter uart0_output;
extern const Uarter uart1_debug;

void ThreadUartEvent(void* pvParameters)
{
    system_ipc_init();

    vTaskDelay(1);
    gpio_bit_set(RDC200A_RESET_PORT, RDC200A_RESET_PIN);

    ULOG_DEBUG("%s\n", __func__);
    init_eeprom(&BL24C64A);
    log_init(&eeprom_log);
    // reload_idu_current();

    int ret;
    ret = rtiVC_Initialize(RDC200A_ADDR);
    if (ret != 0) {
        printf("VC init error (%d)\n", ret);
        E_assert(0);
    }

    // Open device
    ret = rtiVC_OpenDevice();
    if (ret)
        E_assert(0);

    IInitAtLib(&at_obj, kAtNormalMode, NULL, debug_printf);
    IInitAtLib(&output_obj, kAtNormalMode, NULL, output_printf);
    set_sig(sys_sig, sig_mcu_init_ok, true);
    while (1) {
#if 0
        if (0 != GetRxlen(&uart0_output)) {
            debug_printf("Rec SignalBoard: %s\r\n", GetRxData(&uart0_output));
            // ICmdLinesInput(GetRxData(&uart0_output));
            IAtCmdDecodeAndRun(&output_obj, GetRxData(&uart0_output));
            ClearRxData(&uart0_output);
        }
#endif
        if (0 != GetRxlen(&uart1_debug)) {
            // debug_printf("cmd rec->:%s\r\n", GetRxData(&uart1_debug));
            ICmdLinesInput(GetRxData(&uart1_debug));
#if 0
            debug_printf("AT rec->:%s\r\n", GetRxData(&uart1_debug));
            IAtCmdDecodeAndRun(&at_obj, GetRxData(&uart1_debug));
#endif
            ClearRxData(&uart1_debug);
        }
        vTaskDelay(20);
    }
}