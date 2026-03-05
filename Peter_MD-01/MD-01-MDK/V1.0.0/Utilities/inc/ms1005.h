#ifndef __MS1005_H
#define __MS1005_H

#ifdef cplusplus
extern "C"
{
#endif
	
	#include "gd32f4xx.h"
	#include "wtdint.h"
	
	#define TDC_RSTN_PIN			GPIO_PIN_4
	#define TDC_RSTN_PORT			GPIOA
	#define TDC_RSTN_CLK			RCU_GPIOA

	#define TDC_INTN_PIN			GPIO_PIN_1
	#define TDC_INTN_PORT			GPIOA
	#define TDC_INTN_CLK			RCU_GPIOA
	#define TDC_INTN_IRQn			EXTI1_IRQn
	#define TDC_INTN_PORT_SOURCE	EXTI_SOURCE_GPIOA
	#define TDC_INTN_PIN_SOURCE		EXTI_SOURCE_PIN1
	#define TDC_INTN_LINE			EXTI_1
	
	#define TDC_SSN_PIN				GPIO_PIN_0
	#define TDC_SSN_PORT			GPIOB
	#define TDC_SSN_CLK				RCU_GPIOB

	#define TDC_POWER_ON_RESET		0x50
	#define TDC_INIT				0x70
	#define TDC_START_CAL_TDC		0x04	
	#define TDC_CFG_REG				0x80
	#define STOP_N					1
	
	extern uint32_t TDC_Res1[];
	extern uint32_t TDC_Res2[];
	
	void TDC_Init(void);
	uint8_t TDC_ReadByte (uint8_t op);
	uint8_t TDC_ReadCfgLow8 (void);
	uint32_t TDC_Read3Bytes (uint8_t op);
	uint32_t TDC_ReadCal (void);
	uint32_t TDC_ReadState (void);
	uint32_t TDC_ReadRes (uint8_t op);
	float TDC_ReadResF (uint8_t op);
	void TDC_WriteReg(uint8_t op, uint32_t val);
	void TDC_Ncal1PulseCfg (void);
	void TDC_Cal1PulseCfg (void);
	void TDC_MeasureStart(void);
	
#ifdef cplusplus
}
#endif


#endif
