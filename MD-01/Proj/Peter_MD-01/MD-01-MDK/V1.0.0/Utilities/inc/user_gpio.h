 #ifndef __GPIO_H
#define __GPIO_H

#ifdef cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include "gd32f4xx.h"

#define BS_LONG_TICKS      		1000    	/* 1s 		*/
#define BS_SHORT_TMO   			10     		/* 10ms not_usd double click	*/

#define INA						-1
#define ACT						0

/* key IO --------------------------------------------- */
#define PB_N                   	4U
#define PB0_PIN           		GPIO_PIN_1	/* ESC 		*/
#define PB0_PORT     			GPIOB
#define PB0_CLK      			RCU_GPIOB
#define PB1_PIN           		GPIO_PIN_7	/* M 		*/
#define PB1_PORT     			GPIOE
#define PB1_CLK      			RCU_GPIOE
#define PB2_PIN          		GPIO_PIN_11	/* UP 	    */
#define PB2_PORT     			GPIOE
#define PB2_CLK      			RCU_GPIOE
#define PB3_PIN           		GPIO_PIN_13	/* DN 		*/
#define PB3_PORT     			GPIOE
#define PB3_CLK      			RCU_GPIOE
/* ---------------------------------------------------- */

/* led IO --------------------------------------------- */
#define LED_N                   2U
#define BLED_PIN           		GPIO_PIN_15	/* BLUE		*/
#define BLED_PORT     			GPIOE
#define BLED_CLK      			RCU_GPIOE
#define RLED_PIN            	GPIO_PIN_14	/* RED 		*/
#define RLED_PORT      			GPIOE
#define RLED_CLK       			RCU_GPIOE
/* ---------------------------------------------------- */

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

/* output IO ------------------------------------------ */
#define OUT_N					1U
#define NPN1_PIN           		GPIO_PIN_1	/* NPN		*/
#define NPN1_PORT     			GPIOE
#define NPN1_CLK      			RCU_GPIOE
#define PNP1_PIN            	GPIO_PIN_13	/* PNP		*/
#define PNP1_PORT      			GPIOC
#define PNP1_CLK       			RCU_GPIOC
#define FUN1_PIN           		GPIO_PIN_2	/* FUN		*/
#define FUN1_PORT     			GPIOE
#define FUN1_CLK      			RCU_GPIOE
#define FUP1_PIN            	GPIO_PIN_4	/* FUP		*/
#define FUP1_PORT      			GPIOE
#define FUP1_CLK       			RCU_GPIOE
/* ---------------------------------------------------- */

/* ANALOG IO ------------------------------------------ */
#define ANALOG_PIN				GPIO_PIN_15
#define ANALOG_PORT				GPIOD
#define ANALOG_CLK				RCU_GPIOD
/* ---------------------------------------------------- */

/* INPUT IO ------------------------------------------- */
#define INPUT_PIN				GPIO_PIN_3
#define INPUT_PORT				GPIOE
#define INPUT_CLK				RCU_GPIOE
/* ---------------------------------------------------- */
	/* key press state typedef */
	typedef enum {
		kNoneClick = 0,
		kSingleClick,
		kDoubleClick,
		kLongClick,
		kLongClickRelease
	} key_press_state_t;

	/* led typedef ------------------------------------ */
	typedef enum {
		kBLed = 0,
		kRLed
	} led_t;

	typedef enum {
		kLedOff = 0,
		kLedOn,
		kLedToggle,
		kLedNone
	} led_state_t;
	/* ------------------------------------------------ */

	/* channel typedef */
	typedef enum {
		kCh1 = 0,
	} ch_t;

	/* switch typedef */
	typedef enum {
		kPbEsc = 0,
		kPbM,
		kPbUp,
		kPbDn,
		kInput,
	} pb_t;

	/* oled ctrl typedef */
	typedef enum {
		kOledRes = 0,
		kOledCs,
		kOledDc,
		kOledVen,
	} oled_ctrl_t;

	/* function declaration --------------------------- */
	void NpnClr(ch_t ch);
	void NpnSet(ch_t ch);
	void PnpClr(ch_t ch);
	void PnpSet(ch_t ch);
	void LedClr(led_t led);
	void LedSet(led_t led);
	FlagStatus GetNpn(ch_t ch);
	FlagStatus GetPnp(ch_t ch);
	void EnableFunInt(ch_t ch);
	void DisableFunInt(ch_t ch);
	void EnableFupInt(ch_t ch);
	void DisableFupInt(ch_t ch);
	FlagStatus GetPb(pb_t pb);
	void OledClr(oled_ctrl_t ctrl);
	void OledSet(oled_ctrl_t ctrl);
	void LdClr(void);
	void LdSet(void);
	void GPIO_UserInit(void);
	void GPIO_PbScan(uint8_t index);
	key_press_state_t GPIO_ReadPbStatus(uint8_t index);
	bool GPIO_isInputActive(void);
	void GPIO_AnalogSel(uint16_t analog);	
	/* ------------------------------------------------ */

#ifdef cplusplus
}
#endif

#endif
