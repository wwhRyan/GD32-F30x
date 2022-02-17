/**
 * @file main.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-15
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "gd32f30x.h"
#include "systick.h"
#include "main.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* 设置优先级分组为4，16个优先级都是抢占优先级 */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    // init all the peripherals.
    application_init();

    xTaskCreate(TaskIdle, "TaskIdle", TASK_IDLE_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(TaskUartDecode, "TaskUartDecode", TASK_UART_DECODE_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(TaskSectionCurrent, "TaskSectionCurrent", TASK_SECTION_CURENT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    for (;;)
        ;
}
