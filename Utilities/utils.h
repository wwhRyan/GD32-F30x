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

void debug_printf(const char *fmt, ...);
void semaphore_init(void);

#endif
