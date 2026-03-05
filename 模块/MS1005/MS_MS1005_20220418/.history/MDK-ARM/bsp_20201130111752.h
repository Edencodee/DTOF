/* USER CODE BEGIN Header */
/****
	******************************************************************************
	* @file           : bsp.h
	* @brief          : board suPort package
	******************************************************************************
	* @attention
	*
	* @History
	* <author>    <time>      <version >       <desc>
	* JinF     2020.09.22     V1.0             creat
	* 	
	* @Description 
	
MCU:
	GPIO:
	ADC:
	UART:
	TIM:
	SPI:
	DMA:
UART_	
TDC:
Encoder:
APDHV_AD
	* @WorkingPlatform
	* SKLidar_MM32F032
	*
	******************************************************************************
	*/	

/* USER CODE END Header */


#ifndef _BSP_H
#define _BSP_H

    /* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;


    /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;


#define     __O     volatile                        ///< Defines 'write only' permissions
#define     __IO    volatile                        ///< Defines 'read / write' permissions

typedef long long    s64;                              ///< used for signed 64bit

typedef signed int   s32;
typedef signed short s16;
typedef signed char  s8;

typedef signed int  const  sc32;                    ///< Read Only
typedef signed short const sc16;                    ///< Read Only
typedef signed char  const sc8;                     ///< Read Only

typedef volatile signed int   vs32;
typedef volatile signed short vs16;
typedef volatile signed char  vs8;

typedef volatile signed int  const  vsc32;          ///< Read Only
typedef volatile signed short const vsc16;          ///< Read Only
typedef volatile signed char  const vsc8;           ///< Read Only

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef unsigned int  const  uc32;                  ///< Read Only
typedef unsigned short const uc16;                  ///< Read Only
typedef unsigned char  const uc8;                   ///< Read Only

typedef volatile unsigned int   vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char  vu8;

typedef volatile unsigned int  const  vuc32;        ///< Read Only
typedef volatile unsigned short const vuc16;        ///< Read Only
typedef volatile unsigned char  const vuc8;         ///< Read Only


// TDC *************************************************************************
#define USE_TDC_SPI

#ifdef USE_TDC_SPI
//#include <tdc_driver.h>
//MM32F032 TDC_SPI
#define TDC_SPI_NAME                     SPI1
#define TDC_SPI_CLK                      RCC_APB2Periph_SPI1

#define TDC_START_Pin                    GPIO_PIN_3
#define TDC_START_GPIO_Port              GPIOA

#define TDC_NSS_Pin                      GPIO_PIN_15
#define TDC_NSS_GPIO_Port                GPIOA

#define TDC_SCK_Pin                      GPIO_PIN_3
#define TDC_SCK_GPIO_Port                GPIOB
#define TDC_SCK_AFSOURCE                 GPIO_PinSource3
#define TDC_SCK_AFMODE                   GPIO_AF_0

#define TDC_MISO_Pin                     GPIO_PIN_4
#define TDC_MISO_GPIO_Port               GPIOB
#define TDC_MISO_AFSOURCE                GPIO_PinSource4
#define TDC_MISO_AFMODE                  GPIO_AF_0

#define TDC_MOSI_Pin                     GPIO_PIN_5
#define TDC_MOSI_GPIO_Port               GPIOB
#define TDC_MOSI_AFSOURCE                GPIO_PinSource5
#define TDC_MOSI_AFMODE                  GPIO_AF_0

#define TDC_INTN_Pin                     GPIO_PIN_6
#define TDC_INTN_GPIO_Port               GPIOB

#define TDC_RSTN_Pin                     GPIO_PIN_7
#define TDC_RSTN_GPIO_Port               GPIOB


/*---------TDC_INTN-----------*/
#define GET_TDC_INTN                    (TDC_INTN_GPIO_Port->IDR &  (uint32_t)TDC_INTN_Pin) 

/*---------TDC_RSTN---------*/
#define TDC_RSTN_0                      (TDC_RSTN_GPIO_Port->BRR  = (uint32_t)TDC_RSTN_Pin)
#define TDC_RSTN_1                      (TDC_RSTN_GPIO_Port->BSRR = (uint32_t)TDC_RSTN_Pin)

/*---------TDC_NSS---------*/
#define TDC_NSS_0                       (TDC_NSS_GPIO_Port->BRR   = (uint32_t)TDC_NSS_Pin)
#define TDC_NSS_1                       (TDC_NSS_GPIO_Port->BSRR  = (uint32_t)TDC_NSS_Pin)

/*---------TDC_START--------*/
#define TDC_START_0                     (TDC_START_GPIO_Port->BRR  = (uint32_t)TDC_START_Pin)
#define TDC_START_1                     (TDC_START_GPIO_Port->BSRR = (uint32_t)TDC_START_Pin)

#define SCK_L                           {TDC_SCK_GPIO_Port->BRR   = (uint32_t)TDC_SCK_Pin;} 
#define SCK_H                           {TDC_SCK_GPIO_Port->BSRR  = (uint32_t)TDC_SCK_Pin;}  
#define MOSI_L                          {TDC_MOSI_GPIO_Port->BRR  = (uint32_t)TDC_MOSI_Pin;}
#define MOSI_H                          {TDC_MOSI_GPIO_Port->BSRR = (uint32_t)TDC_MOSI_Pin;}
#define MISO_IN                         (TDC_MISO_GPIO_Port->IDR &  (uint32_t)TDC_MISO_Pin)
#endif
/* TDC_SPI */

// Encoder *************************************************************************
//Encoder_GPIO
#define ENCODER_GPIO_Port               GPIOA
#define ENCODER_Pin                     GPIO_PIN_7
#define ENCODER_GPIO_CLK                RCC_AHBPeriph_GPIOA			   
#define ENCODER_GPIO_AFSOURCE           GPIO_PinSource7
#define ENCODER_GPIO_AFMODE             GPIO_AF_5


//Encoder_TIM
#define ENCODER_TIMER                   TIM17
#define ENCODER_TIMER_CLK               RCC_APB2Periph_TIM17
#define ENCODER_TIMER_CH                TIM_CHANNEL_1
#define ENCODER_TIMER_FREQ              (1e6)  //Hz
#define ENCODER_TIMER_PRESCALER         (uint16_t)(((SystemCoreClock) / \
                                          ENCODER_TIMER_FREQ) - 1)//APB2 = HCLK
										  
#define ENCODER_TIMER_IT_FLAG           TIM_IT_CC1
#define ENCODER_TIMER_IRQ               TIM17_IRQn
#define ENCODER_TIMER_IRQ_HANDLER       TIM17_IRQHandler
#define ENCODER_GET_CAPTURE_VALUE       TIM_GetCapture1
#define ENCODER_TIMER_IRQ_PRIORITY      10

#define ENCODER_HOLES_CNT               (30)



// SPAD_UART *************************************************************************
#define SPAD_UART                       USART1


//SPAD_UART_GPIO
#define SPAD_UART_GPIO_CLK				RCC_AHBPeriph_GPIOA

#define SPAD_UART_TX_Pin				GPIO_PIN_9
#define SPAD_UART_TX_GPIO               GPIOA

#define SPAD_UART_RX_Pin                GPIO_PIN_10
#define SPAD_UART_RX_GPIO               GPIOA

#define SPAD_UART_GPIO_AFMODE           GPIO_AF7_USART1
//
#define SPAD_UART_BAUD                  (1152000)

#define SPAD_UART_IRQ                   USART1_IRQn
#define SPAD_UART_INT_HANDLER           USART1_IRQHandler

#define SPAD_UART_DMA_RX_CHANNEL        DMA1_Channel1
#define SPAD_UART_DMA_TX_CHANNEL        DMA1_Channel2

//#define MAX_SPAD_POINT_SIZE             256


// SCAN_UART *************************************************************************
#define SCAN_UART                       USART2


//SCAN_UART_GPIO
#define SCAN_UART_GPIO_CLK				RCC_AHBPeriph_GPIOA

#define SCAN_UART_TX_Pin				GPIO_PIN_2
#define SCAN_UART_TX_GPIO               GPIOA

#define SCAN_UART_RX_Pin				GPIO_PIN_3
#define SCAN_UART_RX_GPIO               GPIOA

#define SCAN_UART_GPIO_AFMODE           GPIO_AF7_USART2
//
#define SCAN_UART_BAUD                  (921600)

#define SCAN_UART_RX_IRQ                USART2_IRQn
#define SCAN_UART_RX_INT_HANDLER        USART2_IRQHandler

#define SCAN_UART_DMA_TX_CHANNEL        DMA1_Channel3


#define MAX_SCAN_POINT_SIZE             249


// ADC *************************************************************************
#define INPUT_VOLTAGE_Pin               GPIO_PIN_0
#define INPUT_VOLTAGE_GPIO_Port         GPIOA
#define PD_LIGHT_Pin                    GPIO_PIN_1
#define PD_LIGHT_GPIO_Port              GPIOA

#define ADC_SCAN_NUM                    2
#define ADC_AVERAGE_LEN                 20


/****************************************************************/
/*                   雷达系统参数                                */
/****************************************************************/

typedef struct 
{
	uint16_t work_mode;        //工作模式：default
	int16_t dist_offset;      //距离修正值：
	uint16_t engy_lower_limit; //强度下限；
}Lidar_Params;

#define SPAD_MODE_1CH      (0x00)    //单回波模式 
#define SPAD_MODE_2CH      (0x01)

#define DIST_MAX           (60000)
#define DIST_MIN           (150)
#define DIST_INVALID       (0xFFFF)


typedef struct
{
	uint16_t hardware_version;  // 硬件版本号
	uint16_t software_version;  // 软件版本号
	Lidar_Params lidar_params;
}System_Params;

#endif /* _BSP_H */

