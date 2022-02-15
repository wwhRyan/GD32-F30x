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

void TaskSectionCurrent(void *pvParameters)
{
        asAtProtocol obj;
    IInitAtLib(&obj, kAtNormalMode, NULL, debug_printf);

    char str[0xff] = {0};

    debug_printf("Start At Example!\n");

    while (1)
    {
        debug_printf("\nPlease Input AT cmd:\n");
        scanf("%s", str);
        str[strlen(str)] = '\n';  //add '\n' to end of str
        IAtCmdDecodeAndRun(&obj, str);
        memset(str, 0 ,0xff);

        debug_printf("TaskSectionCurrent\r\n");
        vTaskDelay(500);
    }
}
