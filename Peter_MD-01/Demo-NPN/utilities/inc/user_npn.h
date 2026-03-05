#ifndef __USER_SPI_H
#define __USER_SPI_H

#ifdef cplusplus
extern "C"
{
#endif

#include "gd32f4xx.h"

/* output IO ------------------------------------------ */
#define OUT_N					1U
#define NPN1_PIN           		GPIO_PIN_1	/* NPN		*/
#define NPN1_PORT     			GPIOE
#define NPN1_CLK      			RCU_GPIOE
#define PNP1_PIN            	GPIO_PIN_13	/* PNP		*/
#define PNP1_PORT      			GPIOC
#define PNP1_CLK       			RCU_GPIOC

	/* channel typedef */
	typedef enum {
		kCh1 = 0,
	} ch_t;

	void GPIO_UserInit(void);
	void NpnClr(ch_t ch);
	void NpnSet(ch_t ch);
	void PnpClr(ch_t ch);
	void PnpSet(ch_t ch);
	
#ifdef cplusplus
}
#endif

#endif
