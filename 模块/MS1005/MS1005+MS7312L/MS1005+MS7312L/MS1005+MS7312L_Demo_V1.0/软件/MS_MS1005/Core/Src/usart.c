/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "board.h"
#include "drv_usart.h"


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR=(uint8_t)ch;      
	return ch;
}
#endif 

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef hscan_uart;
UART_HandleTypeDef hspad_uart;
DMA_HandleTypeDef hdma_scan_uart_tx;
DMA_HandleTypeDef hdma_spad_uart_rx;
DMA_HandleTypeDef hdma_spad_uart_tx;

/* USART2 init function */

void SCAN_UART_Init(void)
{
  hscan_uart.Instance = SCAN_UART;
  hscan_uart.Init.BaudRate = SCAN_UART_BAUD;
  hscan_uart.Init.WordLength = UART_WORDLENGTH_8B;
  hscan_uart.Init.StopBits = UART_STOPBITS_1;
  hscan_uart.Init.Parity = UART_PARITY_NONE;
  hscan_uart.Init.Mode = UART_MODE_TX_RX;
  hscan_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hscan_uart.Init.OverSampling = UART_OVERSAMPLING_16;
  hscan_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hscan_uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hscan_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hscan_uart) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hscan_uart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hscan_uart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hscan_uart) != HAL_OK)
  {
    Error_Handler();
  }


}

	
/* USART1 init function */

void SPAD_UART_Init(void)
{

  hspad_uart.Instance = SPAD_UART;
  hspad_uart.Init.BaudRate = SPAD_UART_BAUD;	//Baudrate  115200
  hspad_uart.Init.WordLength = UART_WORDLENGTH_8B;
  hspad_uart.Init.StopBits = UART_STOPBITS_1;
  hspad_uart.Init.Parity = UART_PARITY_NONE;
  hspad_uart.Init.Mode = UART_MODE_TX_RX;
  hspad_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hspad_uart.Init.OverSampling = UART_OVERSAMPLING_16;
  hspad_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hspad_uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hspad_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hspad_uart) != HAL_OK)
  {
    Error_Handler();
  }	
	
  if (HAL_UARTEx_SetTxFifoThreshold(&hspad_uart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hspad_uart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hspad_uart) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  //if(uartHandle->Instance == SCAN_UART)
	if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = SCAN_UART_TX_Pin|SCAN_UART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = SCAN_UART_GPIO_AFMODE;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /* USART2 DMA Init */
    /* USART2_TX Init */
    hdma_scan_uart_tx.Instance = SCAN_UART_DMA_TX_CHANNEL;
    hdma_scan_uart_tx.Init.Request = DMA_REQUEST_USART2_TX;
    hdma_scan_uart_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_scan_uart_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_scan_uart_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_scan_uart_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_scan_uart_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_scan_uart_tx.Init.Mode = DMA_NORMAL;
    hdma_scan_uart_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_scan_uart_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_scan_uart_tx);
  /* USER CODE BEGIN USART1_MspInit 1 */
    
  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance== SPAD_UART)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = SPAD_UART_TX_Pin|SPAD_UART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = SPAD_UART_GPIO_AFMODE;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    hdma_spad_uart_tx.Instance = SPAD_UART_DMA_TX_CHANNEL;
    hdma_spad_uart_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_spad_uart_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spad_uart_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spad_uart_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spad_uart_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spad_uart_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spad_uart_tx.Init.Mode = DMA_NORMAL;
    hdma_spad_uart_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_spad_uart_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_spad_uart_tx);

    /* USART1_RX Init */
    hdma_spad_uart_rx.Instance = SPAD_UART_DMA_RX_CHANNEL;
    hdma_spad_uart_rx.Init.Request = DMA_REQUEST_USART1_RX;
    hdma_spad_uart_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spad_uart_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spad_uart_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spad_uart_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spad_uart_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spad_uart_rx.Init.Mode = DMA_CIRCULAR;
    hdma_spad_uart_rx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_spad_uart_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_spad_uart_rx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(SPAD_UART_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(SPAD_UART_IRQ);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
		

	//HAL_UART_Receive_IT(&hspad_uart, SPAD_Uart_Rx_Buffer, DMA_SPAD_RX_SIZE);

		
  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, SPAD_UART_TX_Pin|SPAD_UART_RX_Pin);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, SCAN_UART_TX_Pin|SCAN_UART_RX_Pin);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void SPAD_UART_IDLE_Process(UART_HandleTypeDef *huart)
{
	static uint32_t last_count = SPAD_RX_SIZE;
	uint32_t temp = 0;
	
	if (huart->Instance == SPAD_UART)
	{
		
		SPAD_Uart_Rx_Flag	= SET;	

//		temp = __HAL_DMA_GET_COUNTER(&hdma_spad_uart_rx); 
//		SPAD_Uart_Rx_Len = (last_count - temp) & (SPAD_RX_SIZE - 1);
//		last_count = temp;
//		
//		//数据保存
//		memcpy(SPAD_Uart_Rx_Data, SPAD_Uart_Rx_Buffer, SPAD_Uart_Rx_Len);	
//		
//		if (HAL_UART_Receive_DMA(&hspad_uart, SPAD_Uart_Rx_Buffer, SPAD_RX_SIZE) != HAL_OK)
//		{
//			Error_Handler();
//		}		
		//无锁fifo
		temp = __HAL_DMA_GET_COUNTER(&hdma_spad_uart_rx);
		spad_fifo.in += (last_count - temp) & (spad_fifo.size - 1);
		//SPAD_Uart_Rx_Len = spad_fifo.in - spad_fifo.out;
		last_count = temp;
		
//		if(spad_fifo.in - spad_fifo.out > spad_fifo.size) // 不能防止数据被覆盖，但可以提醒用户出现了覆盖情况
//		{
//				printf("fifo is full,please add fifo size!!!\n");
//		}		
	}
	
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if (UartHandle->Instance == SPAD_UART)
	{	
		  SPAD_Uart_Rx_Flag	= SET;	
	}
}
/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of DMA Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
	if (UartHandle->Instance == SPAD_UART)
	{
		Scan_Uart_TxCplt_Flag = SET;
	}

}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
