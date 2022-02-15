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
#include "gd32f307c_eval.h"

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

    /* initilize the LEDs, USART and key */
    gd_eval_com_init(EVAL_COM0);
    // gd_eval_led_init(LED2);
    // gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);

    semaphore_init();

    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    debug_printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    debug_printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    debug_printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
    debug_printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));

    xTaskCreate(TaskIdle, "TaskIdle", TASK_IDLE_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(TaskUartDecode, "TaskUartDecode", TASK_UART_DECODE_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(TaskSectionCurrent, "TaskSectionCurrent", TASK_SECTION_CURENT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    for (;;)
        ;
}
