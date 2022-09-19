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
#include <string.h>

#define EEPROM_W_QUEUE_LENGTH 5

extern eeprom_t eeprom;
extern const Uarter uart0_output;
SemaphoreHandle_t uart_Semaphore = NULL;
SemaphoreHandle_t i2c_Semaphore = NULL;
EventGroupHandle_t sys_sig = NULL;
QueueHandle_t xQueue_eeprom = NULL;

void system_ipc_init(void)
{
    uart_Semaphore = xSemaphoreCreateMutex();
    E_assert(uart_Semaphore != NULL);
    i2c_Semaphore = xSemaphoreCreateMutex();
    E_assert(i2c_Semaphore != NULL);

    // Attempt to create the event group.
    sys_sig = xEventGroupCreate();
    E_assert(sys_sig != NULL);
    clear_sig(sys_sig, sig_rdc200a_status);
    set_sig(sys_sig, sig_lightsource, true);
    set_sig(sys_sig, sig_system, true);
    clear_sig(sys_sig, sig_light_status);
    clear_sig(sys_sig, sig_update_rdc200a);
    clear_sig(sys_sig, sig_update_firmware);
    clear_sig(sys_sig, sig_eeprom_write);
    set_sig(sys_sig, sig_slient_async_msg, false);

    xQueue_eeprom = xQueueCreate(EEPROM_W_QUEUE_LENGTH, sizeof(mem_t));
    E_assert(xQueue_eeprom);
    ULOG_INFO("xQueue_eeprom create\n");
}

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

void output_printf(const char* fmt, ...)
{
    char buf[1280] = { 0 };

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    uarter_send(&uart0_output, buf, strlen(buf));
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
    char_replace(new_line.text, '\n', '\0');
    strncpy(new_line.text, msg, TEXT_NUMBER);

    output_printf("AT+Error#Time:%.2d-%02d-%02d,Text:%s\n", new_line.time / 60 / 60, (new_line.time / 60) % 60, new_line.time % 60, new_line.text);
    file_append_line(&eeprom_log, &new_line);
}

void log_read(void* pdata, size_t size, uint32_t addr)
{
    eeprom_block_read(&BL24C64A, addr, pdata, size);
}
void log_write(void* pdata, size_t size, uint32_t addr)
{
    eeprom_block_write(&BL24C64A, addr, pdata, size);
}

void log_init(file_t* pfile)
{
    ULOG_INIT();
    file_init(pfile, LOG_START_ADDR, 1024 * 6, log_read, log_write, get_LSB_array_crc);
    ULOG_SUBSCRIBE(my_console_logger, ULOG_DEBUG_LEVEL);
    ULOG_SUBSCRIBE(my_file_logger, ULOG_WARNING_LEVEL);
    ULOG_INFO("ULOG init\n"); // logs to file and console
}
