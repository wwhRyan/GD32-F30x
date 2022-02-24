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
#include "ExecThread.h"

asAtProtocol at_obj;
extern const Uarter uart0_debug;

void TaskUartDecode(void *pvParameters)
{
    IInitAtLib(&at_obj, kAtNormalMode, NULL, debug_printf);

    char str[0xff] = {0};

    while (1)
    {
#if 0
        debug_printf("Please Input:\n");
        scanf("%200s", str);
        str[strlen(str)] = '\n'; //add '\n' to end of str
        debug_printf("Input:%s\n", str);
        ICmdLinesInput(str);
        IAtCmdDecodeAndRun(&at_obj, str);
        memset(str, 0, 0xff);

        debug_printf("TaskUartDecode\r\n");
        debug_printf("TaskUartDecode min free stack size %d\r\n", (int)uxTaskGetStackHighWaterMark(NULL));
#endif

        if (0 != GetRxlen(&uart0_debug))
        {
            debug_printf("%s\r\n", GetRxData(&uart0_debug));
            ICmdLinesInput(GetRxData(&uart0_debug));
            IAtCmdDecodeAndRun(&at_obj, str);
            ClearRxData(&uart0_debug);
        }

        vTaskDelay(500);
    }
}
