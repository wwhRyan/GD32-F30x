/**
 * @file utils.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-02-15
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#include "utils.h"
#include "semphr.h"
#include "gd32f307c_eval.h"
#include "Boardinit.h"

/**
 * @brief debug_printf is Thread safety printf function
 *
 * @param fmt
 * @param ...
 */
void debug_printf(const char *fmt, ...)
{
    va_list args;
    if (xSemaphoreTake(uart_Semaphore, (TickType_t)0xFFFF) == pdFALSE)
        return;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    xSemaphoreGive(uart_Semaphore);
}

/* printf is not thread safety, but can output freeRTOS bug. */
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE))
        ;

    return ch;
}

int fgetc(FILE *f)
{
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_RBNE))
        ;
    return (int)usart_data_receive(EVAL_COM1);
}

bool get_sig(EventGroupHandle_t pEventGroup, int BitInx)
{
    int EventGroup = xEventGroupGetBits(pEventGroup);

    if (EventGroup & (0x00000001 << BitInx))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void set_sig(EventGroupHandle_t pEventGroup, int BitInx, bool status)
{
    if (status)
        xEventGroupSetBits(pEventGroup, (0x00000001 << BitInx));
    else
        xEventGroupClearBits(pEventGroup, (0x00000001 << BitInx));
}

void clear_sig(EventGroupHandle_t pEventGroup, int BitInx)
{
    xEventGroupClearBits(pEventGroup, (0x00000001 << BitInx));
}
