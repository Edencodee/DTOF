#ifndef __GPIO_H
#define __GPIO_H

#ifdef cplusplus
extern "C"
{
#endif
#include "gd32f4xx.h"

/* oled IO -------------------------------------------- */
#define OLED_N					4U

#define OLED_RES_PIN           	GPIO_PIN_13	/* RES		*/
#define OLED_RES_PORT     		GPIOD
#define OLED_RES_CLK      		RCU_GPIOD
#define OLED_CS_PIN            	GPIO_PIN_10	/* CS 		*/
#define OLED_CS_PORT      		GPIOD
#define OLED_CS_CLK       		RCU_GPIOD
#define OLED_DC_PIN           	GPIO_PIN_12	/* D/C		*/
#define OLED_DC_PORT     		GPIOB
#define OLED_DC_CLK      		RCU_GPIOB
#define OLED_VEN_PIN            GPIO_PIN_9	/* VEN 		*/
#define OLED_VEN_PORT			GPIOE
#define OLED_VEN_CLK			RCU_GPIOE
/* ---------------------------------------------------- */
	/* oled ctrl typedef */
	typedef enum {
		kOledRes = 0,
		kOledCs,
		kOledDc,
		kOledVen,
	} oled_ctrl_t;

	/* function declaration --------------------------- */
	void OledClr(oled_ctrl_t ctrl);
	void OledSet(oled_ctrl_t ctrl);
	void GPIO_UserInit(void);
	/* ------------------------------------------------ */

#ifdef cplusplus
}
#endif

#endif
