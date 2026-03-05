#ifndef	_MS1050_H_
#define	_MS1050_H_

/***************** Common Register *****************/
	#define MS1050_RST_CMD		0x30
	//#define ADDR1		0x80
  #define Init		0x18
	
/***************----- MS1050 GPIO定义 -----***************/
#define MS1050_SCS		GPIO_Pin_4	//定义MS1050的CS引脚	 
#define MS1050_SCS_PORT	GPIOA
	
//#define MS1050_RST		GPIO_Pin_0	//定义MS1050的RST引脚
//#define MS1050_RST_PORT	GPIOB

#define MS1050_RST		GPIO_Pin_0	//定义MS1050的RST引脚
#define MS1050_RST_PORT	GPIOA

#define MS1050_INT		GPIO_Pin_1	//定义MS1050的INT引脚
#define MS1050_INT_PORT	GPIOB

#define MS1050_Pluse_STOP		GPIO_Pin_12	//定义MS1050的Pluse引脚
#define MS1050_Pluse_STOP_PORT	GPIOA

#define MS1050_Pluse_START		GPIO_Pin_11	//定义MS1050的Pluse引脚
#define MS1050_Pluse_START_PORT	GPIOA

#define MS1050_Disable		GPIO_Pin_15	//定义MS1050的Disable引脚
#define MS1050_Disable_PORT	GPIOA

#define MS1050_KEY		GPIO_Pin_2	//定义MS1050的按键脚
#define MS1050_KEY_PORT	GPIOA


#define MS1050_Pluse		GPIO_Pin_8	//定义MS1050的Pluse引脚
#define MS1050_Pluse_PORT	GPIOA

#define MS1050_Pluse2		GPIO_Pin_13	//定义MS1050的Pluse引脚
#define MS1050_Pluse_STOP2_PORT	GPIOB

#define MS1050_Pluse3		GPIO_Pin_14	//定义MS1050的Pluse引脚
#define MS1050_Pluse_STOP3_PORT	GPIOB

#define MS1050_Pluse4		GPIO_Pin_15	//定义MS1050的Pluse引脚
#define MS1050_Pluse_STOP4_PORT	GPIOB



extern unsigned char Intn_flag;	//MS1050中断标志(0:无中断,1:有中断)
extern unsigned char key_flag;	//MS1050中断标志(0:无中断,1:有中断)

extern void Delay(unsigned int d);//延时函数(ms)
extern void MS1050_SPI1_Send_Byte(unsigned char dat);
extern void Write_MS1050_1Byte(unsigned char dat);
extern void Write_MS1050_1Byte_Addr(unsigned char dat);
extern void MS1050_GPIO_Configuration(void);//MS1050 GPIO初始化配置
//extern void 1003_NVIC_Configuration(void);//MS1050 接收引脚中断优先级设置
extern void MS1050_SPI_Configuration(void);//MS1050 SPI初始化配置(STM32 SPI1)
extern void MS1050_Hardware_Reset(void);//硬件复位MS1050
extern void MS1050_Init(void);//初始化MS1050寄存器函数
extern void Load_reg_Parameters(void);
extern void Write_MS1050_4Byte(unsigned char *dat_ptr);
//extern unsigned char Read_MS1050_3Byte(void);
//extern void Read_MS1050_3Byte(void);
//extern void Read_MS1050_3Byte(unsigned char addr);
//extern unsigned long Read_MS1050_3Byte(unsigned char addr);
extern int Read_MS1050_1Byte();
extern float Read_MS1050_NCAL_3Byte(unsigned char addr);
extern void uart_send_3Byte(unsigned char dat_ptr[3]);
extern void MS1050_NVIC_Configuration(void);//MS1050 接收引脚中断优先级设置
extern float HEX_FLOAT(unsigned char data_0,unsigned char data_1,unsigned char data_2);
extern float HEX_NCAL_FLOAT(unsigned char data_0,unsigned char data_1,unsigned char data_2);
extern unsigned char Read_MS1050_PLUSE_3Byte(void);
#endif

