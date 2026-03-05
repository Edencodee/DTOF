#include "stm32f10x.h"
#include "stdio.h"
#include <string.h>
#include "1003.h"
#include "W5500.h"


void Delay_us(uint32_t xus);
void RCC_Configuration(void);		//设置系统时钟为72MHZ(这个可以根据需要改)
void NVIC_Configuration(void);		//STM32中断向量表配配置
void Timer2_Init_Config(void);		//Timer2初始化配置
void System_Initialization(void);	//STM32系统初始化函数(初始化STM32时钟及外设)
void Delay(unsigned int d);			//延时函数(ms)
void UART1_Init(void);				//STM32串口初始化
void MS1003_Initialization(void);

void MS1003_START();             	//模拟短暂的高电平脉冲的START信号
void MS1003_STOP();					//模拟短暂的高电平脉冲的STOP信号

void CAL_INIT();
void ReadResult();

void CAL_PLUSE_1(void);
void NCAL_PLUSE_1(void);
void MS1003_SPI1_Send_Byte(unsigned char dat);

unsigned int Timer2_Counter = 0; //Timer2定时器计数变量(ms)
unsigned int MS1003_Send_Delay_Counter = 0;
unsigned char reg;
unsigned char USART1_RX_flag;

#if 1
#pragma import(__use_no_semihosting)             //标准库需要的支持函数                 
struct __FILE {
	int handle;
};
FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while( !(USART1->SR & (1 << 7)) );
	USART_SendData(USART1, (uint8_t)ch);
	return ch;
}

#endif


void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				
	SysTick->VAL = 0x00;					
	SysTick->CTRL = 0x00000005;				
	while(!(SysTick->CTRL & 0x00010000));	
	SysTick->CTRL = 0x00000004;				
}

/*******************************************************************************
* 函数名  : System_Initialization
* 描述    : STM32系统初始化函数(初始化STM32时钟及外设)
* 输入    : 无
* 输出    : 无
* 返回    : 无
* 说明    : 无
*******************************************************************************/

void System_Initialization(void)
{
	RCC_Configuration();			//设置系统时钟为72MHZ(这个可以根据需要改)
	NVIC_Configuration();			//STM32中断向量表配配置
	MS1003_SPI_Configuration();		//MS1003 SPI初始化配置(STM32 SPI1)
//	W5500_SPI_Configuration();		//W5500  SPI初始化配置(STM32 SPI2)
	Timer2_Init_Config();			//Timer2初始化配置
	MS1003_GPIO_Configuration();	//1003 GPIO初始化配置
}

/*******************************************************************************
* 函数名  : RCC_Configuration
* 描述    : 设置系统时钟为72MHZ(这个可以根据需要改)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : STM32F107x和STM32F105x系列MCU与STM32F103x系列MCU时钟配置有所不同
*******************************************************************************/
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;               //外部高速时钟(HSE)的工作状态变量

	RCC_DeInit();                               //将所有与时钟相关的寄存器设置为默认值
	RCC_HSEConfig(RCC_HSE_ON);                  //启动外部高速时钟HSE
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); //等待外部高速时钟(HSE)稳定

	if(SUCCESS == HSEStartUpStatus) {           //如果外部高速时钟已经稳定
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //Flash设置
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);

		RCC_HCLKConfig(RCC_SYSCLK_Div1); //设置AHB时钟等于系统时钟(1分频)/72MHZ
		RCC_PCLK2Config(RCC_HCLK_Div1);  //设置APB2时钟和HCLK时钟相等/72MHz(最大为72MHz)
		RCC_PCLK1Config(RCC_HCLK_Div2);  //设置APB1时钟是HCLK时钟的2分频/36MHz(最大为36MHz)

//#ifndef STM32F10X_CL                 //如果使用的不是STM32F107x或STM32F105x系列MCU,PLL以下配置
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //PLLCLK = 8MHz * 9 = 72 MHz
		//#else                                //如果使用的是STM32F107x或STM32F105x系列MCU,PLL以下配置
		/***** 配置PLLx *****/
		/* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
		/*   RCC_PREDIV2Config(RCC_PREDIV2_Div5);
		 RCC_PLL2Config(RCC_PLL2Mul_8);
		 RCC_PLL2Cmd(ENABLE); //使能PLL2
		 while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET);//等待PLL2稳定
		*/
		/* PLL configuration: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */
		/* RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
		#endif*/

		RCC_PLLCmd(ENABLE); //使能PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //等待PLL稳定

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //设置系统时钟的时钟源为PLL

		while(RCC_GetSYSCLKSource() != 0x08);               //检查系统的时钟源是否是PLL
		RCC_ClockSecuritySystemCmd(ENABLE);                 //使能系统安全时钟

		/* Enable peripheral clocks --------------------------------------------------*/
		/* Enable I2C1 and I2C1 clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

		/* Enable GPIOA GPIOB SPI1 and USART1 clocks */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
		                       | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
		                       | RCC_APB2Periph_AFIO, ENABLE);
	}
}
/*******************************************************************************
* 函数名  : NVIC_Configuration
* 描述    : STM32中断向量表配配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 设置KEY1(PC11)的中断优先组
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;						//定义NVIC初始化结构体

	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//设置中断优先级组为1，优先组(可设0～4位)

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//设置中断向量号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//设置抢先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能NVIC
	NVIC_Init(&NVIC_InitStructure);

	MS1003_NVIC_Configuration();
}

/*******************************************************************************
* 函数名  : Timer2_Init_Config
* 描述    : Timer2初始化配置
* 输入    : 无
* 输出    : 无
* 返回    : 无
* 说明    : 无
*******************************************************************************/
void Timer2_Init_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//使能Timer2时钟

	TIM_TimeBaseStructure.TIM_Period = 9;						//设置在下一个更新事件装入活动的自动重装载寄存器周期的值(计数到10为1ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;					//设置用来作为TIMx时钟频率除数的预分频值(10KHz的计数频率)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分割:TDTS = TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); 					//使能TIM2指定的中断

	TIM_Cmd(TIM2, ENABLE);  									//使能TIMx外设
}

/*******************************************************************************
* 函数名  : TIM2_IRQHandler
* 描述    : 定时器2中断断服务函数
* 输入    : 无
* 输出    : 无
* 返回    : 无
* 说明    : 无
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		Timer2_Counter++;
		MS1003_Send_Delay_Counter++;
	}
}
/*******************************************************************************
* 函数名  : UART1_Init
* 描述    : STM32串口初始化配置(STM32 SPI1)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //UART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //
	//UART1_TX   GPIOA_Pin_9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOA_Pin_9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);                    //配置TX口
	//UART1_RX      GPIOA_Pin_10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //GPIOA_Pin_10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);									//配置RX口
	//UART1通讯参数配置
	USART_InitStructure.USART_BaudRate = 115200;                  //通讯波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;        //1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;       //
	USART_Init(USART1, &USART_InitStructure);
	//Uart1 NVIC  中断配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       	//IRQ
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          //数据接收中断使能
	NVIC_Init(&NVIC_InitStructure);
	USART_Cmd(USART1, ENABLE);
}
/*******************************************************************************
* 函数名  : Delay
* 描述    : 延时函数(ms)
* 输入    : d:延时系数，单位为毫秒
* 输出    : 无
* 返回    : 无
* 说明    : 延时是利用Timer2定时器产生的1毫秒的计数来实现的
*******************************************************************************/
void Delay(unsigned int d)
{
	Timer2_Counter = 0;
	while(Timer2_Counter < d);
}

void USART1_IRQHandler(void)                    //串口1中断服务程序
{
	unsigned char  Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) ) {
		//    Res =USART_ReceiveData(USART1);    //读取接收到的数据
		//   if(Res == 0x55)//对于接收指定长度的字符串
		//  {
		USART1_RX_flag = 1;      //记录接收到的值
		Res = USART_ReceiveData(USART1); //为1，则将串口1的数据给变量
		USART_SendData(USART1, Res);  //继续将此数据从USART1发送出去
		//  }
	}
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
	/*     if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
	     {
	         USART_ReceiveData(USART1);
	         USART_ClearFlag(USART1,USART_FLAG_ORE);
	     }*/
	USART_ClearFlag(USART1, USART_IT_RXNE); //一定要清除接收中断
}

unsigned char INTN_fig(int delay)
{
	while(Intn_flag == 1) {
		delay--;
		if(!delay) {
			return 1;
		}
		// Delay(1);
	}
	return 0;
}


/*校准模式预期一脉冲，实际一脉冲*/
void CAL_INIT()
{
	Write_MS1003_1Byte_Addr(ADDR1); 			//MS1003写寄存器地址
	MS1003_SPI1_Send_Byte(0x02);
	MS1003_SPI1_Send_Byte(0x3a);
	MS1003_SPI1_Send_Byte(0x60);
	MS1003_SPI1_Send_Byte(0x10);
	GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //置MS1003的SCS为高电平
	Write_MS1003_1Byte(Init);
	
}
void ReadResult()
{
	if(!INTN_fig(50)) {
		float  i, i1;
		unsigned char k;
		printf("jiaozhun:");
		i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
		i = Read_MS1003_NCAL_3Byte(0xbd);
		k =	Read_MS1003_PLUSE_3Byte();
		printf("   state:");
		printf("%.0lf", i);
		printf("   reg0:");
		printf("%.2lf", i1);
		printf("\r\n");
	}
}

/*校准模式预期一脉冲，实际一脉冲*/
void CAL_PLUSE_1()
{
	float  i, i1;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003写寄存器地址
	MS1003_SPI1_Send_Byte(0x02);
	MS1003_SPI1_Send_Byte(0x3a);
	MS1003_SPI1_Send_Byte(0x60);
	MS1003_SPI1_Send_Byte(0x10);
	GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //置MS1003的SCS为高电平
	Intn_flag = 1;

	while(1) {
		//	i1 = Read_MS1003_NCAL_3Byte(0xbC);
		Intn_flag = 1;
		Write_MS1003_1Byte(Init);
		MS1003_START();
		MS1003_STOP();
		key_flag = 0;
		if(!INTN_fig(50)) {
			printf("jiaozhun:");
			i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
			i = Read_MS1003_NCAL_3Byte(0xbd);
			k =	Read_MS1003_PLUSE_3Byte();
			printf("   state:");
			printf("%.0lf", i);
			printf("   reg0:");
			printf("%.2lf", i1);
			printf("\r\n");
		}
		//if (key_flag == 1) {
		//	CAL_PLUSE_2();
		//}
	}
}



/*非校准模式预计十脉冲，实际一脉冲*/
void NCAL_PLUSE_1()
{
	float  m, i, i1;
	int n;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003写寄存器地址
	MS1003_SPI1_Send_Byte(0x0B);
	MS1003_SPI1_Send_Byte(0x3a);
	MS1003_SPI1_Send_Byte(0x40);
	MS1003_SPI1_Send_Byte(0x60);
	GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //置MS1003的SCS为高电平
	Intn_flag = 1;

	while(1) {
		Intn_flag = 1;
		Write_MS1003_1Byte(Init);
		Write_MS1003_1Byte(0x04);
		if(!INTN_fig(5000)) {
			m  = Read_MS1003_NCAL_3Byte(0xbb);
		}
		Write_MS1003_1Byte(Init);

		MS1003_START();
		Delay_us(1);
		MS1003_STOP();
		key_flag = 0;
		if(!INTN_fig(5000)) {
			printf("feijiaozhun:");
			i1 = Read_MS1003_NCAL_3Byte(0xb0);
			i = Read_MS1003_NCAL_3Byte(0xbd);

			k =	Read_MS1003_PLUSE_3Byte();
			printf("   state:");
			printf("%.0lf", i);
			printf("   CAL:");
			printf("%.0lf", m);
			printf("   reg0:");
			printf("%.0lf", i1);
			printf("\r\n");
		}
		//if (key_flag == 1) {
		//	NCAL_PLUSE_2();
		//}

	}
}

int main(void)
{
	System_Initialization();		//STM32系统初始化函数(初始化STM32时钟及外设)
	UART1_Init();					//STM32串口初始?
	MS1003_Initialization();    	//1003初始化并配置32位寄存器?
	printf("Test\r\n");
	if(GPIOB->IDR & GPIO_IDR_IDR15) {
		while(1) {
			if(key_flag == 1 ) {
				//CAL_PLUSE_1();  	//进行校准模式测量
				CAL_INIT();
				//Delay(5000);
				ReadResult();
			}
		}
	} else  if(GPIOB->IDR & GPIO_IDR_IDR13) {
		while(1) {
			if(key_flag == 1 ) {
				NCAL_PLUSE_1();  	//进行非校准模式测量
			}
		}
	}
}
