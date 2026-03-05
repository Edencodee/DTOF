#ifndef __GPIO_H
#define __GPIO_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"
#include <stdbool.h>

/* led IO --------------------------------------------- */
#define LED_N               	1U
#define LED1_PIN           		GPIO_PIN_0	
#define LED1_PORT     			GPIOA
#define LED1_CLK      			RCU_GPIOA
/* ---------------------------------------------------- */

/* led typedef ---------------------------------------- */
typedef enum {
	kLed1 = 0,
	kLed2
} led_t;

typedef enum {
	kLedOff = 0,
	kLedOn,
	kLedToggle,
	kLedNone
} led_state_t;
/* ---------------------------------------------------- */


/* function declaration ------------------------------- */
void LedClr(led_t led);
void LedSet(led_t led);
void GPIO_UserInit(void);
/* ---------------------------------------------------- */

#ifdef cplusplus
}
#endif

#endif
