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
#include "timers.h"

TaskHandle_t ThreadFirstConsumerHandle = NULL;
TaskHandle_t ThreadUartEventHandle = NULL;
TaskHandle_t ThreadSecondConsumerHandle = NULL;

void TimerCallFunc(TimerHandle_t xTimer);

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

    xTaskCreate(ThreadUartEvent, "THREADUARTEVENT", THREAD_UART_EVENT_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &ThreadUartEventHandle);
    xTaskCreate(ThreadFirstConsumer, "ThreadFirstConsumer", THREAD_FIRST_CONSUMER_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &ThreadFirstConsumerHandle);
    xTaskCreate(ThreadSecondConsumer, "ThreadSecondConsumer", THREAD_SECOND_CONSUMER_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &ThreadSecondConsumerHandle);

#if 0
    TimerHandle_t xTimer;

    xTimer = xTimerCreate("Timer", pdMS_TO_TICKS(20000), pdTRUE, NULL, TimerCallFunc);
    assert(xTimer != NULL);

    xTimerStart(xTimer, 0);
#endif

    vTaskStartScheduler();

    for (;;)
        ;
}

#if 0
void TimerCallFunc(TimerHandle_t xTimer)
{
    debug_printf("ThreadFirstConsumer min free stack size %d\r\n", (int)uxTaskGetStackHighWaterMark(ThreadFirstConsumerHandle));
    debug_printf("ThreadUartEvent min free stack size %d\r\n", (int)uxTaskGetStackHighWaterMark(ThreadUartEventHandle));
    debug_printf("TaskCurrent min free stack size %d\r\n", (int)uxTaskGetStackHighWaterMark(ThreadSecondConsumerHandle));
}

#endif
