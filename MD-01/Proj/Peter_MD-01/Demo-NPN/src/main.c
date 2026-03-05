/*!
    \file    main.c
    \brief   ...

    \version 2025-01-03, V1.0.0, firmware for MD-01 Project

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
	BSP_Init();
	
    while(1) {
#if test
		delay_1ms(1000);
		NpnClr(kCh1);
		PnpSet(kCh1);
		delay_1ms(1000);
		NpnSet(kCh1);
		PnpClr(kCh1);
#endif
    }
}

static void BSP_Init(void)
{
	systick_config();
	
	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);	// rtt init
	rtt_log("Run RGB App Version %s\r\n", "1.0.0");
	
	GPIO_UserInit();
}
