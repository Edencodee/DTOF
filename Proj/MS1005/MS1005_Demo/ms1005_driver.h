#ifndef __MS1005_H
#define __MS1005_H

#ifdef cplusplus
extern "C"
{
#endif
	
	#include "gd32f4xx.h"
	#include "wtdint.h"
	
/**
 * @brief GPIO Definitions		引脚定义
 */
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
	
	//TEST_Mode
	#define TDC_START_PIN			GPIO_PIN_4
	#define TDC_START_PORT			GPIOA
	#define TDC_START_CLK			RCU_GPIOA

	#define TDC_STOP1_PIN			GPIO_PIN_4
	#define TDC_STOP1_PORT			GPIOA
	#define TDC_STOP1_CLK			RCU_GPIOA

	#define TDC_STOP2_PIN			GPIO_PIN_4
	#define TDC_STOP2_PORT			GPIOA
	#define TDC_STOP2_CLK			RCU_GPIOA

	//TDC SPI NSS  
	#define TDC_SSN_PIN				GPIO_PIN_0
	#define TDC_SSN_PORT			GPIOB
	#define TDC_SSN_CLK				RCU_GPIOB


/**
 * @brief 操作码
 */
typedef enum
{
    WRITE_CONFIG_RES = 0x80,   //写入配置寄存器          写入 32bit
    READ_STOP1_DATA0 = 0xB0,   //读取地址 STOP1 数据 0   读出 24bit
    READ_STOP1_DATA1 = 0xB1,   //读取地址 STOP1 数据 1   读出 24bit
    READ_STOP1_DATA2 = 0xB2,   //读取地址 STOP1 数据 2   读出 24bit
    READ_STOP1_DATA3 = 0xB3,   //读取地址 STOP1 数据 3   读出 24bit
    READ_STOP1_DATA4 = 0xB4,   //读取地址 STOP1 数据 4   读出 24bit
    READ_STOP1_DATA5 = 0xB5,   //读取地址 STOP1 数据 5   读出 24bit
    READ_STOP1_DATA6 = 0xB6,   //读取地址 STOP1 数据 6   读出 24bit
    READ_STOP1_DATA7 = 0xB7,   //读取地址 STOP1 数据 7   读出 24bit
    READ_STOP1_DATA8 = 0xB8,   //读取地址 STOP1 数据 8   读出 24bit
    READ_STOP1_DATA9 = 0xB9,   //读取地址 STOP1 数据 9   读出 24bit
    READ_STOP2_DATA0 = 0xC0,   //读取地址 STOP2 数据 0   读出 24bit
    READ_STOP2_DATA1 = 0xC1,   //读取地址 STOP2 数据 1   读出 24bit
    READ_STOP2_DATA2 = 0xC2,   //读取地址 STOP2 数据 2   读出 24bit
    READ_STOP2_DATA3 = 0xC3,   //读取地址 STOP2 数据 3   读出 24bit
    READ_STOP2_DATA4 = 0xC4,   //读取地址 STOP2 数据 4   读出 24bit
    READ_STOP2_DATA5 = 0xC5,   //读取地址 STOP2 数据 5   读出 24bit
    READ_STOP2_DATA6 = 0xC6,   //读取地址 STOP2 数据 6   读出 24bit
    READ_STOP2_DATA7 = 0xC7,   //读取地址 STOP2 数据 7   读出 24bit
    READ_STOP2_DATA8 = 0xC8,   //读取地址 STOP2 数据 8   读出 24bit
    READ_STOP2_DATA9 = 0xC9,   //读取地址 STOP2 数据 9   读出 24bit    
    READ_CAL_DATA    = 0xBB,   //读取校验数据(CAL)       读出 24bit
    READ_CONFIG_RES  = 0xBC,   //读取配置寄存器的低8位    读出 8bit
    READ_STAT_RES    = 0xBD,   //读取状态(STAT)          读出 24bit
    INIT_DEVICE      = 0x70,   //初始化指令
    RESET_DEVICE     = 0x50,   //复位指令
    START_CAL_TDC    = 0x04,   //CAL 测量指令
}OPERATE_CODE;

typedef enum
{
    RES1_0 = 0xB0, //STOP1 测量结果1
    RES1_1 = 0xB1, //STOP1 测量结果2
    RES1_2 = 0xB2, //STOP1 测量结果3
    RES1_3 = 0xB3, //STOP1 测量结果4
    RES1_4 = 0xB4, //STOP1 测量结果5
    RES1_5 = 0xB5, //STOP1 测量结果6
    RES1_6 = 0xB6, //STOP1 测量结果7
    RES1_7 = 0xB7, //STOP1 测量结果8
    RES1_8 = 0xB8, //STOP1 测量结果9
    RES1_9 = 0xB9, //STOP1 测量结果10
    RES2_0 = 0xC0, //STOP2 测量结果1
    RES2_1 = 0xC1, //STOP2 测量结果2
    RES2_2 = 0xC2, //STOP2 测量结果3
    RES2_3 = 0xC3, //STOP2 测量结果4
    RES2_4 = 0xC4, //STOP2 测量结果5
    RES2_5 = 0xC5, //STOP2 测量结果6
    RES2_6 = 0xC6, //STOP2 测量结果7
    RES2_7 = 0xC7, //STOP2 测量结果8
    RES2_8 = 0xC8, //STOP2 测量结果9
    RES2_9 = 0xC9, //STOP2 测量结果10
    CAL    = 0xBB, //校正值,整数,24位
} RESULT_RES;


	extern uint32_t TDC_Res1[];
	extern uint32_t TDC_Res2[];
	
	void TDC_Init(void);
	uint8_t TDC_ReadByte (uint8_t op);
	uint32_t TDC_Read3Bytes (uint8_t op);
	uint32_t TDC_ReadCal (void);
	uint32_t TDC_ReadState (void);
	uint32_t TDC_ReadRes (uint8_t op);
	float TDC_ReadResF (uint8_t op);
	void TDC_WriteReg(uint8_t op, uint32_t val);
	void TDC_NcalPulseCfg (void);
	void TDC_CalPulseCfg (void);
	void TDC_MeasureStart(void);
	
#ifdef cplusplus
}
#endif


#endif
