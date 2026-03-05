/*!
    \file    main.c
    \brief   ...

    \version 2024-11-29, V1.0.0, firmware for SD-02 Project

    Copyright (c) 2024, HM sensor.
*/


#include "main.h"

static void BSP_Init (void);

/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
	uint32_t counter = 0;
	
	BSP_Init();
	
    while(1) {
		printf("Hello World\r\n");
		rtt_log("Hello World %d\r\n", counter++);
		delay_1ms(1000);
    }
}

static void BSP_Init(void)
{
	systick_config();
	
	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);	// rtt init
	rtt_log("Run RGB App Version %s\r\n", "1.0.0");

	UART_UserInit (USER_COM1, 115200U);
}

/* retarget printf function to the USART ------------------- */
int fputc (int ch, FILE *f) 
{
	usart_data_transmit(USER_COM1, (uint8_t)ch);
	while(RESET == usart_flag_get(USER_COM1, USART_FLAG_TBE));

	return ch;
}
/* --------------------------------------------------------- */
