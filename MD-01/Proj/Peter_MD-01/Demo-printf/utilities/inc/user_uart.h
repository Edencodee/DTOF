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

#ifdef cplusplus
}
#endif

#endif
