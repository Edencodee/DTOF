

#include "spad_interface.h"
#include "main.h"
#include "usart.h"

SPAD_CMD_ISTR stop_mode_cmd = {0};	
void UART1_Send_Group(uint8_t *buff, size_t size)
{
	HAL_UART_Transmit(&hspad_uart, buff, size, 1000);
//			if (HAL_UART_Transmit_DMA(&hscan_uart, (uint8_t *)buff, size) != HAL_OK)
//			{
//				Error_Handler();
//			}	
}
/*功能描述：配置SPAD命令指令帧 	
**输入参数：cmd：存放命令帧首地址
**输入参数：cmd_type：命令帧类型
**输入参数：cmd_code：命令帧副脚本
**输出参数：无
**备注说明：v1.0
**/
void SPAD_Stop_Mode_Cmd(void)
{	
	
	SPAD_CMD_ISTR_FRAME(&stop_mode_cmd, Basic_Setting_Cmd, STOP_MODE);

	UART1_Send_Group((u8 *)&stop_mode_cmd, sizeof(SPAD_CMD_ISTR));
		
	
}

/*功能描述：配置SPAD命令指令帧 	
**输入参数：cmd：存放命令帧首地址
**输入参数：cmd_type：命令帧类型
**输入参数：cmd_code：命令帧副脚本
**输出参数：无
**备注说明：v1.0
**/
void SPAD_Self_Test_Cmd(void)
{
	SPAD_CMD_ISTR self_test_cmd;
	
	SPAD_CMD_ISTR_FRAME(&self_test_cmd, Self_Test_Cmd, SELF_TEST_MODE);

	UART1_Send_Group((u8 *)&self_test_cmd, sizeof(SPAD_CMD_ISTR));

	
}

void SPAD_Start_Mode_Cmd(void)
{
	SPAD_CMD_ISTR start_mode_cmd;		
	
	SPAD_CMD_ISTR_FRAME(&start_mode_cmd, Basic_Setting_Cmd, START_MODE);

	UART1_Send_Group((u8 *)&start_mode_cmd, sizeof(SPAD_CMD_ISTR));

	
}

FlagStatus SPAD_Stop_Meas(void)
{
	FlagStatus result = RESET;
	uint8_t timeout = 0xFF;
	
	SPAD_Stop_Mode_Cmd();
	
	while(SPAD_Uart_Rx_Flag)
	{
		SPAD_Uart_Rx_Flag = 0;
		
		SPAD_Stop_Mode_Cmd();
		
		delay_us(100);
		timeout--;
		if(!timeout)
		{
			return result;
		}
		
		
	}
	result = SET;
	return result;

}

FlagStatus SPAD_Start_Meas(void)
{
	FlagStatus result = RESET;
	uint8_t timeout = 0xFF;
	
	SPAD_Start_Mode_Cmd();
	delay_us(100);
	
	while(!SPAD_Uart_Rx_Flag)
	{
		delay_us(100);
		timeout--;
		if(!timeout)
		{
			return result;
		}
		SPAD_Start_Mode_Cmd();
		
	}
	result = SET;
	return result;

}

FlagStatus SPAD_Self_Test(void)
{
	FlagStatus result = SPAD_Stop_Meas();
	uint8_t timeout = 0xFF;
	
	if(!result)
	{
		printf("spad stop fail!\n");
		return result;
	}
	
	//发送自我测试命令
	SPAD_Self_Test_Cmd();
	delay_us(100);
	
	while(!SPAD_Uart_Rx_Flag)
	{
		SPAD_Self_Test_Cmd();
		
		delay_us(100);		
		timeout--;
		if(!timeout)//超时
		{
			return result;
		}	
		
	}
	
	//自我测试回传数据解析
		
	SPAD_Uart_Rx_Flag = 0;
	
//	UART_DMA_Wait();
//			
//	memcpy(UART_DMA_TX_Buffer, SPAD_Uart_Rx_Data, SPAD_Uart_Rx_Len);
//	UART_DMA_Print((uint32_t)UART_DMA_TX_Buffer, SPAD_Uart_Rx_Len);		
	return SET;
}


FlagStatus SPAD_Meas_Mode(ModeSettingCMD_TYPE meas_mode)
{
	SPAD_CMD_ISTR meas_mode_cmd;
	SPAD_CMD_ISTR read_rpt_mode_cmd;

	FlagStatus result = RESET;
	uint8_t timeout = 0xFF;

	//设置报点模式
	SPAD_CMD_ISTR_FRAME(&meas_mode_cmd, Mode_Setting_Cmd, meas_mode);
	
	//读取点模式
	SPAD_CMD_ISTR_FRAME(&read_rpt_mode_cmd, Mode_Setting_Cmd, READ_RPT_MODE);
	
	while(!result)
	{
				
		//设置报点模式
		UART1_Send_Group((u8 *)&meas_mode_cmd, sizeof(SPAD_CMD_ISTR));	

		delay_us(100);

		//读取抱点模式
		UART1_Send_Group((u8 *)&read_rpt_mode_cmd, sizeof(SPAD_CMD_ISTR));	

		delay_us(100);	
		
		if(SPAD_Uart_Rx_Flag)
		{
			SPAD_Uart_Rx_Flag = 0;
			
			result = SET;
			
		}
	
		delay_us(100);
		timeout--;
		if(!timeout)
		{
			return result;
		}		
	}

	return result;

}

FlagStatus SPAD_Engineer_Mode(void)
{
	SPAD_CMD_ISTR engineer_mode_cmd;
	SPAD_CMD_ISTR read_rpt_mode_cmd;
	
	FlagStatus result = RESET;
	uint8_t timeout = 0xFF;

	//设置工程报点模式
	SPAD_CMD_ISTR_FRAME(&engineer_mode_cmd, Engineer_Mode_Cmd, ENGINEERIONG_MODE);
	
	//读取抱点模式
	SPAD_CMD_ISTR_FRAME(&read_rpt_mode_cmd, Mode_Setting_Cmd, READ_RPT_MODE);
	
	while(!result)
	{
				
		//设置报点模式
		UART1_Send_Group((u8 *)&engineer_mode_cmd, sizeof(SPAD_CMD_ISTR));	

		delay_us(100);

		//读取抱点模式
		UART1_Send_Group((u8 *)&read_rpt_mode_cmd, sizeof(SPAD_CMD_ISTR));	

		delay_us(100);	
		
		if(SPAD_Uart_Rx_Flag)
		{
			SPAD_Uart_Rx_Flag = 0;
			
			result = SET;
			
		}
	
		delay_us(100);
		timeout--;
		if(!timeout)
		{
			return result;
		}		
	}

	return result;

}
