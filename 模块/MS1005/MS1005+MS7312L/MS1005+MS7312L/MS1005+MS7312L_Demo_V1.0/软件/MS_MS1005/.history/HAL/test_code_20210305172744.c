/**
 * @file test_code.c
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

#include "test_code.h"

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



void Temp_ADC_Test(void)
{

}

void Flash_ReadWrite_Test(void)
{
    uint8_t test_buf[10] = {0};
    uint8_t read_test
    for (uint8_t i = 0; i < 10; i++)
    {
        test_buf[i] = i * 11;
    }

    Flash_Write(COMPENSATION_DATA_ADDRESS, test_buf, 10);
    delay_us(10);

    Flash_Read(COMPENSATION_DATA_ADDRESS, )
    while (1)
    {

    }
}