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

#ifndef BOARDINIT_H
#define BOARDINIT_H
#include "uart.h"
#include "timer.h"
#include "adc_mcu.h"
#include "dac.h"
#include "port.h"

#ifndef ATHENA_SIGNAL_BOARD
#error "Please define ATHENA_SIGNAL_BOARD"
#endif

//configMINIMAL_STACK_SIZE
#define TASK_IDLE_STACK_SIZE configMINIMAL_STACK_SIZE
#define TASK_UART_DECODE_STACK_SIZE 1024
#define TASK_SECTION_CURENT_STACK_SIZE configMINIMAL_STACK_SIZE

void TaskIdle(void *pvParameters);
void TaskUartDecode(void *pvParameters);
void TaskSectionCurrent(void *pvParameters);

#define SLAVE_3_3_EN_PORT GPIOB
#define SLAVE_3_3_EN_PIN GPIO_PIN_8

#define MCU_GPIO_1_PORT GPIOB
#define MCU_GPIO_1_PIN GPIO_PIN_9

#define GD32_BOOT_PORT GPIOB
#define GD32_BOOT_PIN GPIO_PIN_10

#define MCU_GPIO_3_PORT GPIOB
#define MCU_GPIO_3_PIN GPIO_PIN_11

#define LT6911C_RST_PORT GPIOB
#define LT6911C_RST_PIN GPIO_PIN_12

#define PANEL_12V_ON_PORT GPIOA
#define PANEL_12V_ON_PIN GPIO_PIN_4


#endif
