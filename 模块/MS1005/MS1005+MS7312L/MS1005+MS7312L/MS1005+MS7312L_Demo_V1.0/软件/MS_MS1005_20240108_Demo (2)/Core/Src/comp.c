/**
  ******************************************************************************
  * File Name          : COMP.c
  * Description        : This file provides code for the configuration
  *                      of the COMP instances.
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
#include "comp.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

COMP_HandleTypeDef hcomp2;

/* COMP2 init function */
void MX_COMP2_Init(void)
{

	hcomp2.Instance = COMP2;
	hcomp2.Init.InputPlus = COMP_INPUT_PLUS_IO1;
	hcomp2.Init.InputMinus = COMP_INPUT_MINUS_1_4VREFINT;//COMP_INPUT_MINUS_3_4VREFINT;
	hcomp2.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
	hcomp2.Init.Hysteresis = COMP_HYSTERESIS_NONE;//COMP_HYSTERESIS_60MV;
	hcomp2.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
	hcomp2.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
	if (HAL_COMP_Init(&hcomp2) != HAL_OK) {
		Error_Handler();
	}

	/* Start COMP2 */
	if(HAL_COMP_Start(&hcomp2) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
}

void HAL_COMP_MspInit(COMP_HandleTypeDef* compHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(compHandle->Instance == COMP2) {
		/* USER CODE BEGIN COMP2_MspInit 0 */

		/* USER CODE END COMP2_MspInit 0 */

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**COMP2 GPIO Configuration
		PA7     ------> COMP2_INP
		*/
		GPIO_InitStruct.Pin = ENCODER_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ENCODER_GPIO_Port, &GPIO_InitStruct);

		/* USER CODE BEGIN COMP2_MspInit 1 */

		/* USER CODE END COMP2_MspInit 1 */
	}
}

void HAL_COMP_MspDeInit(COMP_HandleTypeDef* compHandle)
{

	if(compHandle->Instance == COMP2) {
		/* USER CODE BEGIN COMP2_MspDeInit 0 */

		/* USER CODE END COMP2_MspDeInit 0 */

		/**COMP2 GPIO Configuration
		PA7     ------> COMP2_INP
		*/
		HAL_GPIO_DeInit(ENCODER_GPIO_Port, ENCODER_Pin);

		/* USER CODE BEGIN COMP2_MspDeInit 1 */

		/* USER CODE END COMP2_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
