/**
 * @file utils.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-02-15
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */
#ifndef _UTILS_H_
#define _UTILS_H_

#include "main.h"
#include "gd32f30x.h"
#include "ulog.h"
#include "event_groups.h"
#include "file.h"
#include "event_groups.h"
#include "semphr.h"
#include "queue.h"

// #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)   //获取文件名 linux下
// #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) //获取文件名 windows下

#define EXCUTE_ONCE(x)                     \
    do {                                   \
        static uint8_t once##__LINE__ = 0; \
        if (once##__LINE__ == 0) {         \
            once##__LINE__ = 1;            \
            x;                             \
        }                                  \
    } while (0)

#if 0
#ifdef DEBUG
/**
 * @brief 假设没有中断的影响，时间精度是1Us
 *
 */
#define GET_TIME(func, ...)                                                  \
    do {                                                                     \
        uint32_t start = 0, end = 0;                                         \
        start = (uint32_t)(SysTick->VAL);                                    \
        func(__VA_ARGS__);                                                   \
        end = (uint32_t)(SysTick->VAL);                                      \
        printf("%s-->%s-->%dUs\r\n", __FILENAME__, #func, abs(end - start)); \
    } while (0)
#else
#define GET_TIME(func, ...) func(__VA_ARGS__);
#endif
#endif

#ifdef DEBUG
// 1Us
#define GET_NOP_TIME                                                         \
    do {                                                                     \
        uint32_t start = 0, end = 0;                                         \
        start = (uint32_t)(SysTick->VAL);                                    \
        end = (uint32_t)(SysTick->VAL);                                      \
        printf("%s-->%s-->%dUs\r\n", __FILENAME__, "NOP", abs(end - start)); \
    } while (0)
#else
#define GET_NOP_TIME
#endif

extern SemaphoreHandle_t uart_Semaphore;
extern SemaphoreHandle_t i2c_Semaphore;
extern SemaphoreHandle_t lcos_i2c_Semaphore;
extern EventGroupHandle_t sys_sig;
extern QueueHandle_t xQueue_eeprom;

void debug_printf(const char* fmt, ...);
void output_printf(const char* fmt, ...);
bool get_sig(EventGroupHandle_t pEventGroup, int BitInx);
void set_sig(EventGroupHandle_t pEventGroup, int BitInx, bool status);
void clear_sig(EventGroupHandle_t pEventGroup, int BitInx);
void log_init(file_t* pfile);
void system_ipc_init(void);

#endif
