/**
 * @file MainInit.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-17
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "main.h"
#include "gd32f30x.h"
#include "gd32f307c_eval.h"



void application_init()
{
    /* initilize the LEDs, USART and key */
    gd_eval_com_init(EVAL_COM0);
    // gd_eval_led_init(LED2);
    // gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    semaphore_init();

    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    debug_printf("CK_SYS is %d\n", rcu_clock_freq_get(CK_SYS));
    debug_printf("CK_AHB is %d\n", rcu_clock_freq_get(CK_AHB));
    debug_printf("CK_APB1 is %d\n", rcu_clock_freq_get(CK_APB1));
    debug_printf("CK_APB2 is %d\n", rcu_clock_freq_get(CK_APB2));


}
