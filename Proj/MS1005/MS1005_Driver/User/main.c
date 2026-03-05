#include "stm32f10x.h"
#include "core_cm3.h" // 提供CoreDebug等CMSIS定义（本工程CMSIS较旧）
#include "stdio.h"
#include <string.h>
#include "ms1005_driver.h"

// 兼容旧版CMSIS: 若未提供DWT定义，则补充最小DWT寄存器映射
#ifndef DWT_BASE
typedef struct
{
	__IO uint32_t CTRL;
	__IO uint32_t CYCCNT;
} DWT_Type;
#define DWT_BASE (0xE0001000UL)
#define DWT ((DWT_Type *)DWT_BASE)
#endif
#ifndef DWT_CTRL_CYCCNTENA_Msk
#define DWT_CTRL_CYCCNTENA_Msk (1UL << 0)
#endif

void RCC_Configuration(void);	  // 设置系统时钟为72MHZ(这个可以根据需要改)
void NVIC_Configuration(void);	  // STM32中断向量表配配置
void Timer2_Init_Config(void);	  // Timer2初始化配置
void System_Initialization(void); // STM32系统初始化函数(初始化STM32时钟及外设)
void Delay(unsigned int d);		  // 延时函数(ms)
void UART1_Init(void);			  // STM32串口初始化
void DWT_Init(void);			  // 使能DWT周期计数


unsigned int Timer2_Counter = 0; // Timer2定时器计数变量(ms)
// unsigned int MS1003_Send_Delay_Counter = 0;
unsigned char reg;
unsigned char USART1_RX_flag;

#define CPU_CYCLES_PER_US 72U
#define MODE_TIMEOUT_CYCLES (CPU_CYCLES_PER_US * 3000U * 1000U) // 3秒超时
#define TDC_INT_TIMEOUT_CYCLES (CPU_CYCLES_PER_US * 1000U)		// INTN等待超时：1ms超时

#define UART_RX_BUF_LEN 64
// 串口接收环形缓冲，ISR写入、主循环读取
static volatile uint8_t uart_rx_buf[UART_RX_BUF_LEN];
static volatile uint8_t uart_rx_head = 0;
static volatile uint8_t uart_rx_tail = 0;
static volatile uint8_t uart_rx_overflow = 0;

static void uart_rx_push(uint8_t b)
{
	uint8_t next = (uart_rx_head + 1U) % UART_RX_BUF_LEN;
	if (next != uart_rx_tail)
	{
		uart_rx_buf[uart_rx_head] = b;
		uart_rx_head = next;
	}
	else
	{
		uart_rx_overflow = 1U; // 简单溢出标记，最新字节被丢弃
	}
}

static int uart_rx_pop(uint8_t *b)
{
	if (uart_rx_head == uart_rx_tail)
	{
		return 0;
	}
	*b = uart_rx_buf[uart_rx_tail];
	uart_rx_tail = (uart_rx_tail + 1U) % UART_RX_BUF_LEN;
	return 1;
}

#if 1
#pragma import(__use_no_semihosting) // 标准库需要的支持函数
struct __FILE
{
	int handle;
};
FILE __stdout;					 // 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x) { x = x; } // 重定义fputc函数
int fputc(int ch, FILE *f)
{
	while (!(USART1->SR & (1 << 7)))
		;
	USART_SendData(USART1, (uint8_t)ch);
	return ch;
}

#endif

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
	RCC_Configuration();  // 设置系统时钟为72MHZ(这个可以根据需要改)
	DWT_Init();			  // 启用DWT周期计数
	NVIC_Configuration(); // STM32中断向量表配配置
	Timer2_Init_Config(); // Timer2初始化配置
}

/*******************************************************************************
	DWT_Init();                 // 启用DWT周期计数，用于微秒级超时
 * 函数名  : RCC_Configuration
	Timer2_Init_Config();		 // Timer2初始化配置
 * 描述    : 设置系统时钟为72MHZ(这个可以根据需要改)
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : STM32F107x和STM32F105x系列MCU与STM32F103x系列MCU时钟配置有所不同
 *******************************************************************************/
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus; // 外部高速时钟(HSE)的工作状态变量

	RCC_DeInit();								// 将所有与时钟相关的寄存器设置为默认值
	RCC_HSEConfig(RCC_HSE_ON);					// 启动外部高速时钟HSE
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); // 等待外部高速时钟(HSE)稳定

	if (SUCCESS == HSEStartUpStatus) // 如果外部高速时钟已经稳定
	{
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // Flash设置
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);

		RCC_HCLKConfig(RCC_SYSCLK_Div1); // 设置AHB时钟等于系统时钟(1分频)/72MHZ
		RCC_PCLK2Config(RCC_HCLK_Div1);	 // 设置APB2时钟和HCLK时钟相等/72MHz(最大为72MHz)
		RCC_PCLK1Config(RCC_HCLK_Div2);	 // 设置APB1时钟是HCLK时钟的2分频/36MHz(最大为36MHz)

		// #ifndef STM32F10X_CL                 //如果使用的不是STM32F107x或STM32F105x系列MCU,PLL以下配置
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // PLLCLK = 8MHz * 9 = 72 MHz
		// #else                                //如果使用的是STM32F107x或STM32F105x系列MCU,PLL以下配置
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

		RCC_PLLCmd(ENABLE); // 使能PLL
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			; // 等待PLL稳定

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // 设置系统时钟的时钟源为PLL

		while (RCC_GetSYSCLKSource() != 0x08)
			;								// 检查系统的时钟源是否是PLL
		RCC_ClockSecuritySystemCmd(ENABLE); // 使能系统安全时钟

		/* Enable peripheral clocks --------------------------------------------------*/
		/* Enable I2C1 and I2C1 clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

		/* Enable GPIOA GPIOB SPI1 and USART1 clocks */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
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
	NVIC_InitTypeDef NVIC_InitStructure; // 定义NVIC初始化结构体

	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 设置中断优先级组为1，优先组(可设0～4位)

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;			  // 设置中断向量号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 设置抢先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能NVIC
	NVIC_Init(&NVIC_InitStructure);
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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 使能Timer2时钟

	TIM_TimeBaseStructure.TIM_Period = 9;						// 设置在下一个更新事件装入活动的自动重装载寄存器周期的值(计数到10为1ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;					// 设置用来作为TIMx时钟频率除数的预分频值(10KHz的计数频率)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 设置时钟分割:TDTS = TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				// 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 使能TIM2指定的中断

	TIM_Cmd(TIM2, ENABLE); // 使能TIMx外设
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
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		Timer2_Counter++;
		// MS1003_Send_Delay_Counter++;
	}
}

// 使能DWT计数，用于微秒级超时判断
void DWT_Init(void)
{
	if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0)
	{
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	}
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
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
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // UART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //
	// UART1_TX   GPIOA_Pin_9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // GPIOA_Pin_9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);			// 配置TX口
	// UART1_RX      GPIOA_Pin_10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // GPIOA_Pin_10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // 配置RX口
	// UART1通讯参数配置
	USART_InitStructure.USART_BaudRate = 115200;				// 通讯波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					//
	USART_Init(USART1, &USART_InitStructure);
	// Uart1 NVIC  中断配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// 数据接收中断使能
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
	while (Timer2_Counter < d)
		;
}

void USART1_IRQHandler(void) // 串口1中断服务程序
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		uint8_t data = USART_ReceiveData(USART1);
		uart_rx_push(data); // 把接收字节塞入环形缓冲，主循环统一解析
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

	// 溢出: 先读SR再读DR清除ORE，否则会持续进中断
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET)
	{
		USART_ReceiveData(USART1);
		USART_ClearFlag(USART1, USART_FLAG_ORE);
	}
}

/**
 * @brief 阻塞等待中断信号INTN
 *
 * @param delay 阻塞时间，单位CPU周期
 * @return unsigned char
 */
unsigned char INTN_fig(int delay)
{
	while (Intn_flag == 1)
	{
		delay--;
		if (!delay)
		{
			return 1;
		}
		// Delay(1);
	}
	return 0;
}

#define BUF_LENGTH 256
static float s_sample_buf[BUF_LENGTH];

/**
 * @brief start通道输入参考脉冲（在发射脉冲前），stop1通道输入发射脉冲，stop2通道输入反射脉冲
 *
 * @note TOF = t(stop2) - t(stop1)，单精度
 * @param sample_num 采样次数
 */
void CAL_Test(int sample_num)
{
	float i1, i2, j1, j2, k1, k2;

	/* MS1005写入寄存器操作码 */
	TDC_CalModeCfg();
	/* Intn_flag: 中断标志位，TDC的中断触发时被置0，主程序处理后置1 */
	Intn_flag = 1;
	int counter = sample_num;
	while (counter--)
	{
		Intn_flag = 1;

		TDC_MeasureStart(); // 启动测量
		/* 等待TDC中断触发，使用DWT计数实现超时判断1us */
		uint32_t intn_start = DWT->CYCCNT;
		while (Intn_flag != 0)
		{
			uint32_t now = DWT->CYCCNT;
			if ((now - intn_start) >= TDC_INT_TIMEOUT_CYCLES)
			{
				printf("INTN timeout. Skip this sample.\r\n");
				break; // 跳出等待循环，跳过本次样本
			}
		}
		if (Intn_flag == 0)
		{
			/*
			飞行时间
			op：0xbx，读取结果寄存器；x=0~9，表示读取第0~9个结果寄存器
			校准数据格式：24位，高8位整数，低16位小数
			*/
			i1 = TDC_ReadCalResult(0xb0) * 1000; // stop1结果0 转换为纳秒
			i2 = TDC_ReadCalResult(0xb1) * 1000; // stop2结果0 转换为纳秒

			// 脉冲宽度
			j1 = TDC_ReadCalResult(0xb0) * 1000; // stop1结果0 转换为纳秒
			j2 = TDC_ReadCalResult(0xc0) * 1000; // stop2结果0 转换为纳秒

			// 脉冲宽度
			k1 = TDC_ReadCalResult(0xb1) * 1000; // stop1结果0 转换为纳秒
			k2 = TDC_ReadCalResult(0xc1) * 1000; // stop2结果0 转换为纳秒

			// 读状态寄存器，检查错误状态
			uint32_t i = TDC_ReadState();
			// printf("Debug state value: %d\r\n", i);
			if ((i & 0x6000) > 0)
			{
				printf("Error state:");
				printf("%d", i);
				printf("\r\n");
				return;
			}
			else
			{
				// 飞行时间 & 脉宽
				printf("Sample %d -> B0:%.3f     B1:%.3f    TOF:%.3f \r\n", sample_num - counter - 1, i1, i2, i2 - i1);
				printf("PluseW %d -> PW0:%.3f    PW1:%.3f\r\n", sample_num - counter - 1, j2 - j1, k2 - k1);
				s_sample_buf[sample_num - counter - 1] = i2 - i1;
			}
		}
	}

	if (sample_num > 0 && counter <= 0)
	{
		printf("%u times Samples success:\r\n", sample_num - counter);
		printf("Averaged TOF (ns):\r\n");
		float avg = 0.0f;
		for (int idx = 1; idx < (sample_num - counter - 1); idx++)
		{
			printf("Sample %d: %.3f ns\r\n", idx, s_sample_buf[idx]);
			avg += s_sample_buf[idx];
		}
		avg /= (sample_num - counter - 2);
		printf("%.3f\r\n", avg);
	}
}

int main(void)
{
	System_Initialization(); // STM32系统初始化函数(初始化STM32时钟及外设)
	UART1_Init();			 // STM32串口初始
	TDC_Init();

	printf("Send mode (1-3), then sample count (0-65535), finish with Enter.\r\n"); // 先选模式，再输入16位采样数

	uint8_t mode_char = 0;		  // 当前选择的测量方式字符('1'/'2'/'3')
	uint8_t sample_digits = 0;	  // 已输入的采样点数字符数
	uint32_t sample_acc = 0;	  // 累积的采样点数，先用32位避免溢出
	uint32_t mode_wait_start = 0; // 记录开始等待采样数的DWT计数

	while (1)
	{
		uint8_t ch;
		if (!uart_rx_pop(&ch))
		{
			// 没有新数据时检查是否等待采样数超时
			if (mode_char != 0)
			{
				uint32_t now = DWT->CYCCNT;
				if ((now - mode_wait_start) >= MODE_TIMEOUT_CYCLES)
				{
					printf("Timeout waiting sample count. Send mode again (1-3).\r\n");
					mode_char = 0;
					sample_digits = 0;
					sample_acc = 0;
					mode_wait_start = 0;
				}
			}
			continue; // 没有新数据，继续轮询
		}

		if (mode_char == 0)
		{
			// 等待模式数字: 1/2/3/4
			if (ch >= '1' && ch <= '4')
			{
				mode_char = ch;
				sample_digits = 0;
				sample_acc = 0;
				mode_wait_start = DWT->CYCCNT; // 开始计时等待采样数
				printf("Mode %c selected. Enter sample count (0-65535) then Enter:\r\n", ch);
			}
			else if (ch != '\r' && ch != '\n')
			{
				printf("Invalid mode '%c'. Send 1/2/3/4.\r\n", ch);
			}
			continue;
		}

		// 已收到模式，开始读取采样点数字符
		if (ch == '\r' || ch == '\n')
		{
			if (sample_digits == 0)
			{
				printf("Sample count is empty. Enter again:\r\n");
				continue;
			}

			if (sample_acc > 65535U)
			{
				printf("Sample count exceeds 65535. Enter again:\r\n");
				sample_digits = 0;
				sample_acc = 0;
				continue;
			}

			uint16_t sample_num = (uint16_t)sample_acc;
			printf("Start measure: mode %c, samples %u\r\n", mode_char, sample_num);

			switch (mode_char)
			{
			case '1':
				CAL_Test(sample_num);
				break;
			case '2':

				break;
			case '3':

				break;
			case '4':

				break;
			default:
				break;
			}

			// 重置以等待下一轮输入
			mode_char = 0;
			sample_digits = 0;
			sample_acc = 0;
			mode_wait_start = 0;
			printf("Done. Send next mode (1-4):\r\n");
			continue;
		}

		if (ch >= '0' && ch <= '9')
		{
			if (sample_digits < 5)
			{
				sample_acc = sample_acc * 10U + (uint32_t)(ch - '0');
				sample_digits++;
			}
			else
			{
				printf("Sample count too long (max 5 digits). Re-enter count:\r\n");
				sample_digits = 0;
				sample_acc = 0;
			}
		}
		else
		{
			printf("Unexpected char '%c'. Enter digits or Enter.\r\n", ch);
		}
	}
}
