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

    for (int i = 0; i < 10; i++)
    {
        debug_printf("%ds\r\n", i);
        vTaskDelay(1000);
        if (get_ovp921_status() == true)
        {
            laser_on();
            debug_printf("laser on\r\n");
            break;
        }
        else
        {
            laser_off();
            debug_printf("laser off\r\n");
        }
    }
    IInitAtLib(&at_obj, kAtNormalMode, NULL, debug_printf);

    while (1)
    {
#if 0
        char str[0xff] = {0};

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
