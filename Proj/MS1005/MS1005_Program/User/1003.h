#ifndef	_MS1003_H_
#define	_MS1003_H_

/***************** Common Register *****************/
#define MS1003_RST_CMD		0x50
#define ADDR1				0x80
#define Init				0x70

/***************----- MS1003 GPIO定义 -----***************/
#define MS1003_SCS				GPIO_Pin_4			//定义MS1003的CS引脚	 
#define MS1003_SCS_PORT			GPIOA

#define MS1003_RST				GPIO_Pin_0			//定义MS1003的RST引脚
#define MS1003_RST_PORT			GPIOB

#define MS1003_INT				GPIO_Pin_1			//定义MS1003的INT引脚
#define MS1003_INT_PORT			GPIOB

#define MS1003_Pluse_STOP		GPIO_Pin_12			//定义MS1003的Pluse引脚
#define MS1003_Pluse_STOP_PORT	GPIOA

#define MS1003_Pluse_START		GPIO_Pin_11			//定义MS1003的Pluse引脚
#define MS1003_Pluse_START_PORT	GPIOA

#define MS1003_KEY				GPIO_Pin_2			//定义MS1003的按键脚
#define MS1003_KEY_PORT			GPIOA

#define MS1003_Pluse			GPIO_Pin_8			//定义MS1003的Pluse引脚
#define MS1003_Pluse_PORT		GPIOA

#define MS1003_CAL				GPIO_Pin_15			//定义MS1003的CAL引脚
#define MS1003_CAL_PORT			GPIOB

#define MS1003_NCAL				GPIO_Pin_13			//定义MS1003的NCAL引脚
#define MS1003_NCAL_PORT		GPIOB

extern unsigned char Intn_flag;	//MS1003中断标志(0:无中断,1:有中断)
extern unsigned char key_flag;	//MS1003中断标志(0:无中断,1:有中断)

extern void Delay(unsigned int d);//延时函数(ms)
extern void MS1003_SPI1_Send_Byte(unsigned char dat);
extern void Write_MS1003_1Byte(unsigned char dat);
extern void Write_MS1003_1Byte_Addr(unsigned char dat);
extern void MS1003_GPIO_Configuration(void);//MS1003 GPIO初始化配置
//extern void 1003_NVIC_Configuration(void);//MS1003 接收引脚中断优先级设置
extern void MS1003_SPI_Configuration(void);//MS1003 SPI初始化配置(STM32 SPI1)
extern void MS1003_Hardware_Reset(void);//硬件复位MS1003
extern void Load_reg_Parameters(void);
extern void Write_MS1003_4Byte(unsigned char *dat_ptr);
//extern unsigned char Read_MS1003_3Byte(void);
//extern void Read_MS1003_3Byte(void);
//extern void Read_MS1003_3Byte(unsigned char addr);
//extern unsigned long Read_MS1003_3Byte(unsigned char addr);
extern float Read_MS1003_CAL_3Byte(unsigned char addr);
extern float Read_MS1003_NCAL_3Byte(unsigned char addr);
extern void uart_send_3Byte(unsigned char dat_ptr[3]);
extern void MS1003_NVIC_Configuration(void);//MS1003 接收引脚中断优先级设置
extern float HEX_FLOAT(unsigned char data_0, unsigned char data_1, unsigned char data_2);
extern float HEX_NCAL_FLOAT(unsigned char data_0, unsigned char data_1, unsigned char data_2);
extern unsigned char Read_MS1003_PLUSE_3Byte(void);
#endif
