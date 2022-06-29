/**
 * @file adc.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-03-22
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#include "adc_mcu.h"

void software_triggle_adc_init(rcu_periph_enum adc_clock, uint32_t adc_base, uint32_t port, uint16_t pin)
{
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, pin);

    rcu_periph_clock_enable(adc_clock);
    /* ADC prescaler select */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

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
    DelayMs(2U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(adc_base);
}

uint16_t get_adc_channel_sample(uint32_t adc_base, uint8_t channel)
{
    adc_regular_data_read(adc_base); // clear last adc data
    /* ADC regular channel config */
    adc_regular_channel_config(adc_base, 0U, channel, ADC_SAMPLETIME_239POINT5);
    /* ADC software trigger enable */
    adc_software_trigger_enable(adc_base, ADC_REGULAR_CHANNEL);

    /* wait the end of conversion flag */
    while (!adc_flag_get(adc_base, ADC_FLAG_EOC))
        vTaskDelay(1);
    ;
    /* clear the end of conversion flag */
    adc_flag_clear(adc_base, ADC_FLAG_EOC);
    /* return regular channel sample value */
    return (adc_regular_data_read(adc_base));
}

void ntc_adc_config(const ntc_adc_config_t *ntc_adc_config)
{
    software_triggle_adc_init(ntc_adc_config->adc_clock, ntc_adc_config->adc_base,
                              ntc_adc_config->gpio_port, ntc_adc_config->gpio_pin);
}
uint16_t get_ntc_adc_sample(const ntc_adc_config_t *ntc_adc_config)
{
    return get_adc_channel_sample(ntc_adc_config->adc_base, ntc_adc_config->adc_channel);
}
