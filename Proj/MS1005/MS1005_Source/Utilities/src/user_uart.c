#include <string.h>
#include <stdio.h>
#include "user_uart.h"
#include "systick.h"

#define USART1_DATA_ADDRESS		((uint32_t)&USART_DATA(USART1))

/* IO array ------------------------------------------------------- */
static rcu_periph_enum COM_CLK[COMn] = {COM1_CLK};
static uint32_t COM_TX_PIN[COMn] = {COM1_TX_PIN};
static uint32_t COM_RX_PIN[COMn] = {COM1_RX_PIN};
static uint32_t COM_TX_PORT[COMn] = {COM1_TX_PORT};
static uint32_t COM_RX_PORT[COMn] = {COM1_RX_PORT};
static rcu_periph_enum COM_TX_CLK[COMn] = {COM1_TX_CLK};
static rcu_periph_enum COM_RX_CLK[COMn] = {COM1_RX_CLK};
#if APP || TEST
static uint32_t COM_IRQn[COMn] = {COM1_IRQn};
#endif
/* ---------------------------------------------------------------- */
__attribute__((aligned(4))) uart_buf_t g_uartBuf = {{{0}}, {0}, 0, 0};

void UART_UserDMAInit (void)
{
	dma_single_data_parameter_struct dma_init_struct;

	rcu_periph_clock_enable(RCU_DMA0);

	/* deinitialize DMA0 channel5 (USART1 RX) --------------------- */
	dma_deinit(DMA0, DMA_CH5);
	dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
	dma_init_struct.memory0_addr = (uint32_t)g_uartBuf.rxBuf[0];
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.number = USART1_RX_BUF_SIZE;
	dma_init_struct.periph_addr = USART1_DATA_ADDRESS;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	dma_single_data_mode_init(DMA0, DMA_CH5, &dma_init_struct);

	/* configure DMA mode */
	dma_circulation_disable(DMA0, DMA_CH5);
	dma_channel_subperipheral_select(DMA0, DMA_CH5, DMA_SUBPERI4);
	/* enable DMA0 channel5 */
	dma_channel_enable(DMA0, DMA_CH5);
	/* ------------------------------------------------------------ */

	/* deinitialize DMA0 channel6(USART1 TX) ---------------------- */
	dma_deinit(DMA0, DMA_CH6);
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
	//Send Data
	dma_init_struct.memory0_addr = NULL;
	dma_init_struct.number = 0;
	dma_single_data_mode_init(DMA0, DMA_CH6, &dma_init_struct);

	/* configure DMA mode */
	dma_circulation_disable(DMA0, DMA_CH6);
	dma_channel_subperipheral_select(DMA0, DMA_CH6, DMA_SUBPERI4);
	/* enable DMA0 channel6 */
	dma_channel_enable(DMA0, DMA_CH6);
	/* ------------------------------------------------------------ */
}

void UART_UserInit (uint32_t usartPeriph, uint32_t baud)
{
	uint32_t comId = 0U;

	switch (usartPeriph) {
	case USER_COM1:
		comId = 0U;
		break;
	}

	/* enable GPIO clock */
	rcu_periph_clock_enable(COM_TX_CLK[comId]);
	rcu_periph_clock_enable(COM_RX_CLK[comId]);
	/* enable USART clock */
	rcu_periph_clock_enable(COM_CLK[comId]);

	/* connect port to USARTx_Tx */
	gpio_af_set(COM_TX_PORT[comId], GPIO_AF_7, COM_TX_PIN[comId]);
	/* connect port to USARTx_Rx */
	gpio_af_set(COM_RX_PORT[comId], GPIO_AF_7, COM_RX_PIN[comId]);

	/* configure USART Tx as alternate function push-pull */
	gpio_mode_set(COM_TX_PORT[comId], GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_TX_PIN[comId]);
	gpio_output_options_set(COM_TX_PORT[comId], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, COM_TX_PIN[comId]);
	/* configure USART Rx as alternate function push-pull */
	gpio_mode_set(COM_RX_PORT[comId], GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN[comId]);
	gpio_output_options_set(COM_RX_PORT[comId], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, COM_RX_PIN[comId]);

	/* configure USART */
	usart_deinit(usartPeriph);
	usart_baudrate_set(usartPeriph, baud);
	usart_receive_config(usartPeriph, USART_RECEIVE_ENABLE);
	usart_transmit_config(usartPeriph, USART_TRANSMIT_ENABLE);
	usart_dma_transmit_config(usartPeriph, USART_TRANSMIT_DMA_ENABLE);
	usart_dma_receive_config(usartPeriph, USART_RECEIVE_DMA_ENABLE);
	usart_enable(usartPeriph);

#if APP || TEST
	nvic_irq_enable(COM_IRQn[comId], 0, 0);
	/* enable the USART receive timeout and configure the time of timeout */
	usart_receiver_timeout_enable(usartPeriph);
	usart_receiver_timeout_threshold_config(usartPeriph, 40); // 3.5 character, 35 @ no parity, 38 @ parity
	usart_interrupt_enable(usartPeriph, USART_INT_RT);
#endif
}

#if BOOT
int32_t UART_RecvByte(uint32_t usartPeriph, uint8_t *buf, int32_t timeout)
{
	return UART_RecvData(usartPeriph, buf, 1, timeout);
}

int32_t UART_RecvData (uint32_t usartPeriph, uint8_t *buf, int32_t size, int32_t timeout)
{
	switch (usartPeriph) {
	case USER_COM1: {
		uint32_t num = (size == -1) ? USART1_RX_BUF_SIZE : size;

		dma_channel_disable(DMA0, DMA_CH5);
		dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_FTF);
		dma_transfer_number_config(DMA0, DMA_CH5, num);
		dma_channel_enable(DMA0, DMA_CH5);

		while (RESET == usart_flag_get(USART1, USART_FLAG_IDLE)) {
			if (timeout != -1) {
				if (timeout-- == 0) {
					return -1;
				}
				delay_1ms(1);
			}
		}

		/* clear IDLE flag */
		usart_data_receive(USART1);
		uint16_t rx_count = num - (dma_transfer_number_get(DMA0, DMA_CH5));
		if ((size != -1) && dma_transfer_number_get(DMA0, DMA_CH5) != 0) {
			return -1;
		}
//		memcpy(buf, (void*)g_uart1Rxbuffer0, rx_count);
		memcpy(buf, g_uartBuf.rxBuf[0], rx_count);

		return 0;
	}

		/*
		case COMx: {
		} break;
		*/

	}

	return -1;
}

int32_t UART_SendChar (uint32_t usartPeriph, uint8_t val, int32_t timeout)
{
	usart_data_transmit(usartPeriph, val);
	while (RESET == usart_flag_get(usartPeriph, USART_FLAG_TBE)) {
		if (timeout != -1) {
			if (timeout-- == 0) {
				return -1;
			}
			delay_1ms(1);
		}
	}
	return 0;
}
#endif

#if APP || TEST
uint16_t UART_ToggleDmaBuf (void)
{
//	g_uartBuf.actIdx = 1 - g_uartBuf.actIdx;
//	uint32_t actUartBuf = (uint32_t)g_uartBuf.rxBuf[g_uartBuf.actIdx];
	
	uint16_t len = USART1_RX_BUF_SIZE - dma_transfer_number_get(DMA0, DMA_CH5);
	g_uartBuf.rxLen[g_uartBuf.actIdx] = len;
	g_uartBuf.actIdx = (g_uartBuf.actIdx + 1) % 8;
	dma_channel_disable(DMA0, DMA_CH5);
	dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_FTF);
	dma_transfer_number_config(DMA0, DMA_CH5, USART1_RX_BUF_SIZE); // kFrameCnt
//	dma_memory_address_config(DMA0, DMA_CH5, DMA_MEMORY_0, actUartBuf);
	dma_memory_address_config(DMA0, DMA_CH5, DMA_MEMORY_0, (uint32_t)(g_uartBuf.rxBuf[g_uartBuf.actIdx]));
	dma_channel_enable(DMA0, DMA_CH5);

	return len;
}

void UART_SendData (uint32_t usartPeriph, const uint8_t* buf, uint32_t size)
{
	for (int i = 0; i < size; i++) {
		usart_data_transmit(usartPeriph, buf[i]);
		while (RESET == usart_flag_get(usartPeriph, USART_FLAG_TBE)) {
		}
	}
}

void UART_DMA_SendData (uint32_t usartPeriph, const uint8_t* buf, uint32_t size) // TBD
{
	switch (usartPeriph) {
	case USART1: {
//		while(RESET == dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF));
		dma_channel_disable(DMA0, DMA_CH6);
		dma_flag_clear(DMA0, DMA_CH6, DMA_FLAG_FTF);
		dma_memory_address_config(DMA0, DMA_CH6, DMA_MEMORY_0, (uint32_t)buf);
		dma_transfer_number_config(DMA0, DMA_CH6, size);
		dma_channel_enable(DMA0, DMA_CH6);
		while(RESET == dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF));
	}
	break;
	}
}

void UART_SendString(uint32_t usartPeriph, const char* str)
{
    uint32_t len = strlen(str);
    
    if (len > 16) {
        UART_DMA_SendData(usartPeriph, (const uint8_t*)str, len);
    } else {
        UART_SendData(usartPeriph, (const uint8_t*)str, len);
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f) {
	usart_data_transmit(USER_COM1, (uint8_t)ch);
	while(RESET == usart_flag_get(USER_COM1, USART_FLAG_TBE));
	return ch;
}


#endif
