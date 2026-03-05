/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32g4xx_hal.h"

#include "board.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
//void Error_Handler(void);

#define Error_Handler()   assert_failed((uint8_t *)__FILE__, __LINE__)

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
//#define SCAN_UART_BAUD 921600
//#define SPAD_UART_BAUD 1152000
//#define ADC_SCAN_NUM 2
//#define ADC_AVERAGE_LEN 20
//#define INPUT_VOLTAGE_Pin GPIO_PIN_0
//#define INPUT_VOLTAGE_GPIO_Port GPIOA
//#define PD_LIGHT_Pin GPIO_PIN_1
//#define PD_LIGHT_GPIO_Port GPIOA
//#define SPAD_UART_TX_Pin GPIO_PIN_2
//#define SPAD_UART_TX_GPIO_Port GPIOA
//#define SPAD_UART_RX_Pin GPIO_PIN_3
//#define SPAD_UART_RX_GPIO_Port GPIOA
//#define ENCODER__Pin GPIO_PIN_7
//#define ENCODER__GPIO_Port GPIOA
//#define SCAN_UART_TX_Pin GPIO_PIN_9
//#define SCAN_UART_TX_GPIO_Port GPIOA
//#define SCAN_UART_RX_Pin GPIO_PIN_10
//#define SCAN_UART_RX_GPIO_Port GPIOA
//#define SWDIO_Pin GPIO_PIN_13
//#define SWDIO_GPIO_Port GPIOA
//#define SWCLK_Pin GPIO_PIN_14
//#define SWCLK_GPIO_Port GPIOA
//#define TDC_NSS_Pin GPIO_PIN_15
//#define TDC_NSS_GPIO_Port GPIOA
//#define TDC_SCK_Pin GPIO_PIN_3
//#define TDC_SCK_GPIO_Port GPIOB
//#define TDC_MISO_Pin GPIO_PIN_4
//#define TDC_MISO_GPIO_Port GPIOB
//#define TDC_MOSI_Pin GPIO_PIN_5
//#define TDC_MOSI_GPIO_Port GPIOB
//#define TDC_INTN_Pin GPIO_PIN_6
//#define TDC_INTN_GPIO_Port GPIOB
//#define TDC_RSTN_Pin GPIO_PIN_7
//#define TDC_RSTN_GPIO_Port GPIOB

#define SPAD_RX_SIZE           RX_BUFF_MAX

#define SCAN_FRAME_SIZE        1024

/* USER CODE BEGIN Private defines */
extern const uint16_t systemclock;

extern u8 SPAD_Uart_Rx_Buffer[SPAD_RX_SIZE];
extern u8 SPAD_Uart_Rx_Data[SPAD_RX_SIZE];

extern struct kfifo spad_fifo;

extern __IO u8 SPAD_Uart_Rx_Flag;
extern __IO u8 SPAD_Uart_Rx_Len;

extern __IO u8 Scan_Uart_Rx_Flag;

extern __IO ITStatus Scan_Uart_TxCplt_Flag;

extern Lidar_Params skl100_params;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
