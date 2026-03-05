
#include "data_correction.h"

#include "spad_uart_driver.h"
#include "spad_protocol.h"

#include "main.h"



FlagStatus Get_Meas_Data(SenPF *point);

int16_t SPAD_Data_Offset = 0;



int16_t Data_Correction(uint16_t std_value)
{
	SenPF correction_point = {0, 0};

	while(!Get_Meas_Data(&correction_point)) {
		;
	}

	SPAD_Data_Offset = correction_point.dist - std_value;

	return SPAD_Data_Offset;

}


FlagStatus Get_Meas_Data(SenPF *point)
{
	FlagStatus result = RESET;

	if(!SPAD_Uart_Rx_Flag) {
		while(!SPAD_Uart_Rx_Flag);//等待SPAD测量完毕
	}

	SPAD_Uart_Rx_Flag = 0;
//	if(SPAD_Msg_Parsing(SPAD_Uart_Rx_Data,
//						 point,
//						 SPAD_Uart_Rx_Len,
//						 sizeof(PacketF_1CH_LiDAR))
//	)
	{
		//数据解析成功
		result = SET;
	}

	return result;

}

