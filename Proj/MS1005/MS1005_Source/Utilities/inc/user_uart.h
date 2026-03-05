#ifndef __USER_UART_H
#define __USER_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"

#define COMn					1U

#define USER_COM1				USART1
#define COM1_CLK				RCU_USART1
#define COM1_TX_PIN				GPIO_PIN_2
#define COM1_TX_PORT			GPIOA
#define COM1_TX_CLK				RCU_GPIOA
#define COM1_RX_PIN				GPIO_PIN_3
#define COM1_RX_PORT			GPIOA
#define COM1_RX_CLK				RCU_GPIOA
#define COM1_IRQn				USART1_IRQn

#define USART1_RX_BUF_SIZE		1029
	
	typedef struct {
		uint8_t rxBuf[8][USART1_RX_BUF_SIZE];
		uint32_t rxLen[8];
		__IO uint32_t actIdx;
		__IO uint32_t currIdx;
	} uart_buf_t;
	
	extern uart_buf_t g_uartBuf;

#if APP || TEST
//	extern __IO uint8_t* g_inaUartBuf;
	
	uint16_t UART_ToggleDmaBuf (void);
	void UART_SendData(uint32_t usartPeriph, const uint8_t* buf, uint32_t size);
	void UART_DMA_SendData(uint32_t usartPeriph, const uint8_t* buf, uint32_t size);
#endif
#if BOOT
	int32_t UART_RecvData(uint32_t usartPeriph, uint8_t *buf, int32_t len, int32_t timeout);
	int32_t UART_RecvByte(uint32_t usartPeriph, uint8_t *buf, int32_t timeout);
	int32_t UART_SendChar(uint32_t usartPeriph, uint8_t val, int32_t timeout);
#endif

	void UART_UserInit(uint32_t usartPeriph, uint32_t baud);
	void UART_UserDMAInit(void);
	void UART_SendString(uint32_t usartPeriph, const char* str);

#ifdef __cplusplus
}
#endif

#endif
