#include "stm32f10x.h"
#include "stm32f10x_spi.h"	
#include "stdio.h"
#include <string.h>
#include "1050.h"

unsigned char Reg[4];
unsigned int a,b,c;
unsigned char a1,b1,c1;
unsigned long buf,buf1;
float buf2;
unsigned char Intn_flag = 0,key_flag = 0;


/*******************************************************************************
* 函数名  : 1003_GPIO_Configuration
* 描述    : 1003 GPIO初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void MS1050_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
  EXTI_InitTypeDef  EXTI_InitStructure;	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 |RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);	
	
	/* 初始化RST引脚 */
	GPIO_InitStructure.GPIO_Pin  = MS1050_RST;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MS1050_RST_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(MS1050_RST_PORT, MS1050_RST);
	

	
		/* 初始化Pluse引脚 */
	GPIO_InitStructure.GPIO_Pin  = MS1050_Pluse_STOP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MS1050_Pluse_STOP_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(MS1050_Pluse_STOP_PORT, MS1050_Pluse_STOP);
	

	GPIO_InitStructure.GPIO_Pin  = MS1050_Pluse_START;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MS1050_Pluse_START_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(MS1050_Pluse_START_PORT, MS1050_Pluse_START);
	
	
	GPIO_InitStructure.GPIO_Pin  = MS1050_Pluse;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MS1050_Pluse_PORT, &GPIO_InitStructure);
	GPIO_SetBits(MS1050_Pluse_PORT, MS1050_Pluse);
	

	GPIO_InitStructure.GPIO_Pin = MS1050_Pluse2; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//浮空输出
	GPIO_Init(MS1050_Pluse_STOP2_PORT, &GPIO_InitStructure);
	GPIO_SetBits(MS1050_Pluse_STOP2_PORT,MS1050_Pluse2);


	GPIO_InitStructure.GPIO_Pin = MS1050_Pluse3; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//浮空输出
	GPIO_Init(MS1050_Pluse_STOP3_PORT, &GPIO_InitStructure);	
	GPIO_SetBits(MS1050_Pluse_STOP3_PORT,MS1050_Pluse3);
	
	GPIO_InitStructure.GPIO_Pin = MS1050_Pluse4; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//浮空输出
	GPIO_Init(MS1050_Pluse_STOP4_PORT, &GPIO_InitStructure);	
	GPIO_SetBits(MS1050_Pluse_STOP4_PORT,MS1050_Pluse4);
	
 /* 初始化Disable引脚 */
	GPIO_InitStructure.GPIO_Pin = MS1050_Disable; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//浮空输出
	GPIO_Init(MS1050_Disable_PORT, &GPIO_InitStructure);	
	GPIO_SetBits(MS1050_Disable_PORT,MS1050_Disable);

	/* 初始化INT引脚 */
	GPIO_InitStructure.GPIO_Pin = MS1050_INT; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(MS1050_INT_PORT, &GPIO_InitStructure);

/* 初始化KEY引脚 */
	GPIO_InitStructure.GPIO_Pin = MS1050_KEY; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(MS1050_KEY_PORT, &GPIO_InitStructure);


/* Connect EXTI Line4 to PC4 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);


	/* PC4 as MS1050 interrupt input */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

/*******************************************************************************
* 函数名  : MS1050_NVIC_Configuration
* 描述    : MS1050 接收引脚中断优先级设置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void MS1050_NVIC_Configuration(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the EXTI4 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/*******************************************************************************
* 函数名  : SPI_Configuration
* 描述    : MS1050 SPI初始化配置(STM32 SPI1)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void MS1050_SPI_Configuration(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef   	SPI_InitStructure;	   

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);	

	/* 初始化SCK、MISO、MOSI引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	

	/* 初始化CS引脚 */
	GPIO_InitStructure.GPIO_Pin = MS1050_SCS;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(MS1050_SCS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(MS1050_SCS_PORT, MS1050_SCS);

	/* 初始化配置STM32 SPI1 */
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;	//SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;							//设置为主SPI
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;						//SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;							//时钟悬空低
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;							//数据捕获于第2个时钟沿
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;								//NSS由外部管脚管理
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;	//波特率预分频值为4
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;					//数据传输从MSB位开始
//	SPI_InitStructure.SPI_CRCPolynomial=7;								//CRC多项式为7
	SPI_Init(SPI1,&SPI_InitStructure);									//根据SPI_InitStruct中指定的参数初始化外设SPI1寄存器

	SPI_Cmd(SPI1,ENABLE);	//STM32使能SPI1
}

/*******************************************************************************
* 函数名  : MS1050_Initialization
* 描述    : MS1050复位及初始化寄存器配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void MS1050_Initialization(void)
{
  MS1050_Hardware_Reset(); //MS1050硬件复位
	Write_MS1050_1Byte(MS1050_RST_CMD); //MS1050软件复位
}
/*******************************************************************************
* 函数名  : MS1050_Hardware_Reset
* 描述    : 硬件复位MS1050
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void MS1050_Hardware_Reset(void)
{
	GPIO_ResetBits(MS1050_RST_PORT, MS1050_RST);//复位引脚拉低
//	Delay(1);
	GPIO_SetBits(MS1050_RST_PORT, MS1050_RST);//复位引脚拉高
	Delay(1);
	GPIO_ResetBits(MS1050_RST_PORT, MS1050_RST);//复位引脚拉低

}
/*******************************************************************************
* 函数名  : Write_MS1050_1Byte
* 描述    : 通过SPI1写1个字节数据
* 输入    : dat:待写入的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_MS1050_1Byte(unsigned char dat)
{
	GPIO_ResetBits(MS1050_SCS_PORT, MS1050_SCS);//置MS1050的SCS为低电平
  MS1050_SPI1_Send_Byte(dat);//写1个字节数据
  GPIO_SetBits(MS1050_SCS_PORT, MS1050_SCS); //置MS1050的SCS为高电平
}
/*******************************************************************************
* 函数名  : Write_MS1050_1Byte_Addr
* 描述    : 通过SPI1写1个字节地址数据，使能配合写4字节寄存器
* 输入    : dat:待写入的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_MS1050_1Byte_Addr(unsigned char dat)
{
	GPIO_ResetBits(MS1050_SCS_PORT, MS1050_SCS);//置MS1050的SCS为低电平
  MS1050_SPI1_Send_Byte(dat);//写1个字节数据
  //GPIO_SetBits(MS1050_SCS_PORT, MS1050_SCS); //置MS1050的SCS为高电平
}



/*******************************************************************************
* 函数名  : SPI1_Send_Byte
* 描述    : SPI1发送1个字节数据
* 输入    : dat:待发送的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void MS1050_SPI1_Send_Byte(unsigned char dat)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//等待数据寄存器空
	SPI_I2S_SendData(SPI1,dat);//写1个字节数据
	while((SPI1->SR & SPI_I2S_FLAG_BSY));
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);	
	SPI_I2S_ReceiveData(SPI1);
//	a=SPI1->DR;
}



/*******************************************************************************
* 函数名  : MS1050_START
* 描述    : 发送START脉冲信号 
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
	void MS1050_START(void)
{
	unsigned int delay = 1;
	
	MS1050_Pluse_START_PORT->BSRR = MS1050_Pluse_START;
//						while(delay != 0)
//						{delay--;}
//						delay=1;	
	MS1050_Pluse_START_PORT->BRR = MS1050_Pluse_START;
	//MS1050_Pluse_PORT->BRR = MS1050_Pluse;
	//MS1050_Pluse_PORT->BSRR = MS1050_Pluse;
}
/*******************************************************************************
* 函数名  : MS1050_STOP
* 描述    : 发送STOP脉冲信号 
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/

void MS1050_STOP(void)
{
	MS1050_Pluse_STOP_PORT->BSRR = MS1050_Pluse_STOP;
	MS1050_Pluse_STOP_PORT->BRR = MS1050_Pluse_STOP;
}

void MS1050_STOP1(void)
{
	MS1050_Pluse_PORT->BSRR = MS1050_Pluse;
	MS1050_Pluse_PORT->BRR = MS1050_Pluse;
}
void MS1050_STOP2(void)
{
	MS1050_Pluse_STOP2_PORT->BSRR = MS1050_Pluse2;
	MS1050_Pluse_STOP2_PORT->BRR = MS1050_Pluse2;
}
void MS1050_STOP3(void)
{
	MS1050_Pluse_STOP3_PORT->BSRR = MS1050_Pluse3;
	MS1050_Pluse_STOP3_PORT->BRR = MS1050_Pluse3;
}

void MS1050_STOP4(void)
{
	MS1050_Pluse_STOP4_PORT->BSRR = MS1050_Pluse4;
	MS1050_Pluse_STOP4_PORT->BRR = MS1050_Pluse4;
}

/*******************************************************************************
* 函数名  : HEX_FLOAT
* 描述    : 校准模式输出16进制数据转换浮点数
* 输入    : 16进制数
* 输出    : 浮点数
* 返回值  : 浮点数
* 说明    : 无
*******************************************************************************/
float HEX_FLOAT(unsigned char data_0,unsigned char data_1,unsigned char data_2)
{
	float REG_TEMP=0;
	float REG_TEMP0=0;
	float REG_TEMP1=0;
	float DEC_TEMP=0;
	unsigned char i;
	unsigned long t=0x00010000;
	DEC_TEMP=data_0;
	for(i=8;i>0;i--)
	{
		REG_TEMP0=data_2&0x01; 
		REG_TEMP1=REG_TEMP0/t; 
		REG_TEMP=REG_TEMP+REG_TEMP1;
		data_2>>=1;
		t>>=1;	
	}
  for(i=8;i>0;i--)
	{
		REG_TEMP0=data_1&0x01; 
		REG_TEMP1=REG_TEMP0/t; 
		REG_TEMP=REG_TEMP+REG_TEMP1;
		data_1>>=1;
		t>>=1;	
	}        
        REG_TEMP=DEC_TEMP+REG_TEMP;      
	return REG_TEMP;
}
/*******************************************************************************
* 函数名  : HEX_NCAL_FLOAT
* 描述    : 非校准模式输出16进制数据转换浮点数
* 输入    : 16进制数
* 输出    : 浮点数
* 返回值  : 浮点数
* 说明    : 无
*******************************************************************************/
float HEX_NCAL_FLOAT(unsigned char data_0,unsigned char data_1,unsigned char data_2)
{
	float REG_TEMP=0;
//	float REG_TEMP0=0;
//	float REG_TEMP1=0;
	float DEC_TEMP=0;
//	unsigned char i;
//	unsigned long t=0x00010000;
	DEC_TEMP=data_0*256*256+data_1*256+data_2;       
  REG_TEMP=DEC_TEMP;      
	return REG_TEMP;
}

/*******************************************************************************
* 函数名  : Read_MS1050_1Byte
* 描述    : 读SPI数据
* 输入    : 无
* 输出    : 寄存器值
* 返回值  : HEX
* 说明    : 无
*******************************************************************************/
int Read_MS1050_1Byte(void)
{

//	Write_MS1050_1Byte_Addr(0x00);//发送一个哑数据
	GPIO_ResetBits(MS1050_SCS_PORT, MS1050_SCS);	//置MS1050的SCS为低电平
//  MS1050_SPI1_Send_Byte(0x00);									//写1个字节数据
	
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//等待数据寄存器空
	SPI_I2S_SendData(SPI1,0x00);//写1个字节数据
	while((SPI1->SR & SPI_I2S_FLAG_BSY));
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);	
	a = SPI_I2S_ReceiveData(SPI1);
//	a=SPI1->DR;

	return a;
}

/*******************************************************************************
* 函数名  : Read_MS1050_NCAL_3Byte
* 描述    : 读SPI数据
* 输入    : 地址
* 输出    : 非校准模式输出
* 返回值  : 时间值浮点数
* 说明    : 无
*******************************************************************************/


float Read_MS1050_NCAL_3Byte(unsigned char addr)
{
	Write_MS1050_1Byte_Addr(addr);
	a=SPI1->DR;
	MS1050_SPI1_Send_Byte(0x00);//发送一个哑数据
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);	
	a=SPI1->DR;
	//a=SPI_I2S_ReceiveData(SPI1);
//	a=0x55;
	MS1050_SPI1_Send_Byte(0x00);//发送一个哑数据
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET){};
		b=SPI1->DR;
//	b=SPI_I2S_ReceiveData(SPI1);
//	b=0x55;
	MS1050_SPI1_Send_Byte(0x00);//发送一个哑数据
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);	
	c= SPI1->DR;
	//c=SPI_I2S_ReceiveData(SPI1);
//	c=0x55;
	GPIO_SetBits(MS1050_SCS_PORT, MS1050_SCS);//置MS1050的SCS为高电平
/*	USART_SendData(USART1,a);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	Delay(5);
  USART_SendData(USART1,b);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	 Delay(5);
	USART_SendData(USART1,c);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	 Delay(5);*/
  buf2 = HEX_NCAL_FLOAT(a,b,c);
	a = 0x00;
	b = 0x00;
	c = 0x00;
	return buf2;
}

/*******************************************************************************
* 函数名  : Read_MS1050_PLUSE_3Byte
* 描述    : 读取脉冲个数
* 输入    : 无
* 输出    : 打印脉冲个数
* 返回值  : 对应寄存器地址
* 说明    : 无
*******************************************************************************/
unsigned char Read_MS1050_PLUSE_3Byte(void)
{
	unsigned char i;
	Write_MS1050_1Byte_Addr(0xbd);
	a=SPI_I2S_ReceiveData(SPI1);
	MS1050_SPI1_Send_Byte(0x00);//发送一个哑数据
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);	
	a=(SPI1->DR);
	MS1050_SPI1_Send_Byte(0x00);//发送一个哑数据
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);	
	b=(SPI1->DR);
	MS1050_SPI1_Send_Byte(0x00);//发送一个哑数据
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);	
	c= (SPI1->DR) ;
	//buf = ((a << 16)|(b<<8)|c)>>5;
	//c = buf;
	c= c & 0x0f;
	GPIO_SetBits(MS1050_SCS_PORT, MS1050_SCS);//置MS1050的SCS为高电平
	/*	USART_SendData(USART1,a);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
//	Delay(500);
  USART_SendData(USART1,b);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	// Delay(500);
	USART_SendData(USART1,c);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   */
	buf2 = HEX_NCAL_FLOAT(0x00,0x00,c);
	printf(" Pluse:");
  printf("%.0lf",buf2);
	printf(" ");
	// while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
	 if(c==0x03)
	 {
	 return i=0xb1;
	 }
	 else if(c == 0x02)
	 {
	 return i=0xb0;
	 }
	 else if(c == 0x04)
	 {
	 return i=0xb2;
	 }
	 else if(c == 0x05)
	 {
	 return i=0xb3;
	 }
	 else if(c == 0x06)
	 {
	 return i=0xb4;
	 }
	 else if(c == 0x07)
	 {
	 return i=0xb5;
	 }
	 else if(c == 0x08)
	 {
	 return i=0xb6;
	 }
	 else if(c == 0x09)
	 {
	 return i=0xb7;
	 }
	 else if(c == 0x0a)
	 {
	 return i=0xb8;
	 }
	 else if(c == 0x0b)
	 {
	 return i=0xb9;
	 }
}
/*******************************************************************************
* 函数名  : EXTI1_IRQHandler
* 描述    : 中断线1中断服务函数(MS1050 INT引脚中断服务函数)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
unsigned char EXTI1_IRQHandler(void)
{
/*	if(PAin(1)==0)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		Intn_flag=0;
		return Intn_flag;
	}*/
 	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		Intn_flag=1;
		return Intn_flag;
	}
}
/*******************************************************************************
* 函数名  : EXTI2_IRQHandler
* 描述    : 中断线2中断服务函数(KEY引脚中断服务函数)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
/*	if(PAin(1)==0)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		Intn_flag=0;
		return Intn_flag;
	}*/
 	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
		key_flag=1;
//		return Intn_flag;
	}	
}


