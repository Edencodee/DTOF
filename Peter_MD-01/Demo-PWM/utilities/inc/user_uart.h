#ifndef __UART_H
#define __UART_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"

#define COMn					1U

#define USER_COM1				USART2
#define COM1_CLK				RCU_USART2
#define COM1_TX_PIN				GPIO_PIN_10
#define COM1_TX_PORT			GPIOB
#define COM1_TX_CLK				RCU_GPIOB
#define COM1_RX_PIN				GPIO_PIN_11
#define COM1_RX_PORT			GPIOB
#define COM1_RX_CLK				RCU_GPIOB

	void UART_UserInit(uint32_t usartPeriph, uint32_t baud);
	void UART_UserDMAInit(void);
	int32_t UART_RecvData(uint32_t usartPeriph, uint8_t *buf, int32_t len, int32_t timeout);
	int32_t UART_RecvByte(uint32_t usartPeriph, uint8_t *buf, int32_t timeout);
	int32_t UART_SendChar(uint32_t usartPeriph, uint8_t val, int32_t timeout);
	void UART_SendData(uint32_t usartPeriph, uint8_t* buf, uint32_t size);
	void UART_DMA_SendData(uint32_t usartPeriph, uint8_t* buf, uint32_t size);

#ifdef cplusplus
}
#endif

#endif
