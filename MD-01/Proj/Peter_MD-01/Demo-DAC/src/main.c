/*!
    \file    main.c
    \brief   ...

    \version 2024-11-19, V1.0.0, firmware for SD-02 Project
*/

/*
    Copyright (c) 2024, HM sensor.
*/

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

static void BSP_Init (void);

/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
#define VOL_OUT			1

int main(void)
{
	BSP_Init();
	
	u16_t test = {.word = 0};
	
#if VOL_OUT
	GPIO_AnalogSel(1);
#else
	GPIO_AnalogSel(0);
#endif
	
    while(1) {
		delay_1ms(2000);
		
		SPI_WriteDAC(test);
		rtt_log("%05d -> %d mv\r\n", test.word, test.word * 10000 / 65535);
		test.word += 5000;

		if (test.word >= 65535) {
			test.word = 0;
		}
    }
}

static void BSP_Init(void)
{
	systick_config();
	
	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);	// rtt init
	rtt_log("Run RGB App Version %s\r\n", "1.0.0");
	
	SPI_UserInit ();
}
