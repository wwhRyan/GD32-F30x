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
#include "basicApp.h"
#include "gd32f30x.h"
#include "main.h"
#include "ovp921.h"


asAtProtocol at_obj;
asAtProtocol output_obj;
extern const Uarter uart0_output;
extern const Uarter uart1_debug;

void ThreadUartEvent(void* pvParameters)
{
    ULOG_DEBUG("%s\n", __func__);
    init_eeprom(&BL24C64A);

    reload_idu_current();

    IInitAtLib(&at_obj, kAtNormalMode, NULL, debug_printf);
    IInitAtLib(&output_obj, kAtNormalMode, NULL, output_printf);

    while (1) {

        if (0 != GetRxlen(&uart0_output)) {
            debug_printf("Rec SignalBoard: %s\r\n", GetRxData(&uart0_output));
            // ICmdLinesInput(GetRxData(&uart0_output));
            IAtCmdDecodeAndRun(&output_obj, GetRxData(&uart0_output));
            ClearRxData(&uart0_output);
        }

        if (0 != GetRxlen(&uart1_debug)) {
            // debug_printf("cmd rec->:%s\r\n", GetRxData(&uart1_debug));
            // ICmdLinesInput(GetRxData(&uart1_debug));
            debug_printf("AT rec->:%s\r\n", GetRxData(&uart1_debug));
            IAtCmdDecodeAndRun(&at_obj, GetRxData(&uart1_debug));
            ClearRxData(&uart1_debug);
        }
        vTaskDelay(5);
    }
}
