/**
 * @file BoardInit.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-03-22
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "main.h"
#include "gd32f30x.h"
#include "gd32f307c_eval.h"
#include "BoardInit.h"
#include "Common.h"

/**
 * @brief -We
 * Suppresses the warning messages given when an extended constant expression is used in an initializer (see C language extensions) that other C compilers are not required by the standard to accept:
 * C2564W: extended constant initialiser used
 */

#pragma diag_suppress 1296 //Suppress warning message: extended constant initialiser used

static UartBuffer uart0_rx_buffer = {0};
static UartBuffer uart1_rx_buffer = {0};

const Uarter uart0_output = {
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

const Uarter uart1_debug = {
    .uart_periph = RCU_USART1,
    .dma_periph = RCU_DMA0,
    .gpio_periph = RCU_GPIOA,
    .uart_base = USART1,
    .dma_base = DMA0,
    .Irqn = USART1_IRQn,
    .uart_interrupt_type = USART_INT_IDLE,
    .uart_interrupt_flag = USART_INT_FLAG_IDLE,
    .baudrate = 115200U,
    .uart_periph_address = USART1_DATA_ADDRESS,
    .tx_port = GPIOA,
    .tx_pin = GPIO_PIN_2,
    .tx_mode = GPIO_MODE_AF_PP,
    .tx_speed = GPIO_OSPEED_50MHZ,
    .rx_port = GPIOA,
    .rx_pin = GPIO_PIN_3,
    .rx_mode = GPIO_MODE_IN_FLOATING,
    .rx_speed = GPIO_OSPEED_50MHZ,
    .dma_tx_channel = DMA_CH6,
    .dma_rx_channel = DMA_CH5,
    .p_rx_buffer = &uart1_rx_buffer,
    .p_tx_buffer = NULL,
};

const fan_timer_config_t fan1_pwm = {
    .timer_clock = RCU_TIMER0,
    .timer_base = TIMER0,
    .timer_channel = TIMER_CH_0,
    .gpio_port = GPIOA,
    .gpio_pin = GPIO_PIN_8,
};

const fan_timer_config_t fan1_FG = {
    .timer_clock = RCU_TIMER2,
    .timer_base = TIMER2,
    .timer_channel = TIMER_CH_1,
    .gpio_port = GPIOB,
    .gpio_pin = GPIO_PIN_5,
};

const fan_timer_config_t fan2_pwm = {
    .timer_clock = RCU_TIMER0,
    .timer_base = TIMER0,
    .timer_channel = TIMER_CH_3,
    .gpio_port = GPIOA,
    .gpio_pin = GPIO_PIN_11,
};

const fan_timer_config_t fan2_FG = {
    .timer_clock = RCU_TIMER1,
    .timer_base = TIMER1,
    .timer_channel = TIMER_CH_0,
    .gpio_port = GPIOA,
    .gpio_pin = GPIO_PIN_15,
};

gpio_config_t gpio_config_table[] = {
    {SLAVE_3_3_EN_PORT, SLAVE_3_3_EN_PIN,   GPIO_MODE_OUT_PP,       GPIO_OSPEED_50MHZ, RESET},
    {MCU_GPIO_1_PORT,   MCU_GPIO_1_PIN,     GPIO_MODE_OUT_PP,       GPIO_OSPEED_50MHZ, RESET},
    {GD32_BOOT_PORT,    GD32_BOOT_PIN,      GPIO_MODE_IN_FLOATING,  GPIO_OSPEED_50MHZ, RESET},
    {MCU_GPIO_3_PORT,   MCU_GPIO_3_PIN,     GPIO_MODE_OUT_PP,       GPIO_OSPEED_50MHZ, RESET},
    {LT6911C_RST_PORT,  LT6911C_RST_PIN,    GPIO_MODE_OUT_PP,       GPIO_OSPEED_50MHZ, RESET},
    {PANEL_12V_ON_PORT, PANEL_12V_ON_PIN,   GPIO_MODE_OUT_PP,       GPIO_OSPEED_50MHZ, SET},
    {LED_PORT,          LED_PIN,            GPIO_MODE_OUT_PP,       GPIO_OSPEED_50MHZ, SET},
};

void application_init()
{
    /* initilize the LEDs, USART and key */
    semaphore_init();

    /* initilize the USART */
    uarter_init(&uart1_debug);

    gpio_table_init(gpio_config_table, ARRAYNUM(gpio_config_table));

    fan_timer_pwm_config(&fan1_pwm);
    fan_timer_pwm_config(&fan2_pwm);
    Set_fan_timer_pwm(&fan1_pwm, 80);
    Set_fan_timer_pwm(&fan2_pwm, 80);

    fan_timer_FG_config(&fan1_FG);
    fan_timer_FG_config(&fan2_FG);

    debug_printf("Signal Board %s finished\r\n", __func__);
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    debug_printf("CK_SYS is %d\n", rcu_clock_freq_get(CK_SYS));
    debug_printf("CK_AHB is %d\n", rcu_clock_freq_get(CK_AHB));
    debug_printf("CK_APB1 is %d\n", rcu_clock_freq_get(CK_APB1));
    debug_printf("CK_APB2 is %d\n", rcu_clock_freq_get(CK_APB2));
}

void USART1_IRQHandler(void)
{
    uarter_IRQ(&uart1_debug);
}

uint16_t readvalue1 = 0, readvalue2 = 0;
__IO uint16_t ccnumber = 0;
__IO uint32_t count = 0;
__IO uint16_t fre = 0;
void TIMER2_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH1))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH1);

        if (0 == ccnumber)
        {
            /* read channel 0 capture value */
            readvalue1 = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_1) + 1;
            ccnumber = 1;
        }
        else if (1 == ccnumber)
        {
            /* read channel 0 capture value */
            readvalue2 = timer_channel_capture_value_register_read(TIMER2, TIMER_CH_1) + 1;

            if (readvalue2 > readvalue1)
            {
                count = (readvalue2 - readvalue1);
            }
            else
            {
                count = ((0xFFFFU - readvalue1) + readvalue2);
            }

            fre = 1000000U / count;
            ccnumber = 0;
        }
    }
}
