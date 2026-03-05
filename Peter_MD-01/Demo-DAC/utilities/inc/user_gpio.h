 #ifndef __GPIO_H
#define __GPIO_H

#ifdef cplusplus
extern "C"
{
#endif

	#include "gd32f4xx.h"
	
/* ANALOG IO ------------------------------------------ */
#define ANALOG_PIN				GPIO_PIN_15
#define ANALOG_PORT				GPIOD
#define ANALOG_CLK				RCU_GPIOD
/* ---------------------------------------------------- */
	
	void GPIO_AnalogSel(uint16_t analog);
	void GPIO_UserInit (void);
	
#ifdef cplusplus
}
#endif

#endif
