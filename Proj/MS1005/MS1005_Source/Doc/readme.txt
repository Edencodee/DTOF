0. 环境
mdk 5.39 +GigaDevice::GD32F4xx_DFP@3.2.0
GD32F4xx_Firmware_Library_V3.2.0
GD32F425VGH6 + daplink


1 .板载资源
USART1（串口）
RXD 	PD6		DMA0_CH1
TXD	      	PD5		DMA0_CH5


2.简介
本工程初始化了调试串口，可作为工程模板移植使用
test.c中实现了串口接收测试的方法：
void ProcessRxData(char data)
{
	printf("RX_Data: %c\r\n", data);
	switch (data) {
	case (char)'1': {
		printf("one!\r\n");
		break;
	}
	case (char)'2': {
		printf("two!\r\n");
		break;
	}
	case (char)'3': {
		printf("three!\r\n");
		break;
	}
	default:
		printf("OK!\r\n");
		break;
	}
}


3.本工程用于调试MS1005驱动的触发源信号






