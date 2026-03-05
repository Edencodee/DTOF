#include "test.h"
#include "systick.h"
#include "user_uart.h"

void ProcessRxData(char data)
{
	//printf("RX_Data: %c\r\n", data);
	switch (data) {
	case (char)'1': {
		//printf("one!\r\n");
		Ms1005_Start();
		delay_ns(1000);
		Ms1005_Stop1();
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


void RecvTimeoutCallBack(uint8_t* buf, uint32_t len)
{
    //UART_SendString(USER_COM1, (char*)buf);
	//printf("\r\n");
	for (int i = 0; i < len; i++) {
          char data = buf[i]; 
          ProcessRxData(data);
    }
}


void delay_ns(uint32_t ns)
{
    uint32_t cycles = ns / 4;  
    if(cycles < 1) cycles = 1;
    
    volatile uint32_t count;
    for(count = 0; count < cycles; count++)
    {
        __NOP();  
    }
}


void delay_us(uint32_t us)
{
    uint32_t cycles = us * 240;  
    
    volatile uint32_t count;
    for(count = 0; count < cycles; count++)
    {
        __NOP();
    }
}


void Ms1005GpioConfig(void)
{
	/* configure TDC GPIO port */
	rcu_periph_clock_enable(TDC_START_CLK);
	gpio_mode_set(TDC_START_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TDC_START_PIN);
	gpio_output_options_set(TDC_START_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TDC_START_PIN);
	GPIO_BC(TDC_START_PORT) = TDC_START_PIN;
	
	rcu_periph_clock_enable(TDC_STOP1_CLK);
	gpio_mode_set(TDC_STOP1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TDC_STOP1_PIN);
	gpio_output_options_set(TDC_STOP1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TDC_STOP1_PIN);
	GPIO_BC(TDC_STOP1_PORT) = TDC_STOP1_PIN;
	
	rcu_periph_clock_enable(TDC_STOP2_CLK);
	gpio_mode_set(TDC_STOP2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TDC_STOP2_PIN);
	gpio_output_options_set(TDC_STOP2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TDC_STOP2_PIN);
	GPIO_BC(TDC_STOP2_PORT) = TDC_STOP2_PIN;
}

void Ms1005_Start(void)
{
	//gpio_bit_set(TDC_START_PORT, TDC_START_PIN);
	GPIO_BOP(TDC_START_PORT) = TDC_START_PIN;
	//delay_ns(5);
	GPIO_BC(TDC_START_PORT) = TDC_START_PIN;
}
void Ms1005_Stop1(void)
{
	GPIO_BOP(TDC_STOP1_PORT) = TDC_STOP1_PIN;
	delay_ns(1);
	GPIO_BC(TDC_STOP1_PORT) = TDC_STOP1_PIN;
}

void Ms1005_Stop2(void)
{
	GPIO_BOP(TDC_STOP2_PORT) = TDC_STOP2_PIN;
	//delay_ns(5);
	GPIO_BC(TDC_STOP2_PORT) = TDC_STOP2_PIN;
}

