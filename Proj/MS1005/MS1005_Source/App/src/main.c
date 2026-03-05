#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "wtdint.h"
#include "test.h"
#include "user_uart.h"
#include "user_gpio.h"


volatile bool g_isLogEn = true;
volatile uint8_t g_status = 0;

static void BSP_Init(void) {
	/* 1. configure systick */
	systick_config();
	/* 2. GPIO configure */
	GPIO_UserInit();
	/* 3. USART configure */
	UART_UserInit(USER_COM1,115200);
	UART_UserDMAInit();
	delay_1ms(10);
}

int main(void)
{
    BSP_Init();
	Ms1005GpioConfig();
	printf("TDC Test.\r\n");
    while(1) {
		if (g_uartBuf.currIdx != g_uartBuf.actIdx) {
			RecvTimeoutCallBack(g_uartBuf.rxBuf[g_uartBuf.currIdx], \
				g_uartBuf.rxLen[g_uartBuf.currIdx]);
			g_uartBuf.currIdx = (g_uartBuf.currIdx + 1) % 8;
		}
		Ms1005_Start();
		//GPIO_BC(TDC_START_PORT) = TDC_START_PIN;
		//delay_ns(10);
		Ms1005_Stop1();
		Ms1005_Stop1();
		//delay_ns(20);
		//Ms1005_Stop2();
		//Ms1005_Start();
		//GPIO_BC(TDC_START_PORT) = TDC_START_PIN;
		delay_us(5);
    }
}



