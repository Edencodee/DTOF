/*!
    \file    main.c
    \brief   ...

    \version 2024-11-19, V1.0.0, firmware for SD-02 Project

    Copyright (c) 2024, HM sensor.
*/

#include "main.h"

/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void BSP_Init(void);

int main(void)
{
	BSP_Init();
	
	while(1) {
	}
}

static void BSP_Init(void)
{
	systick_config();
	
	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);	// rtt init
	rtt_log("Run App Version %s\r\n", "1.0.0");

	GPIO_UserInit();
	SPI_UserInit();
	OLED_Init();
	delay_1ms(200);
	OLED_FullBmp(hm);
}
