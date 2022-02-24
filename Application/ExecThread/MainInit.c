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
#include "ExecThread.h"

/**
 * @brief -We
 * Suppresses the warning messages given when an extended constant expression is used in an initializer (see C language extensions) that other C compilers are not required by the standard to accept:
 * C2564W: extended constant initialiser used
 */

#pragma diag_suppress 1296 //Suppress warning message: extended constant initialiser used

static UartBuffer uart0_rx_buffer = {0};
static UartBuffer uart1_rx_buffer = {0};

const Uarter uart0_debug = {
    .uart_periph = RCU_USART0,
    .dma_periph = RCU_DMA0,
    .gpio_periph = RCU_GPIOA,
    .uart_base = USART0,
    .dma_base = DMA0,
    .Irqn = USART0_IRQn,
    .uart_interrupt_type = USART_INT_IDLE,
    .uart_interrupt_flag = USART_INT_FLAG_IDLE,
    .baudrate = 115200U,
    .uart_periph_address = USART0_DATA_ADDRESS,
    .tx_port = GPIOA,
    .tx_pin = GPIO_PIN_9,
    .tx_mode = GPIO_MODE_AF_PP,
    .tx_speed = GPIO_OSPEED_50MHZ,
    .rx_port = GPIOA,
    .rx_pin = GPIO_PIN_10,
    .rx_mode = GPIO_MODE_IN_FLOATING,
    .rx_speed = GPIO_OSPEED_50MHZ,
    .dma_tx_channel = DMA_CH3,
    .dma_rx_channel = DMA_CH4,
    .p_rx_buffer = &uart0_rx_buffer,
    .p_tx_buffer = NULL,
};

const Uarter uart1_Athena = {
    .uart_periph = RCU_USART1,
    .dma_periph = RCU_DMA0,
    .gpio_periph = RCU_GPIOA,
    .uart_base = USART0,
    .dma_base = DMA0,
    .Irqn = USART1_IRQn,
    .uart_interrupt_type = USART_INT_IDLE,
    .uart_interrupt_flag = USART_INT_FLAG_IDLE,
    .baudrate = 115200U,
    .uart_periph_address = USART1_DATA_ADDRESS,
    .tx_port = GPIOA,
    .tx_pin = GPIO_PIN_3,
    .tx_mode = GPIO_MODE_AF_PP,
    .tx_speed = GPIO_OSPEED_50MHZ,
    .rx_port = GPIOA,
    .rx_pin = GPIO_PIN_2,
    .rx_mode = GPIO_MODE_IN_FLOATING,
    .rx_speed = GPIO_OSPEED_50MHZ,
    .dma_tx_channel = DMA_CH1,
    .dma_rx_channel = DMA_CH2,
    .p_rx_buffer = &uart1_rx_buffer,
    .p_tx_buffer = NULL,
};

void application_init()
{
    /* initilize the LEDs, USART and key */
    semaphore_init();

    /* initilize the USART */
    uarter_init(&uart0_debug);

    uarter_init(&uart1_Athena);

    Whole_MCU_init();

    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    debug_printf("CK_SYS is %d\n", rcu_clock_freq_get(CK_SYS));
    debug_printf("CK_AHB is %d\n", rcu_clock_freq_get(CK_AHB));
    debug_printf("CK_APB1 is %d\n", rcu_clock_freq_get(CK_APB1));
    debug_printf("CK_APB2 is %d\n", rcu_clock_freq_get(CK_APB2));
}

void USART0_IRQHandler(void)
{
    uarter_IRQ(&uart0_debug);
}

void EXTI5_9_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_5))
    {
        exti_interrupt_flag_clear(EXTI_5);
    }
    if (RESET != exti_interrupt_flag_get(EXTI_6))
    {
        exti_interrupt_flag_clear(EXTI_6);
    }
    if (RESET != exti_interrupt_flag_get(EXTI_7))
    {
        exti_interrupt_flag_clear(EXTI_7);
    }
}
