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

#include "main.h"
#include "gd32f30x.h"
#include "AtProtocol.h"
#include "CmdLine.h"
#include "BoardInit.h"
#include "ovp921.h"
#include "basicApp.h"

asAtProtocol at_obj;
extern const Uarter uart0_output;
extern const Uarter uart1_debug;

void TaskUartDecode(void *pvParameters)
{
    reload_idu_current();

    IInitAtLib(&at_obj, kAtNormalMode, NULL, debug_printf);

    while (1)
    {

        if (0 != GetRxlen(&uart0_output))
        {
            debug_printf("Rec SignalBoard: %s\r\n", GetRxData(&uart0_output));
            // ICmdLinesInput(GetRxData(&uart0_output));
            // IAtCmdDecodeAndRun(&at_obj, str);
            ClearRxData(&uart0_output);
        }

        if (0 != GetRxlen(&uart1_debug))
        {
            debug_printf("cmd rec->:%s\r\n", GetRxData(&uart1_debug));
            ICmdLinesInput(GetRxData(&uart1_debug));
            debug_printf("AT rec->:%s\r\n", GetRxData(&uart1_debug));

            IAtCmdDecodeAndRun(&at_obj, GetRxData(&uart1_debug));
            ClearRxData(&uart1_debug);
        }
        vTaskDelay(5);
    }
}
