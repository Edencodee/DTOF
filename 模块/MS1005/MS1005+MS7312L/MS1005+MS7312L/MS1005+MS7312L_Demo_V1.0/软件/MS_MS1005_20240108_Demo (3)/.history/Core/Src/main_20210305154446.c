/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "comp.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "board.h"

#include "delay.h"

#include "drv_gpio.h"
#include "drv_usart.h"
#include "drv_temp.h"

//SPAD
#include "spad_uart_driver.h"
#include "spad_interface.h"

//encoder
#include "encoder_processing.h"
#include "scan_msg_processing.h"

#include "tdc_driver.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8 SPAD_Uart_Rx_Buffer[SPAD_RX_SIZE] = {0};
u8 SPAD_Uart_Rx_Data[SPAD_RX_SIZE] = {0};
uint8_t SPAD_RingBuffer[SPAD_RX_SIZE] = {0};

uint8_t scan_msg_frame[SCAN_FRAME_SIZE] = {0};

//uint8_t Meas_Data[SCAN_FRAME_SIZE] = {0};

struct kfifo spad_fifo;
struct kfifo spad_uart_fifo;

__IO u8 SPAD_Uart_Rx_Flag = 0;
__IO u8 SPAD_Uart_Rx_Len = 0;

__IO u8 Scan_Uart_Rx_Flag = 0;

__IO ITStatus Scan_Uart_TxCplt_Flag = SET;

const uint16_t systemclock = 160;

//uint16_t skl100_params.engy_lower_limit = 50; //雷达数据点强度下限
Lidar_Params skl100_params = {SPAD_MODE_1CH, -2000, 20};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t test_buf[12] = "helloword\n";
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define LED2_PIN GET_PIN(A, 5)
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  SenPF tmp_point = {0, 0}, last_point;

  TDC_RESTypeDef tmp_tdc;

  Lidar_PointTypeDef Dist_point;

  static uint16_t get_buf_size = 0;
  uint16_t scan_event_cnt = 0;
  uint16_t scan_msg_size = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  SystemClock_Config();

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */
  delay_init(systemclock);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();

  //	kfifo_init(&spad_fifo,      SPAD_Uart_Rx_Buffer, SPAD_RX_SIZE);
  //	kfifo_init(&spad_uart_fifo, SPAD_RingBuffer,     SPAD_RX_SIZE);

  //	SCAN_UART_Init();
  //	delay_us(100);
  SPAD_UART_Init();
  delay_us(100);

  APD_HV_TIM_Init(1500, 1, 160);

  Tdc_Driver_Init();

  __HAL_UART_ENABLE_IT(&hspad_uart, UART_IT_IDLE);
  //__HAL_UART_DISABLE_IT(&hspad_uart, UART_IT_ERR);
  //__HAL_UART_DISABLE_IT(&hspad_uart, UART_IT_PE);
  if (HAL_UART_Receive_DMA(&hspad_uart, SPAD_Uart_Rx_Buffer, SPAD_RX_SIZE) != HAL_OK)
  {
    Error_Handler();
  }

  delay_ms(2000);
  
  Temp_ADC
  /* USER CODE BEGIN 2 */
  Encoder_Proc_Init();

  while (0)
  {
    //delay_ms(500);
    Dist_point = Meas_SinglePoint();
  }

  while (0) // encoder test
  {
    if ((scan_event_cnt / 2) != (encoder_proc_event_cnt / 2))
    {
      scan_event_cnt = encoder_proc_event_cnt;
      printf("cnt = %d, period = %d %d\n", scan_event_cnt, enc_capt[scan_event_cnt], enc_period);
    }
  }

  printf("Test begin!\n");

  scan_event_cnt = encoder_proc_event_cnt;

  scan_msg_packet.point_cnt = 0;

  while (1)
  {
    /* USER CODE END WHILE */

    Dist_point = Meas_SinglePoint();
    // Dist_point.distance= 3000;
    msg_point[scan_msg_packet.point_cnt].dist = Dist_point.distance;

    scan_msg_packet.point_cnt++;

    //    if ((scan_event_cnt / 2) != (encoder_proc_event_cnt / 2))
    //    {

    //      scan_event_cnt = encoder_proc_event_cnt;

    if (scan_msg_packet.point_cnt == 48)
    {
      //printf("T");

      scan_event_cnt += 2;
      if (scan_event_cnt >= 30)
        scan_event_cnt = 0;

      scan_msg_packet.speed = current_speed;
      scan_msg_packet.angle_start = (scan_event_cnt * 12 + 336) % 360 * 100;
      scan_msg_packet.angle_end = (scan_msg_packet.angle_start +
                                   2400 * (scan_msg_packet.point_cnt - 1) / scan_msg_packet.point_cnt) %
                                  36000;

      /* Wait for the end of the transfer ###################################*/
      while (Scan_Uart_TxCplt_Flag != SET)
      {
        ;
      }
      Scan_Uart_TxCplt_Flag = RESET;

      scan_msg_size = Scan_Msg_Packaging(&scan_msg_packet, scan_msg_frame);

      if (HAL_UART_Transmit_DMA(&hspad_uart, (uint8_t *)scan_msg_frame, scan_msg_size) != HAL_OK)
      {
        Error_Handler();
      }
      //printf("s = %d, cnt = %d %d\n", scan_msg_packet.angle_start, scan_event_cnt, enc_period);
      scan_msg_packet.point_cnt = 0;
    }

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = systemclock / 2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
//void Error_Handler(void)
//{
//  /* USER CODE BEGIN Error_Handler_Debug */
//  /* User can add his own implementation to report the HAL error return state */

//  /* USER CODE END Error_Handler_Debug */
//}
//void error_handler((uint8_t *)__FILE__, __LINE__)
//{
//  printf("error handler: file %s on line %d\r\n", file, line);
//}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  printf("error handler: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
