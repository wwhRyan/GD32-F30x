/**
 * @file port.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-24
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "port.h"
#include "main.h"
#include "systick.h"

void adc_config(uint32_t adc_base);
uint16_t adc_channel_sample(uint32_t adc_base, uint8_t channel);

void dac_config(void);
void capture_timer_config(void);
void PWM_timer_config(void);


const NTC_ADC_Channel_t Athena_Ntc[eMax_NTC_Channel] = {
    {ADC0, ADC_CHANNEL_8}, //eRed_laser_NTC
    {ADC0, ADC_CHANNEL_9}, //eGreen_laser_NTC
    {ADC0, ADC_CHANNEL_1}, //eEnvironmemt_NTC
    {ADC0, ADC_CHANNEL_0}, //eHardware_Version_NTC
};

/**
 * @brief function to initialize the gpio, contains some below functions
 * @note ADC
 * @note DAC
 * @note PWM out
 * @note PWM in
 * @note GPIO
 * @note UART
 * @note I2C
 * @note GPIO interrupt
 */
void Whole_MCU_init()
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    // gpio_init
    /* SWD remap */
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    /* GPIOB output */
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4);

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    gpio_init(I_SPOKER_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, I_SPOKER_PIN);

    //ADC init
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* ADC prescaler select */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
    /* config the GPIO as analog mode */
    gpio_init(EVN_NTC_PORT, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, EVN_NTC_PIN);
    gpio_init(LD_NTC_PORT, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, LD_NTC_PIN);
    gpio_init(G_LED_NTC_PORT, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, G_LED_NTC_PIN);
    adc_config(ADC0);

    // DAC init
    rcu_periph_clock_enable(RCU_DAC);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    dac_config();

    //External Interrupt init
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(R_LED_PWM_PORT, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ,
              R_LED_PWM_PIN | G_LED_PWM_PIN | B_LED_PWM_PIN);
    /* enable and set gpio EXTI interrupt to the lowest priority */
    nvic_irq_enable(EXTI5_9_IRQn, 0, 0);
    /* connect gpio EXTI line to gpio pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_5);
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_6);
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_7);
    /* configure gpio EXTI line */
    exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_init(EXTI_6, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_5);
    exti_interrupt_flag_clear(EXTI_6);
    exti_interrupt_flag_clear(EXTI_7);

    /* PWM output & PWM input */
    gpio_init(FAN_PWM_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, FAN_PWM_PIN | FG_PIN);
    capture_timer_config();
    PWM_timer_config();
}

void adc_config(uint32_t adc_base)
{
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC data alignment config */
    adc_data_alignment_config(adc_base, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(adc_base, ADC_REGULAR_CHANNEL, 1U);

    /* ADC trigger config */
    adc_external_trigger_source_config(adc_base, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    /* ADC external trigger config */
    adc_external_trigger_config(adc_base, ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable(adc_base);
    delay_1ms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(adc_base);
}

/*!
    \brief      ADC channel sample
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint16_t adc_channel_sample(uint32_t adc_base, uint8_t channel)
{
    /* ADC regular channel config */
    adc_regular_channel_config(adc_base, 0U, channel, ADC_SAMPLETIME_7POINT5);
    /* ADC software trigger enable */
    adc_software_trigger_enable(adc_base, ADC_REGULAR_CHANNEL);

    /* wait the end of conversion flag */
    while (!adc_flag_get(adc_base, ADC_FLAG_EOC))
        ;
    /* clear the end of conversion flag */
    adc_flag_clear(adc_base, ADC_FLAG_EOC);
    /* return regular channel sample value */
    return (adc_regular_data_read(adc_base));
}

uint16_t Get_NTC_Voltage(int ntc_idx)
{
    return adc_channel_sample(Athena_Ntc[ntc_idx].adc_base, Athena_Ntc[ntc_idx].channel);
}

void dac_config(void)
{
    dac_deinit();
    /* configure the DAC0 */
    dac_trigger_source_config(DAC0, DAC_TRIGGER_SOFTWARE);
    dac_trigger_enable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_DISABLE);

    /* enable DAC0 and set data */
    dac_enable(DAC0);
    dac_data_set(DAC0, DAC_ALIGN_12B_R, 0);
}

void Set_DAC_Value(uint16_t value)
{
    dac_data_set(DAC0, DAC_ALIGN_12B_R, value);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PWM_timer_config()
{
    /* -----------------------------------------------------------------------
    TIMER0 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER0CLK = SystemCoreClock / 120 = 1MHz

    TIMER0 channel0 duty cycle = (4000/ 16000)* 100  = 25%
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler = 119;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 15999;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH3 configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocintpara);

    /* CH3 configuration in PWM mode0,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 3999);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);
}

void Set_DAC_PWM_percent(uint16_t percent)
{
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, (uint32_t)((uint32_t)percent * 15999 / 100));
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void capture_timer_config()
{
    /* TIMER0 configuration: input capture mode -------------------
    the external signal is connected to TIMER0 CH0 pin (PB4)
    the rising edge is used as active edge
    the TIMER0 CH3CV is used to compute the frequency value
    ------------------------------------------------------------ */
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler = 119;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 65535;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* TIMER0  configuration */
    /* TIMER0 CH0 input capture configuration */
    timer_icinitpara.icpolarity = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter = 0x0;
    timer_input_capture_config(TIMER0, TIMER_CH_3, &timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    /* clear channel 0 interrupt bit */
    // timer_interrupt_flag_clear(TIMER0,TIMER_INT_FLAG_CH3);
    /* channel 0 interrupt enable */
    // timer_interrupt_enable(TIMER0,TIMER_INT_CH3);

    /* TIMER0 counter enable */
    timer_enable(TIMER0);
}

uint32_t Get_FAN_FG()
{
    return timer_channel_capture_value_register_read(TIMER0, TIMER_CH_3) + 1;
}
