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

void TaskUartDecode(void *pvParameters)
{
    asAtProtocol obj;
    IInitAtLib(&obj, kAtNormalMode, NULL, debug_printf);

    char str[0xff] = {0};

    while (1)
    {
        debug_printf("Please Input:\n");
        scanf("%s", str);
        str[strlen(str)] = '\n'; //add '\n' to end of str
        debug_printf("Input:%s\n", str);
        ICmdLinesInput(str);
        IAtCmdDecodeAndRun(&obj, str);
        memset(str, 0, 0xff);

        // debug_printf("TaskUartDecode\r\n");
        debug_printf("TaskUartDecode min free stack size %d\r\n",(int)uxTaskGetStackHighWaterMark(NULL));

        vTaskDelay(500);
    }
}
