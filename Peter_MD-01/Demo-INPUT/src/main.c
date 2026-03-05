/*!
    \file    main.c
    \brief   ...

    \version 2024-11-19, V1.0.0, firmware for SD-02 Project

    Copyright (c) 2024, HM sensor.
*/

#include "main.h"

static void BSP_Init (void);

int main(void)
{
	uint32_t g_counter = 0;

	BSP_Init();
	rtt_log("INPUT TEST\r\n");
	
    while(1) {
		if (gpio_input_bit_get(INPUT_PORT, INPUT_PIN)) {
			delay_1ms(1);
			if (gpio_input_bit_get(INPUT_PORT, INPUT_PIN)) {
				rtt_log("INPUT 24V %d, waiting release...\r\n", g_counter++);
				while (gpio_input_bit_get(INPUT_PORT, INPUT_PIN));
				rtt_log("INPUT release...\r\n");
			}
		} 
		delay_1ms(1);
    }
}

static void BSP_Init (void)
{
	systick_config();
	
	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);	// rtt init
	rtt_log("Run RGB App Version %s\r\n", "1.0.0");
	
	GPIO_UserInit ();
}
