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

#ifdef DEBUG
#define assert(condition)                                   \
    do                                                      \
    {                                                       \
        if (condition)                                      \
            ;                                               \
        else                                                \
        {                                                   \
            printf("\nERROR:%s, %d\n", __FILE__, __LINE__); \
            for (;;)                                        \
                ;                                           \
        }                                                   \
    } while (0)
#else
#define assert(condition)
#endif

#define GET_SysTick (uint32_t)(SysTick->VAL)
// #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)   //获取文件名 linux下
// #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) //获取文件名 windows下

#ifdef DEBUG
/**
 * @brief 假设没有中断的影响，时间精度是1Us
 * 
 */
#define GET_TIME(func, ...)                                                  \
    do                                                                       \
    {                                                                        \
        uint32_t start = 0, end = 0;                                         \
        start = GET_SysTick;                                                 \
        func(__VA_ARGS__);                                                   \
        end = GET_SysTick;                                                   \
        printf("%s-->%s-->%dUs\r\n", __FILENAME__, #func, abs(end - start)); \
    } while (0)
#else
#define GET_TIME(func, ...) func(__VA_ARGS__);
#endif

#ifdef DEBUG
// 1Us
#define GET_NOP_TIME                                                         \
    do                                                                       \
    {                                                                        \
        uint32_t start = 0, end = 0;                                         \
        start = GET_SysTick;                                                 \
        end = GET_SysTick;                                                   \
        printf("%s-->%s-->%dUs\r\n", __FILENAME__, "NOP", abs(end - start)); \
    } while (0)
#else
#define GET_NOP_TIME
#endif

void debug_printf(const char *fmt, ...);
void semaphore_init(void);

#endif
