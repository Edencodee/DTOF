#include <string.h>
#include <stdio.h>
#include "user_uart.h"
#include "systick.h"

#define USART2_DATA_ADDRESS		((uint32_t)&USART_DATA(USART2))
#define USART2_RX_BUF_SIZE		1029

/* IO array ------------------------------------------------------- */
static rcu_periph_enum COM_CLK[COMn] = {COM1_CLK};
static uint32_t COM_TX_PIN[COMn] = {COM1_TX_PIN};
static uint32_t COM_RX_PIN[COMn] = {COM1_RX_PIN};
static uint32_t COM_TX_PORT[COMn] = {COM1_TX_PORT};
static uint32_t COM_RX_PORT[COMn] = {COM1_RX_PORT};
static rcu_periph_enum COM_TX_CLK[COMn] = {COM1_TX_CLK};
static rcu_periph_enum COM_RX_CLK[COMn] = {COM1_RX_CLK};
#if APP
static uint32_t COM_IRQn[COMn] = {COM1_IRQn};
#endif
/* ---------------------------------------------------------------- */
#pragma arm section code = "RAMCODE"
uint8_t g_uart2Rxbuffer0[USART2_RX_BUF_SIZE];
uint8_t g_uart2Rxbuffer1[USART2_RX_BUF_SIZE];
uint8_t *g_uart2Rxbuffer[2] = {g_uart2Rxbuffer0, g_uart2Rxbuffer1};
uint8_t g_uart2Txbuf[] = "Run APP\r\n";
uint8_t* g_inaUartBuf;
#pragma arm section
void UART_UserDMAInit (void)
{
	dma_single_data_parameter_struct dma_init_struct;
    
    rcu_periph_clock_enable(RCU_DMA0);
    
	/* deinitialize DMA0 channel1 (USART2 rx) --------------------- */
    dma_deinit(DMA0, DMA_CH1);
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
	dma_init_struct.memory0_addr = (uint32_t)g_uart2Rxbuffer0;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.number = 1024; // USART2_RX_BUF_SIZE;
    dma_init_struct.periph_addr = USART2_DATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH1, &dma_init_struct);
    
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_channel_subperipheral_select(DMA0, DMA_CH1, DMA_SUBPERI4);
    /* enable DMA0 channel1 */
    dma_channel_enable(DMA0, DMA_CH1);
	/* ------------------------------------------------------------ */
	
	/* deinitialize DMA0 channel3(USART2 TX) ---------------------- */
	dma_deinit(DMA0, DMA_CH3);
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
	dma_init_struct.memory0_addr = (uint32_t)g_uart2Txbuf;
	dma_init_struct.number = 9;
	dma_single_data_mode_init(DMA0, DMA_CH3, &dma_init_struct);
	
	/* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_channel_subperipheral_select(DMA0, DMA_CH3, DMA_SUBPERI4);
    /* enable DMA0 channel3 */
    dma_channel_enable(DMA0, DMA_CH3);
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
#if APP
	nvic_irq_enable(COM_IRQn[comId], 0, 0);
	/* enable the USART receive timeout and configure the time of timeout */
	usart_receiver_timeout_enable(usartPeriph);
	usart_receiver_timeout_threshold_config(usartPeriph, 40); // 3.5 character, 35 @ no parity, 38 @ parity
	usart_interrupt_enable(usartPeriph, USART_INT_RT);
#endif
}
#pragma arm section code = "RAMCODE"
#if BOOT
int32_t UART_RecvByte(uint32_t usartPeriph, uint8_t *buf, int32_t timeout)
{
	return UART_RecvData(usartPeriph, buf, 1, timeout);
}

int32_t UART_RecvData (uint32_t usartPeriph, uint8_t *buf, int32_t size, int32_t timeout)
{
	switch (usartPeriph) {
	case USER_COM1: {
		uint32_t num = (size == -1) ? USART2_RX_BUF_SIZE : size;

		dma_channel_disable(DMA0, DMA_CH1);
		dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
		dma_transfer_number_config(DMA0, DMA_CH1, num);
		dma_channel_enable(DMA0, DMA_CH1);
		
		while (RESET == usart_flag_get(USART2, USART_FLAG_IDLE)) {
			if (timeout != -1) {
				if (timeout-- == 0) {
					return -1;
				}
				delay_1ms(1);
			}
		}
		
		/* clear IDLE flag */
        usart_data_receive(USART2);
		uint16_t rx_count = num - (dma_transfer_number_get(DMA0, DMA_CH1));
		if ((size != -1) && dma_transfer_number_get(DMA0, DMA_CH1) != 0) {
			return -1;
		}
		memcpy(buf, (void*)g_uart2Rxbuffer0, rx_count);

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
#else
uint16_t UART_ToggleDmaBuf (void)
{
	static int32_t s_bufToggle = 0;
	
	s_bufToggle = 1 - s_bufToggle;
	uint32_t actUartBuf = (uint32_t)(g_uart2Rxbuffer[s_bufToggle]);
	g_inaUartBuf = g_uart2Rxbuffer[1 - s_bufToggle];
	
	uint16_t len = 1024 - dma_transfer_number_get(DMA0, DMA_CH1);
	dma_channel_disable(DMA0, DMA_CH1);
	dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
	dma_transfer_number_config(DMA0, DMA_CH1, 1024); // kFrameCnt
	dma_memory_address_config(DMA0, DMA_CH1, DMA_MEMORY_0, actUartBuf);
	dma_channel_enable(DMA0, DMA_CH1);
	
	return len;
}

void UART_SendData (uint32_t usartPeriph, uint8_t* buf, uint32_t size)
{
	for (int i = 0; i < size; i++) {
		usart_data_transmit(usartPeriph, buf[i]);
		while (RESET == usart_flag_get(usartPeriph, USART_FLAG_TBE)) {
		}
	}
}

void UART_DMA_SendData (uint32_t usartPeriph, uint8_t* buf, uint32_t size) // TBD
{
	switch (usartPeriph) {
	case USART2: {
//		while(RESET == dma_flag_get(DMA0, DMA_CH3, DMA_FLAG_FTF));
		dma_channel_disable(DMA0, DMA_CH3);
		dma_flag_clear(DMA0, DMA_CH3, DMA_FLAG_FTF);	
		dma_memory_address_config(DMA0, DMA_CH3, DMA_MEMORY_0, (uint32_t)buf);
		dma_transfer_number_config(DMA0, DMA_CH3, size);	
		dma_channel_enable(DMA0, DMA_CH3);
		while(RESET == dma_flag_get(DMA0, DMA_CH3, DMA_FLAG_FTF));
	} break;
	}
}
#endif
#pragma arm section
