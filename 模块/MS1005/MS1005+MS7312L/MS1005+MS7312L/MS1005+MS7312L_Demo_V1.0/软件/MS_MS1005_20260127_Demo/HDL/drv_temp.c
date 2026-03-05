/**
 * @file drv_temp.c
 * @brief
 * @author jinf (jinf_li@foxmail.com)
 * @version 1.0
 * @date 2021-03-05
 *
 * @copyright Copyright (c) 2021
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-03-05 <td>1.0     <td>jinf    <td>内容
 * </table>
 */


#include "drv_temp.h"
#include "adc.h"
#include "delay.h"

#define ADC_TEST_LOG 1 //ADC 测试 log 标志位

#define ADC_CONVERT_TIMES   10
static uint16_t ADC_ConvertData[ADC_CONVERT_TIMES] = {0};

static uint16_t Get_ADC_Value(void)
{
	uint16_t adc_value = 0x00;
	uint16_t adc_timeout = 0xFFFF;

	/* 开始 ADC_DMA 采样 */
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_ConvertData, ADC_CONVERT_TIMES) != HAL_OK) {
		;
#if ADC_TEST_LOG
		printf("adc dma start failed\n");
#endif
	}

	/* 等待采样完成 */
	while (ADC_DMA_ConvCplt_Flag != SET) {
		if ((adc_timeout--) == 0) {
#if ADC_TEST_LOG
			printf("adc timeout\n");
#endif
			return 0xFFFF;
		}
	}

	/* 停止ADC采样 */
	if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
		Error_Handler();
	}
	/* 复位采样标志位 */
	ADC_DMA_ConvCplt_Flag = RESET;

	/* 采样数据处理 */
	for (int i = 0; i < ADC_CONVERT_TIMES; i++) {
		adc_value += ADC_ConvertData[i];
	}
	adc_value /= ADC_CONVERT_TIMES;

	return adc_value;
}

void Temp_ADC_Init(void)
{
	MX_ADC1_Init();

	delay_us(10);

	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
		/* Calibration Error */
		Error_Handler();
	}
}

int16_t Get_Temp_Value(void)
{

	int16_t temperature = 0xFFFF;
	float temp_adc_voltage = ((float)Get_ADC_Value() * 3.3f) / 4095;

#if ADC_TEST_LOG
	printf("adc_voltage = %5.2f\n", temp_adc_voltage);
#endif

	temperature = (int16_t)(((1858.3f - temp_adc_voltage * 1000.0f) / 11.67f) * 100);

	return temperature;
}

