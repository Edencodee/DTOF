#ifndef __TEST_H
#define __TEST_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"
#include <stdbool.h>
#include <stdio.h>

/* ---------------------------------------------------- */
#define TDC_START_PIN           	GPIO_PIN_2
#define TDC_START_PORT     			GPIOB
#define TDC_START_CLK      			RCU_GPIOB

#define TDC_STOP1_PIN           	GPIO_PIN_3
#define TDC_STOP1_PORT     			GPIOB
#define TDC_STOP1_CLK      			RCU_GPIOB

#define TDC_STOP2_PIN           	GPIO_PIN_4
#define TDC_STOP2_PORT     			GPIOB
#define TDC_STOP2_CLK      			RCU_GPIOB
/* ---------------------------------------------------- */


/* function declaration ------------------------------- */
    void ProcessRxData(char data);
	void RecvTimeoutCallBack(uint8_t* buf, uint32_t len);
	void Ms1005GpioConfig(void);
	void Ms1005_Start(void);
	void Ms1005_Stop1(void);
	void Ms1005_Stop2(void);
	void delay_ns(uint32_t ns);
	void delay_us(uint32_t us);
/* ---------------------------------------------------- */

#ifdef cplusplus
}
#endif

#endif
