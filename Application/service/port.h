/**
 * @file port.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-24
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef __PORT_H__
#define __PORT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "gd32f30x.h"

    typedef struct __NTC_ADC_Channel_t
    {
        uint32_t adc_base;
        uint8_t channel;
    } NTC_ADC_Channel_t;

    typedef enum
    {
        eRed_laser_NTC,
        eGreen_LED_NTC,
        eEnvironmemt_NTC,
        eHardware_Version_NTC,
        eMax_NTC_Channel,
    }eNTC_Idx;

/*<! Red laser NTC !>*/
#define LD_NTC_PORT GPIOB
#define LD_NTC_PIN GPIO_PIN_0

/*<! G LED NTC !>*/
#define G_LED_NTC_PORT GPIOB
#define G_LED_NTC_PIN GPIO_PIN_1

/*<! 高电平点灯 !>*/
#define LD_EN_H_PORT GPIOB
#define LD_EN_H_PIN GPIO_PIN_3

/*<! 低电平点灯 !>*/
#define LD_EN_L_PORT GPIOB
#define LD_EN_L_PIN GPIO_PIN_5

/*<!  !>*/
#define SCCB_CLK_PORT GPIOB
#define SCCB_CLK_PIN GPIO_PIN_6

/*<!  !>*/
#define SCCB_SDA_PORT GPIOB
#define SCCB_SDA_PIN GPIO_PIN_7

/*<! B-->R 大电流跳变到小电流时需要给一个脉冲，宽度暂定40uS,根据实际情况调整时间 !>*/
#define DISCHARGE_PORT GPIOB
#define DISCHARGE_PIN GPIO_PIN_8

/*<! G-->B 高电压跳变到低电压时需要给一个脉冲，宽度暂定40uS,根据实际情况调整时间 !>*/
#define DISCHARGE2_PORT GPIOB
#define DISCHARGE2_PIN GPIO_PIN_9

/*<! system switch !>*/
#define SYS_12V_ON_PORT GPIOB
#define SYS_12V_ON_PIN GPIO_PIN_10

#define EE_WP_PORT GPIOA
#define EE_WP_PIN GPIO_PIN_8

#define VOP921_RESET_PORT GPIOB
#define VOP921_RESET_PIN GPIO_PIN_12

#define MCU_R_LED_EN_PORT GPIOB
#define MCU_R_LED_EN_PIN GPIO_PIN_13

#define MCU_G_LED_EN_PORT GPIOB
#define MCU_G_LED_EN_PIN GPIO_PIN_14

#define MCU_B_LED_EN_PORT GPIOB
#define MCU_B_LED_EN_PIN GPIO_PIN_15

/*<! hardware version detect !>*/
#define HW_PORT GPIOA
#define HW_PIN GPIO_PIN_0

#define OVP2200_1_5V_EN_PORT GPIOC
#define OVP2200_1_5V_EN_PIN GPIO_PIN_14

/*<! environment !>*/
#define EVN_NTC_PORT GPIOA
#define EVN_NTC_PIN GPIO_PIN_1

/*<!  !>*/
#define GD32_UART_RX_PORT GPIOA
#define GD32_UART_RX_PIN GPIO_PIN_2

/*<!  !>*/
#define GD32_UART_TX_PORT GPIOA
#define GD32_UART_TX_PIN GPIO_PIN_3

/*<! 电流与DAC对应关系为：DAC_VALUE=(1.24/15+1.24/20-0.03*电流值）*(4095*20/3.3) !>*/
#define DAC1_PORT GPIOA
#define DAC1_PIN GPIO_PIN_4

/*<! ovp921 red light on signal !>*/
#define R_LED_PWM_PORT GPIOA
#define R_LED_PWM_PIN GPIO_PIN_5

/*<! ovp921 green light on signal !>*/
#define G_LED_PWM_PORT GPIOA
#define G_LED_PWM_PIN GPIO_PIN_6

/*<! ovp921 blue light on signal !>*/
#define B_LED_PWM_PORT GPIOA
#define B_LED_PWM_PIN GPIO_PIN_7

/*<!  !>*/
#define TV_UART_TX_PORT GPIOA
#define TV_UART_TX_PIN GPIO_PIN_9

/*<!  !>*/
#define TV_UART_RX_PORT GPIOA
#define TV_UART_RX_PIN GPIO_PIN_10

/*<! PWM detect !>*/
#define FG_PORT GPIOA
#define FG_PIN GPIO_PIN_11

/*<! PWM driving !>*/
#define FAN_PWM_PORT GPIOB
#define FAN_PWM_PIN GPIO_PIN_11

/*<! None using. !>*/
#define CC_EN_PORT GPIOA
#define CC_EN_PIN GPIO_PIN_15

/*<!  !>*/
#define I_SPOKER_PORT GPIOC
#define I_SPOKER_PIN GPIO_PIN_13

    //whole chip init
    void Whole_MCU_init(void);

    uint16_t Get_NTC_Voltage(int ntc_idx);

    void Set_DAC_Value(uint16_t value);

    void Set_DAC_PWM_percent(uint16_t percent);

    uint32_t Get_FAN_FG(void);

#endif
