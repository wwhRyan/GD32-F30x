/**
 * @file uart.h
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-18
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#ifndef __UART_H__
#define __UART_H__

#include "main.h"
#include "gd32f30x.h"

#define BAUDRATE (115200)
#define DMA_BUFFER_SIZE (255)
#define BUFF_CACHE_SIZE (3) /* uart buffer number, more buffer, more memery need, more safety for uart receive. */

#define USART0_DATA_ADDRESS ((uint32_t)&USART_DATA(USART0))
#define USART1_DATA_ADDRESS ((uint32_t)&USART_DATA(USART1))

typedef struct __uart_buffer
{
	uint8_t buffer[BUFF_CACHE_SIZE][DMA_BUFFER_SIZE];
	uint16_t count[BUFF_CACHE_SIZE];
	uint8_t head; /* buffer cache head idx */
	uint8_t tail; /* buffer cache tail idx */
} UartBuffer;

typedef struct __Uarter Uarter;
typedef struct __Uarter
{
	rcu_periph_enum uart_periph; /* uart rcu_periph clock */
	rcu_periph_enum dma_periph;	 /* dma rcu_periph clock */
	rcu_periph_enum gpio_periph; /* gpio rcu_periph clock */

	uint32_t uart_base; /* uart base address */
	uint32_t dma_base;	/* dma base address */

	IRQn_Type Irqn;								   /* uart irqn for nvic setting*/
	usart_interrupt_enum uart_interrupt_type;	   /* uart interrupt type enable setting*/
	usart_interrupt_flag_enum uart_interrupt_flag; /* uart interrupt type enable setting*/

	uint32_t baudrate;
	uint32_t uart_periph_address; //USARTx_DATA_ADDRESS

	/* connect port to USARTx_Tx */
	uint32_t tx_port;
	uint32_t tx_pin;
	uint32_t tx_mode;
	uint32_t tx_speed;

	/* connect port to USARTx_Rx */
	uint32_t rx_port;
	uint32_t rx_pin;
	uint32_t rx_mode;
	uint32_t rx_speed;

	dma_channel_enum dma_tx_channel;
	dma_channel_enum dma_rx_channel;

	UartBuffer *p_rx_buffer;
	UartBuffer *p_tx_buffer;
} Uarter;

void uarter_init(const Uarter *uarter);
void uarter_IRQ(const Uarter *uarter);

char *GetRxData(const Uarter *pUarter);
int GetRxlen(const Uarter *pUarter);
void ClearRxData(const Uarter *pUarter);

void uarter_send(const Uarter *pUarter, const char *pData, uint32_t len);

#endif //__UART_H__
