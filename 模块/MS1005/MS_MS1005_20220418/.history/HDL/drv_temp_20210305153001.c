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

#define 
static uint16_t ADC_ConvertData[10] = {0};

static uint16_t Get_ADC_Value(void)
{
    uint16_t adc_value = 0;
    uint16_t adc_timeout = 0xFFFF;

    /* 开始 ADC_DMA 采样 */
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_ConvertData, 2) != HAL_OK)
    {
        printf("adc dma start failed\n");
	}

    /* 等待采样完成 */
    while (ADC_DMA_ConvCplt_Flag != SET)
    {

    }


    return adc_value;
}

void Temp_ADC_Init(void)
{
    MX_ADC1_Init();

    delay_us(10);

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }    
}

int16_t Get_Temp_Value(void)
{

    uint16_t temperature = 0xFFFF;
    float temp_adc_voltage = ((float)Get_ADC_Value() * 3.3f) / 4095;

    temperature = (1858.3 - temp_adc_voltage * 1000) / 11.67;
    
    return temperature;
}