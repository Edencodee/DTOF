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


static uint16_t Get_ADC_Value(void)
{
    uint16_t adc_value = 0;

    return adc_value;
}


int16_t Get_Temp_Value(void)
{

    uint16_t temperature = 0xFFFF;
    float temp_adc_voltage = ((float)Get_ADC_Value() * 3.3f) / 4095;

    temperature = ((1858.3 - temp_adc_voltage * 1000) / 11.67;return temperature;
}