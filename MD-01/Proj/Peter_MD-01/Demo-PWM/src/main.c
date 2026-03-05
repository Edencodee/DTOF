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

static void BSP_Init(void);
/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	BSP_Init();

	rtt_log("RUN APP");
    while(1) {
    }
}

static void BSP_Init(void)
{
	systick_config();
	
	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);	// rtt init
	rtt_log("Run RGB App Version %s\r\n", "1.0.0");
	
	TIMER_UserInit();
}
