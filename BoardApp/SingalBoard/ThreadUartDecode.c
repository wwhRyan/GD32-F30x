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

asAtProtocol at_obj;
extern const Uarter uart1_debug;

void TaskUartDecode(void *pvParameters)
{
    IInitAtLib(&at_obj, kAtNormalMode, NULL, debug_printf);

    char str[0xff] = {0};

    while (1)
    {

        if (0 != GetRxlen(&uart1_debug))
        {
            debug_printf("%s\r\n", GetRxData(&uart1_debug));
            ICmdLinesInput(GetRxData(&uart1_debug));
            IAtCmdDecodeAndRun(&at_obj, str);
            ClearRxData(&uart1_debug);
        }

        vTaskDelay(10);
    }
}
