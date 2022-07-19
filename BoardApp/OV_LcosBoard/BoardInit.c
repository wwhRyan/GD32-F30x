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
#include "i2c.h"
#include "eeprom.h"
#include "ovp921.h"
#include "basicApp.h"

/**
 * @brief -We
 * Suppresses the warning messages given when an extended constant expression is used in an initializer (see C language extensions) that other C compilers are not required by the standard to accept:
 * C2564W: extended constant initialiser used
 */

#pragma diag_suppress 1296 // Suppress warning message: extended constant initialiser used

static UartBuffer uart0_rx_buffer = {0};
static UartBuffer uart1_rx_buffer = {0};

static timer_capture_calc_t cw_wheel_fg_calc = {0};

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

const fan_timer_config_t cw_wheel_pwm = {
    .timer_clock = RCU_TIMER1,
    .timer_base = TIMER1,
    .timer_channel = TIMER_CH_3,
    .gpio_port = FAN_PWM_PORT,
    .gpio_pin = FAN_PWM_PIN,
};

const fan_timer_config_t cw_wheel_fg = {
    .timer_clock = RCU_TIMER0,
    .timer_base = TIMER0,
    .timer_channel = TIMER_CH_3,
    .gpio_port = FG_PORT,
    .gpio_pin = FG_PIN,
    .timer_IRQ = TIMER0_Channel_IRQn,
    .channel_interrupt_flag = TIMER_INT_FLAG_CH3,
    .channel_interrupt_enable = TIMER_INT_CH3,
    .p_st_calc = &cw_wheel_fg_calc,
};

const ntc_adc_config_t red_ld_ntc = {
    .adc_clock = RCU_ADC0,
    .adc_base = ADC0,
    .adc_channel = ADC_CHANNEL_8,
    .gpio_port = R_LD_NTC_PORT,
    .gpio_pin = R_LD_NTC_PIN,
};

const ntc_adc_config_t green_led_ntc = {
    .adc_clock = RCU_ADC0,
    .adc_base = ADC0,
    .adc_channel = ADC_CHANNEL_9,
    .gpio_port = G_LED_NTC_PORT,
    .gpio_pin = G_LED_NTC_PIN,
};

const ntc_adc_config_t evn_ntc = {
    .adc_clock = RCU_ADC0,
    .adc_base = ADC0,
    .adc_channel = ADC_CHANNEL_1,
    .gpio_port = EVN_NTC_PORT,
    .gpio_pin = EVN_NTC_PIN,
};

const ntc_adc_config_t lcos_panel_ntc = {
    .adc_clock = RCU_ADC0,
    .adc_base = ADC0,
    .adc_channel = ADC_CHANNEL_4,
    .gpio_port = LCOS_PANEL_NTC_PORT,
    .gpio_pin = LCOS_PANEL_NTC_PIN,
};

const dac_t laser_dac = {
    .dac_clock = RCU_DAC,
    .dac_base = DAC1,
    .gpio_port = DAC1_PORT,
    .gpio_pin = DAC1_PIN,
};

const SoftwareI2C ovp921_i2c = {
    .sda_port = SCCB_SDA_PORT,
    .sda_pin = SCCB_SDA_PIN,
    .scl_port = SCCB_CLK_PORT,
    .scl_pin = SCCB_CLK_PIN,
    .delay_time = SCCB_DELAY_TIME,
};

const SoftwareI2C sensor_i2c = {
    .sda_port = SENSOR_SDA_PORT,
    .sda_pin = SENSOR_SDA_PIN,
    .scl_port = SENSOR_SCL_PORT,
    .scl_pin = SENSOR_SCL_PIN,
    .delay_time = SCCB_DELAY_TIME,
};

const eeprom_model_t BL24C64A = {
    .i2c_addr = EEPROM_ADDRESS,
    .i2c_addr_type = REG_ADDR_2BYTE,
    .lock = eeprom_lock,
    .page_size = 32,
    .write_delay_time = 3 + 3,
    .i2c = &ovp921_i2c,
};

const exti_gpio_t R_pwm_led = {
    .gpio_port = R_LED_PWM_PORT,
    .gpio_pin = R_LED_PWM_PIN,
    .gpio_clk = RCU_GPIOA,
    .exti_line = EXTI_8,
    .port_source = GPIO_PORT_SOURCE_GPIOA,
    .pin_source = GPIO_PIN_SOURCE_8,
    .gpio_IRQ = EXTI5_9_IRQn,
};

const exti_gpio_t G_pwm_led = {
    .gpio_port = G_LED_PWM_PORT,
    .gpio_pin = G_LED_PWM_PIN,
    .gpio_clk = RCU_GPIOA,
    .exti_line = EXTI_6,
    .port_source = GPIO_PORT_SOURCE_GPIOA,
    .pin_source = GPIO_PIN_SOURCE_6,
    .gpio_IRQ = EXTI5_9_IRQn,
};

const exti_gpio_t B_pwm_led = {
    .gpio_port = B_LED_PWM_PORT,
    .gpio_pin = B_LED_PWM_PIN,
    .gpio_clk = RCU_GPIOA,
    .exti_line = EXTI_7,
    .port_source = GPIO_PORT_SOURCE_GPIOA,
    .pin_source = GPIO_PIN_SOURCE_7,
    .gpio_IRQ = EXTI5_9_IRQn,
};

gpio_config_t gpio_config_table[] = {
    // output
    {LD_EN_H_PORT, LD_EN_H_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {LD_EN_L_PORT, LD_EN_L_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {DISCHARGE_PORT, DISCHARGE_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {DISCHARGE2_PORT, DISCHARGE2_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {SYS_12V_ON_PORT, SYS_12V_ON_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SET},
    {OVP921_RESET_PORT, OVP921_RESET_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SET},
    {MCU_GPIO_INT_PORT, MCU_GPIO_INT_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},

    // input
    {HW_VER0_PORT, HW_VER0_PIN, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, RESET},
    {HW_VER1_PORT, HW_VER1_PIN, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, RESET},

    {RESERVERD_1_PORT, RESERVERD_1_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {RESERVERD_2_PORT, RESERVERD_2_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},
    {EE_WP_PORT, EE_WP_PIN, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET},

};

SemaphoreHandle_t uart_Semaphore = NULL;
SemaphoreHandle_t i2c_Semaphore = NULL;

// Declare a variable to hold the created event group.
EventGroupHandle_t sys_sig;

void system_ipc_init(void)
{
    uart_Semaphore = xSemaphoreCreateMutex();
    E_assert(uart_Semaphore != NULL);
    i2c_Semaphore = xSemaphoreCreateMutex();
    E_assert(i2c_Semaphore != NULL);

    // Attempt to create the event group.
    sys_sig = xEventGroupCreate();
    E_assert(sys_sig != NULL);
    set_sig(sys_sig, sig_lightsource, true);
    clear_sig(sys_sig, sig_ovp921_status);
    clear_sig(sys_sig, sig_lightsource);
}

void omnivision_lcos_init(void);

void application_init()
{
    /* initilize the LEDs, USART and key */
    system_ipc_init();
    // rcu_periph_clock_enable(RCU_CRC);

    /* initilize the USART */
    uarter_init(&uart0_output);
    uarter_init(&uart1_debug);

    log_init();

    /* GPIO remap */
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    gpio_pin_remap_config(GPIO_TIMER1_PARTIAL_REMAP1, ENABLE);

    gpio_table_init(gpio_config_table, ARRAYNUM(gpio_config_table));
    extern_gpio_interrupt_init(&R_pwm_led);
    extern_gpio_interrupt_init(&G_pwm_led);
    extern_gpio_interrupt_init(&B_pwm_led);

    fan_timer_pwm_config(&cw_wheel_pwm);
    Set_fan_timer_pwm(&cw_wheel_pwm, 90);

    fan_timer_FG_config(&cw_wheel_fg);

    ntc_adc_config(&red_ld_ntc);
    ntc_adc_config(&green_led_ntc);
    ntc_adc_config(&evn_ntc);
    ntc_adc_config(&lcos_panel_ntc);

    laser_dac_init(&laser_dac);
    // laser_dac_set_value(&laser_dac, 4095);
    laser_dac_set(2.00);

    INewSoftwareI2C(&ovp921_i2c);

    printf("OV Lcos Board %s finished\r\n", __func__);
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("CK_SYS is %d\n", rcu_clock_freq_get(CK_SYS));
    printf("CK_AHB is %d\n", rcu_clock_freq_get(CK_AHB));
    printf("CK_APB1 is %d\n", rcu_clock_freq_get(CK_APB1));
    printf("CK_APB2 is %d\n", rcu_clock_freq_get(CK_APB2));

    init_eeprom(&BL24C64A);
    omnivision_lcos_init();
}

void USART0_IRQHandler(void)
{
    uarter_IRQ(&uart0_output);
}

void USART1_IRQHandler(void)
{
    uarter_IRQ(&uart1_debug);
}

void TIMER0_Channel_IRQHandler(void)
{
    timer_input_capture_IRQ(&cw_wheel_fg);
}

void EXTI5_9_IRQHandler(void)
{

    if (RESET != exti_interrupt_flag_get(EXTI_6))
    {
        color_EN_EXIT_IRQ(GREEN);
        exti_interrupt_flag_clear(EXTI_6);
    }
    if (RESET != exti_interrupt_flag_get(EXTI_7))
    {
        color_EN_EXIT_IRQ(BLUE);
        exti_interrupt_flag_clear(EXTI_7);
    }
    if (RESET != exti_interrupt_flag_get(EXTI_8))
    {
        color_EN_EXIT_IRQ(RED);
        exti_interrupt_flag_clear(EXTI_8);
    }
}

void output_printf(const char *fmt, ...)
{
    char buf[256] = {0};

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    uarter_send(&uart0_output, buf, strlen(buf));
}
