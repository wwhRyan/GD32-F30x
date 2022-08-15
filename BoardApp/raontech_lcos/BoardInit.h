/**
 * @file ExecThread.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-02-14
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */

#ifndef EXECTHREAD_H
#define EXECTHREAD_H

#include "file.h"
#define MCU_VERSION "V1.0"

#include "adc_mcu.h"
#include "basicApp.h"
#include "dac.h"
#include "eeprom.h"
#include "event_groups.h"
#include "i2c.h"
#include "port.h"
#include "semphr.h"
#include "timer.h"
#include "uart.h"
#include "ulog.h"

#ifndef ATHENA_OV_LCOS
#error "Please define ATHENA_OV_LCOS"
#endif

// configMINIMAL_STACK_SIZE
#define THREAD_UART_EVENT_STACK_SIZE (1024 * 2)
#define THREAD_FIRST_CONSUMER_STACK_SIZE (1024 * 2)
#define THREAD_SECOND_CONSUMER_STACK_SIZE 512
#define CONFIG_START_ADDR 0x00
#define LOG_START_ADDR 0x1024

void ThreadFirstConsumer(void* pvParameters);
void ThreadUartEvent(void* pvParameters);
void ThreadSecondConsumer(void* pvParameters);

void output_printf(const char* fmt, ...);
void system_ipc_init(void);

extern EventGroupHandle_t sys_sig;
extern SemaphoreHandle_t uart_Semaphore;
extern SemaphoreHandle_t i2c_Semaphore;
extern QueueHandle_t xQueue_eeprom;
// extern SoftwareI2C raontech_i2c;

extern const eeprom_model_t BL24C64A; /* 8K bytes page=32 */
extern const eeprom_model_t AT24C02D; /* 2K bytes page=8*/
extern const fan_timer_config_t cw_wheel_fg;
extern file_t eeprom_log;

typedef enum sys_sig_t {
    sig_lightsource,
    sig_system,
    sig_ovp921_status,
    sig_light_status,
    sig_slient_async_msg,
    sig_update_anf,
    sig_update_firmware,
    sig_eeprom_write,
    sig_mcu_init_ok,
} sys_sig_t;

typedef struct msg_t {
    uint8_t idx;
    void* pData;
    uint16_t addr;
    uint8_t size;
} msg_t;

/*<! Red laser NTC !>*/
#define R_LD_NTC_PORT GPIOB
#define R_LD_NTC_PIN GPIO_PIN_0

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

/*<! PWM driving !>*/
#define FAN_PWM_PORT GPIOB
#define FAN_PWM_PIN GPIO_PIN_11

#define OVP921_RESET_PORT GPIOB
#define OVP921_RESET_PIN GPIO_PIN_12

#define MCU_GPIO_INT_PORT GPIOB
#define MCU_GPIO_INT_PIN GPIO_PIN_13

#define SENSOR_SCL_PORT GPIOB
#define SENSOR_SCL_PIN GPIO_PIN_14

#define SENSOR_SDA_PORT GPIOB
#define SENSOR_SDA_PIN GPIO_PIN_15

/*<! hardware version detect !>*/
#define HW_VER0_PORT GPIOC
#define HW_VER0_PIN GPIO_PIN_14

#define HW_VER1_PORT GPIOC
#define HW_VER1_PIN GPIO_PIN_15

#define B_LED_NTC_PORT GPIOA
#define B_LED_NTC_PIN GPIO_PIN_0

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

#define LCOS_PANEL_NTC_PORT GPIOA
#define LCOS_PANEL_NTC_PIN GPIO_PIN_5

/*<! ovp921 green light on signal !>*/
#define G_LED_PWM_PORT GPIOA
#define G_LED_PWM_PIN GPIO_PIN_6

/*<! ovp921 blue light on signal !>*/
#define B_LED_PWM_PORT GPIOA
#define B_LED_PWM_PIN GPIO_PIN_7

/*<! ovp921 red light on signal !>*/
#define R_LED_PWM_PORT GPIOA
#define R_LED_PWM_PIN GPIO_PIN_8

/*<!  !>*/
#define TV_UART_TX_PORT GPIOA
#define TV_UART_TX_PIN GPIO_PIN_9

/*<!  !>*/
#define TV_UART_RX_PORT GPIOA
#define TV_UART_RX_PIN GPIO_PIN_10

/*<! PWM detect !>*/
#define FG_PORT GPIOA
#define FG_PIN GPIO_PIN_11

/*<! None using. !>*/
#define RESERVERD_1_PORT GPIOA
#define RESERVERD_1_PIN GPIO_PIN_12

/*<! None using. !>*/
#define RESERVERD_2_PORT GPIOA
#define RESERVERD_2_PIN GPIO_PIN_15

/*<! EE_WP !>*/
#define EE_WP_PORT GPIOC
#define EE_WP_PIN GPIO_PIN_13

#endif
