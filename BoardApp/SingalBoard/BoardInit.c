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

static timer_capture_calc_t fan1_fg_calc = {0};
static timer_capture_calc_t fan2_fg_calc = {0};

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
    .timer_IRQ = TIMER2_IRQn,
    .channel_interrupt_flag = TIMER_INT_FLAG_CH1,
    .channel_interrupt_enable = TIMER_INT_CH1,
    .p_st_calc = &fan1_fg_calc,
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
    .timer_IRQ = TIMER1_IRQn,
    .channel_interrupt_flag = TIMER_INT_FLAG_CH0,
    .channel_interrupt_enable = TIMER_INT_CH0,
    .p_st_calc = &fan2_fg_calc,
};

gpio_config_t gpio_config_table[] = {
    {SLAVE_3_3_EN_PORT, SLAVE_3_3_EN_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SET},
    {MCU_GPIO_1_PORT, MCU_GPIO_1_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {GD32_BOOT_PORT, GD32_BOOT_PIN, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, RESET},
    {MCU_GPIO_3_PORT, MCU_GPIO_3_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {LT6911C_RST_PORT, LT6911C_RST_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SET},
    {PANEL_12V_ON_PORT, PANEL_12V_ON_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SET},
    {LED_PORT, LED_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SET},
};

void application_init()
{
    /* initilize the LEDs, USART and key */
    semaphore_init();

    /* initilize the USART */
    uarter_init(&uart1_debug);

    /* GPIO remap */
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP, ENABLE);
    gpio_pin_remap_config(GPIO_TIMER1_PARTIAL_REMAP0, ENABLE);

    gpio_table_init(gpio_config_table, ARRAYNUM(gpio_config_table));

    fan_timer_pwm_config(&fan1_pwm);
    fan_timer_pwm_config(&fan2_pwm);
    Set_fan_timer_pwm(&fan1_pwm, 80);
    Set_fan_timer_pwm(&fan2_pwm, 80);

    fan_timer_FG_config(&fan1_FG);
    fan_timer_FG_config(&fan2_FG);

    // vTaskDelay(3000);
    debug_printf("Signal Board Init %s finished\r\n", __func__);
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    // debug_printf("CK_SYS is %d\n", rcu_clock_freq_get(CK_SYS));
    // debug_printf("CK_AHB is %d\n", rcu_clock_freq_get(CK_AHB));
    // debug_printf("CK_APB1 is %d\n", rcu_clock_freq_get(CK_APB1));
    // debug_printf("CK_APB2 is %d\n", rcu_clock_freq_get(CK_APB2));
}

void USART1_IRQHandler(void)
{
    uarter_IRQ(&uart1_debug);
}

void TIMER2_IRQHandler(void)
{
    timer_input_capture_IRQ(&fan1_FG);
}

void TIMER1_IRQHandler(void)
{
    timer_input_capture_IRQ(&fan2_FG);
}
