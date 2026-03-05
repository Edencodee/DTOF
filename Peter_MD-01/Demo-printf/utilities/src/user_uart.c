#include "user_uart.h"

/* IO array ------------------------------------------------------- */
static rcu_periph_enum COM_CLK[COMn] = {COM1_CLK};
static uint32_t COM_TX_PIN[COMn] = {COM1_TX_PIN};
static uint32_t COM_RX_PIN[COMn] = {COM1_RX_PIN};
static uint32_t COM_TX_PORT[COMn] = {COM1_TX_PORT};
static uint32_t COM_RX_PORT[COMn] = {COM1_RX_PORT};
static rcu_periph_enum COM_TX_CLK[COMn] = {COM1_TX_CLK};
static rcu_periph_enum COM_RX_CLK[COMn] = {COM1_RX_CLK};

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
    usart_enable(usartPeriph);
}
