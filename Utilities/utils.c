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
#include "Boardinit.h"
#include "file.h"
#include "gd32f307c_eval.h"
#include "semphr.h"
#include <string.h>

extern eeprom_t eeprom;

/**
 * @brief debug_printf is Thread safety printf function
 *
 * @param fmt
 * @param ...
 */
void debug_printf(const char* fmt, ...)
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
int fputc(int ch, FILE* f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE))
        ;

    return ch;
}

int fgetc(FILE* f)
{
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_RBNE))
        ;
    return (int)usart_data_receive(EVAL_COM1);
}

bool get_sig(EventGroupHandle_t pEventGroup, int BitInx)
{
    int EventGroup = xEventGroupGetBits(pEventGroup);

    if (EventGroup & (0x00000001 << BitInx)) {
        return true;
    } else {
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

void char_replace(char* str, char dest, char new_char)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == dest)
            str[i] = new_char;
    }
}
void my_console_logger(ulog_level_t severity, char* msg)
{
    debug_printf("%s.%s", ulog_level_name(severity), msg);
}

void my_file_logger(ulog_level_t severity, char* msg)
{
    line_t new_line;
    new_line.level = severity;
    new_line.time = eeprom.light_source_time;

    char_replace(msg, ' ', '_');
    char_replace(msg, ',', '_');
    strncpy(new_line.text, msg, TEXT_NUMBER);
    file_append_line(&eeprom_log, &new_line);
}

void log_read(void* pdata, size_t size, uint32_t addr)
{
    eeprom_block_read(&AT24C02D, addr, pdata, size);
}
void log_write(void* pdata, size_t size, uint32_t addr)
{
    eeprom_block_write(&AT24C02D, addr, pdata, size);
}

void log_init(file_t* pfile)
{
    ULOG_INIT();
    file_init(pfile, LOG_START_ADDR, 1024, log_read, log_write, get_LSB_array_crc);
    ULOG_SUBSCRIBE(my_console_logger, ULOG_DEBUG_LEVEL);
    ULOG_SUBSCRIBE(my_file_logger, ULOG_WARNING_LEVEL);
    ULOG_INFO("ULOG init\n"); // logs to file and console
}
