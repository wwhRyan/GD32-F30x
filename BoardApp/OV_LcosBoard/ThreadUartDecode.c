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
extern const Uarter uart0_output;
extern const Uarter uart1_debug;

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

        if (0 != GetRxlen(&uart0_output))
        {
            debug_printf("%s\r\n", GetRxData(&uart0_output));
            ICmdLinesInput(GetRxData(&uart0_output));
            IAtCmdDecodeAndRun(&at_obj, str);
            ClearRxData(&uart0_output);
        }

        if(0 != GetRxlen(&uart1_debug))
        {
            debug_printf("%s\r\n", GetRxData(&uart1_debug));
            ICmdLinesInput(GetRxData(&uart1_debug));
            IAtCmdDecodeAndRun(&at_obj, str);
            ClearRxData(&uart1_debug);
        }

        vTaskDelay(10);
    }
}
