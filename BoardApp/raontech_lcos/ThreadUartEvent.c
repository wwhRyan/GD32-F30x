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
extern const Uarter uart0_output;
extern const Uarter uart1_debug;

void ThreadUartEvent(void* pvParameters)
{
    ULOG_DEBUG("%s\n", __func__);
    // reload_idu_current();

    init_eeprom(&BL24C64A);

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

    while (1) {
#if 0
        if (0 != GetRxlen(&uart0_output)) {
            debug_printf("Rec SignalBoard: %s\r\n", GetRxData(&uart0_output));
            // ICmdLinesInput(GetRxData(&uart0_output));
            // IAtCmdDecodeAndRun(&at_obj, str);
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
        vTaskDelay(5);
    }
}