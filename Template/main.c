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

#include "main.h"
#include "Common.h"
#include "gd32f30x.h"
#include "systick.h"
#include "timers.h"
#include "ulog.h"

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

    E_assert(xTaskCreate(ThreadUartEvent, "uarteventthread", THREAD_UART_EVENT_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &ThreadUartEventHandle));
    E_assert(xTaskCreate(ThreadFirstConsumer, "firstconsumerthread", THREAD_FIRST_CONSUMER_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &ThreadFirstConsumerHandle));
    E_assert(xTaskCreate(ThreadSecondConsumer, "secondconsumerthread", THREAD_SECOND_CONSUMER_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &ThreadSecondConsumerHandle));

#if 0
    TimerHandle_t xTimer;

    xTimer = xTimerCreate("timer", pdMS_TO_TICKS(20000), pdTRUE, NULL, TimerCallFunc);
    E_assert(xTimer != NULL);

    xTimerStart(xTimer, 0);
#endif

    vTaskStartScheduler();

    for (;;)
        ;
}

#if 0 /* 优先级为0，最低，后续可以移动至空闲任务钩子函数 */
void TimerCallFunc(TimerHandle_t xTimer)
{
#ifdef DEBUG
    ULOG_DEBUG("ThreadUartEvent min free stack size %d\r\n", (int)uxTaskGetStackHighWaterMark(ThreadUartEventHandle));
    ULOG_DEBUG("ThreadFirstConsumer min free stack size %d\r\n", (int)uxTaskGetStackHighWaterMark(ThreadFirstConsumerHandle));
    ULOG_DEBUG("ThreadSecondConsumer min free stack size %d\r\n", (int)uxTaskGetStackHighWaterMark(ThreadSecondConsumerHandle));
#endif

#if 0 /* 过程中会禁用中断，仅仅调试使用 */
    char tasks_buf[512] = { 0 };
       
    strcat((char*)tasks_buf, "任务名称\t运行状态\t优先级\t剩余堆栈\t任务序号\r\n");
    strcat((char*)tasks_buf, "---------------------------------------------\r\n");

    /* The list of tasks and their status */
    vTaskList((char*)(tasks_buf + strlen(tasks_buf)));
    strcat((char*)tasks_buf, "---------------------------------------------\r\n");

    // strcat((char *)tasks_buf, "B : Blocked, R : Ready, D : Deleted, S : Suspended\r\n");
    strcat((char*)tasks_buf, "B : 阻塞, R : 就绪, D : 删除, S : 暂停\r\n");
    printf("%s\r\n", tasks_buf);
#endif
}

#endif
