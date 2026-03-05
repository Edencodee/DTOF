#ifndef __INPUT_H
#define __INPUT_H

#ifdef cplusplus
extern "C"
{
#endif

/* INPUT IO ------------------------------------------- */
#define INPUT_PIN				GPIO_PIN_3
#define INPUT_PORT				GPIOE
#define INPUT_CLK				RCU_GPIOE

void GPIO_UserInit (void);
	
#ifdef cplusplus
}
#endif

#endif
