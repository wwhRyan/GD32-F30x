/**
 * @file uart.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-02-17
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */

#include "uart.h"
#include "gd32f307c_eval.h"

/**
 * @brief init uart, using DMA send data & receive data, and using interrupt
 * 
 * @param uarter 
 */
void uarter_init(const Uarter *pUarter)
{
    /* enable USART clock */
    rcu_periph_clock_enable(pUarter->uart_periph);

    /* enable DMA clock */
    rcu_periph_clock_enable(pUarter->dma_periph);

    /* enable GPIO clock */
    rcu_periph_clock_enable(pUarter->gpio_periph);

    /* connect port to USARTx_Tx */
    gpio_init(pUarter->tx_port, pUarter->tx_mode, pUarter->tx_speed, pUarter->tx_pin);

    /* connect port to USARTx_Rx */
    gpio_init(pUarter->rx_port, pUarter->rx_mode, pUarter->rx_speed, pUarter->rx_pin);

    /* USART configure */
    usart_deinit(pUarter->uart_base);
    usart_baudrate_set(pUarter->uart_base, pUarter->baudrate);
    usart_word_length_set(pUarter->uart_base, USART_WL_9BIT);              // USART word length 8 bit
    usart_stop_bit_set(pUarter->uart_base, USART_STB_1BIT);                // USART stop bit 1 bit
    usart_parity_config(pUarter->uart_base, USART_PM_EVEN);                // USART parity mode none
    usart_hardware_flow_rts_config(pUarter->uart_base, USART_RTS_DISABLE); // RTS disable, none used
    usart_hardware_flow_cts_config(pUarter->uart_base, USART_CTS_DISABLE); // CTS disable, none used
    usart_receive_config(pUarter->uart_base, USART_RECEIVE_ENABLE);
    usart_transmit_config(pUarter->uart_base, USART_TRANSMIT_ENABLE);
    usart_enable(pUarter->uart_base);

    /* DMA configure */
    dma_parameter_struct dma_init_struct;
    if (pUarter->p_tx_buffer->buffer != NULL)
    {
        dma_deinit(pUarter->dma_periph, pUarter->dma_tx_channel);
        dma_struct_para_init(&dma_init_struct);
        dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.memory_addr = (uint32_t)pUarter->p_tx_buffer->buffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = DMA_BUFFER_SIZE;
        dma_init_struct.periph_addr = pUarter->uart_periph_address;
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(pUarter->dma_base, pUarter->dma_tx_channel, &dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(pUarter->dma_base, pUarter->dma_tx_channel);
        /* USART DMA enable for transmission and reception */
        usart_dma_transmit_config(pUarter->uart_base, USART_DENT_ENABLE);
        /* enable DMA channel */
        dma_channel_enable(pUarter->dma_base, pUarter->dma_tx_channel);
    }

    if (pUarter->p_rx_buffer->buffer != NULL)
    {
        dma_deinit(pUarter->dma_base, pUarter->dma_rx_channel);
        dma_struct_para_init(&dma_init_struct);
        dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_addr = (uint32_t)pUarter->p_rx_buffer->buffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = DMA_BUFFER_SIZE;
        dma_init_struct.periph_addr = pUarter->uart_periph_address;
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(pUarter->dma_base, pUarter->dma_rx_channel, &dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(pUarter->dma_base, pUarter->dma_rx_channel);
        /* USART DMA enable for transmission and reception */
        usart_dma_receive_config(pUarter->uart_base, USART_DENR_ENABLE);
        /* enable DMA channel */
        dma_channel_enable(pUarter->dma_base, pUarter->dma_rx_channel);
    }

    /* enable USART interrupt */
    nvic_irq_enable(pUarter->Irqn, 2, 0);
    usart_interrupt_enable(pUarter->uart_base, pUarter->uart_interrupt_type);

    /* initial buffer */
    pUarter->p_tx_buffer->head = 0;
    pUarter->p_tx_buffer->tail = 0;
    pUarter->p_rx_buffer->head = 0;
    pUarter->p_rx_buffer->tail = 0;
    memset(pUarter->p_tx_buffer->count, 0, BUFF_CACHE_SIZE * sizeof(uint16_t));
    memset(pUarter->p_rx_buffer->count, 0, BUFF_CACHE_SIZE * sizeof(uint16_t));
    memset(pUarter->p_tx_buffer->buffer, 0, DMA_BUFFER_SIZE * BUFF_CACHE_SIZE * sizeof(uint8_t));
    memset(pUarter->p_rx_buffer->buffer, 0, DMA_BUFFER_SIZE * BUFF_CACHE_SIZE * sizeof(uint8_t));
}

static inline bool is_circular_array_full(int head, int tail, int size)
{
    return (((head - tail + size) % size) == (size - 1));
}

// NOTE: data must be lesser than DMA_BUFFER_SIZE
void uarter_IRQ(const Uarter* pUarter)
{
    E_assert(pUarter != NULL);
    if (RESET != usart_interrupt_flag_get(pUarter->uart_base, pUarter->uart_interrupt_flag)) {
        dma_channel_disable(pUarter->dma_base, pUarter->dma_rx_channel);
        usart_interrupt_flag_clear(pUarter->uart_base, pUarter->uart_interrupt_flag);
        usart_data_receive(pUarter->uart_base);

        pUarter->p_rx_buffer->count[pUarter->p_rx_buffer->head] = abs(DMA_BUFFER_SIZE - dma_transfer_number_get(pUarter->dma_base, pUarter->dma_rx_channel));

        /* When the old buffer is not processed completely, the received data overwrites the latest head */
        if (!is_circular_array_full(pUarter->p_rx_buffer->head, pUarter->p_rx_buffer->tail, BUFF_CACHE_SIZE)) {
            pUarter->p_rx_buffer->head++;
            if (pUarter->p_rx_buffer->head >= BUFF_CACHE_SIZE)
                pUarter->p_rx_buffer->head = 0;
        }

        dma_memory_address_config(pUarter->dma_base, pUarter->dma_rx_channel, (uint32_t)&pUarter->p_rx_buffer->buffer[pUarter->p_rx_buffer->head]);
        // dma_transfer_number_config(pUarter->dma_base, pUarter->dma_rx_channel, DMA_BUFFER_SIZE);
        dma_channel_enable(pUarter->dma_base, pUarter->dma_rx_channel);
    }
}

char *GetRxData(const Uarter *pUarter)
{
    E_assert(pUarter != NULL);
    return (char *)pUarter->p_rx_buffer->buffer[pUarter->p_rx_buffer->tail];
}

int GetRxlen(const Uarter *pUarter)
{
    E_assert(pUarter != NULL);
    return (int)pUarter->p_rx_buffer->count[pUarter->p_rx_buffer->tail];
}

void ClearRxData(const Uarter* pUarter)
{
    E_assert(pUarter != NULL);
    pUarter->p_rx_buffer->count[pUarter->p_rx_buffer->tail] = 0;
    memset(pUarter->p_rx_buffer->buffer[pUarter->p_rx_buffer->tail], 0, DMA_BUFFER_SIZE);

    //A string sent by the host may be received twice(for unknow reason?), 
    //which will cause the serial port not to receive(tail larger than head, mcu misunderstanding buffer is full), 
    //fixing:add the tail cannot be larger than the head.
    if (pUarter->p_rx_buffer->tail == pUarter->p_rx_buffer->head)
        return;

    pUarter->p_rx_buffer->tail++;
    if (pUarter->p_rx_buffer->tail >= BUFF_CACHE_SIZE)
        pUarter->p_rx_buffer->tail = 0;
}

void uarter_send(const Uarter *pUarter, const char *pData, uint32_t len)
{
    E_assert(pUarter != NULL);
    char *p = (char *)pData;

    while (len--)
    {
        while (RESET == usart_flag_get(pUarter->uart_base, USART_FLAG_TBE))
            ;
        usart_data_transmit(pUarter->uart_base, *p++);
    }
}
