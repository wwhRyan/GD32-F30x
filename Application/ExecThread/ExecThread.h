/**
 * @file ExecThread.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-14
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef EXECTHREAD_H
#define EXECTHREAD_H

//configMINIMAL_STACK_SIZE
#define TASK_IDLE_STACK_SIZE 1024
#define TASK_UART_DECODE_STACK_SIZE 1024
#define TASK_SECTION_CURENT_STACK_SIZE 1024

void TaskIdle(void *pvParameters);
void TaskUartDecode(void *pvParameters);
void TaskSectionCurrent(void *pvParameters);

#endif
