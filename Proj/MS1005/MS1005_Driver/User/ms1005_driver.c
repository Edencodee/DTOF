#include "ms1005_driver.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stdio.h"
#include "stdint.h"
#include <string.h>

#define TDC_TEST_MODE 0
#define DELAY_TIME 10

/**
 * @brief 0x33 0x3a 0x64 0x20 校准模式1脉冲配置命令
 * @notice: 配置详情参考规格书
 * 			0x33   STOP1 STOP2通道使能，两脉冲输出
 * 			0x3a   双通道单精度
 * 			0x64   STOP1上升沿有效 STOP2下降沿有效，自动产生校准值
 * 			0x20   Endhits 中断触发
 */
#define TDC_CFG_CMD 0x333a6420

unsigned char Intn_flag;

/* ================================ 硬件抽象层 ================================ */

/**
 * @brief MS1005 GPIO初始化配置
 * @param none
 * @return none
 * @notice:
 */
void TDC_GpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* TDC_NRST configure */
	RCC_APB2PeriphClockCmd(TDC_RSTN_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_RSTN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TDC_RSTN_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(TDC_RSTN_PORT, TDC_RSTN_PIN);

	/* TDC_INTN configure */
	RCC_APB2PeriphClockCmd(TDC_INTN_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_INTN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TDC_INTN_PORT, &GPIO_InitStructure);

	/* TDC_CAL configure */
	RCC_APB2PeriphClockCmd(TDC_CAL_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_CAL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TDC_CAL_PORT, &GPIO_InitStructure);
	GPIO_SetBits(TDC_CAL_PORT, TDC_CAL_PIN);

	/* TDC_NCAL configure */
	RCC_APB2PeriphClockCmd(TDC_NCAL_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_NCAL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TDC_NCAL_PORT, &GPIO_InitStructure);
	GPIO_SetBits(TDC_NCAL_PORT, TDC_NCAL_PIN);

#if TDC_TEST_MODE
	/* TDC_Start configure */
	RCC_APB2PeriphClockCmd(TDC_START_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_START_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TDC_START_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(TDC_START_PORT, TDC_START_PIN);

	RCC_APB2PeriphClockCmd(TDC_STOP1_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_STOP1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TDC_STOP1_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(TDC_STOP1_PORT, TDC_STOP1_PIN);

	RCC_APB2PeriphClockCmd(TDC_STOP2_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_STOP2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TDC_STOP2_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(TDC_STOP2_PORT, TDC_STOP2_PIN);

#endif

	/* Connect EXTI Line4 to PC4 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	/* PC4 as MS1005 interrupt input */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/**
 * @brief MS1005 INIT外部中断配置
 * @param none
 * @return none
 * @notice:
 */
void MS1005_NVIC_Configuration(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable INTN EXTI interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief MS1005 INTN中断服务程序
 * @param none
 * @return none
 * @notice:
 */
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		Intn_flag = 0;
	}
}

/* ================================ 通信层 ================================ */

/**
 * @brief MS1005 SPI 配置
 * @param none
 * @return none
 * @notice: 新增SPI配置
 */
void MS1005_SPI_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);

	/* 初始化SCK、MISO、MOSI引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

	/* 初始化CS引脚 */
	RCC_APB2PeriphClockCmd(TDC_SSN_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TDC_SSN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TDC_SSN_PORT, &GPIO_InitStructure);
	GPIO_SetBits(TDC_SSN_PORT, TDC_SSN_PIN);

	/* 初始化配置STM32 SPI1 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					   // 设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				   // SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   // 时钟悬空低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;					   // 数据捕获于第2个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						   // NSS由外部管脚管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 波特率预分频值为4
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   // 数据传输从MSB位开始
	//	SPI_InitStructure.SPI_CRCPolynomial=7;								//CRC多项式为7
	SPI_Init(SPI1, &SPI_InitStructure); // 根据SPI_InitStruct中指定的参数初始化外设SPI1寄存器

	SPI_Cmd(SPI1, ENABLE); // STM32使能SPI1
}

/**
 * @brief MS1005 SPI 片选低电平 使能传输
 * @param none
 * @return none
 * @notice: MS1005 SPI 片选低电平 使能传输
 */
static void SPI_NssLow(void)
{
	for(int i = 0; i < 2 * DELAY_TIME; i++);
	GPIO_ResetBits(TDC_SSN_PORT, TDC_SSN_PIN);
	for(int i = 0; i < DELAY_TIME; i++);
}

/**
 * @brief MS1005 SPI 片选高电平 停止传输
 * @param none
 * @return none
 * @notice: MS1005 SPI 片选高电平 停止传输
 */
static void SPI_NssHigh(void)
{
	for(int i = 0; i < DELAY_TIME; i++);
	GPIO_SetBits(TDC_SSN_PORT, TDC_SSN_PIN);
	for(int i = 0; i < 2 * DELAY_TIME; i++);
}

/**
 * @brief MS1005 SPI SPI传输，同时接收响应数据，一发一收
 * @param uint8_t val 传输数据
 * @return uint8_t val 响应数据
 * @notice: MS1005 SPI 片选高电平 使能传输
 */
static uint8_t SPI_Transmit(uint8_t val)
{
	/* transmit */
	SPI_I2S_SendData(SPI1, val);
	while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
		;
	/* receive */
	while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
		;
	return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

/**
 * @brief TDC_WriteByte 写一个字节
 * @param uint8_t val 写入数据
 * @return none
 * @notice:
 */
static void TDC_WriteByte(uint8_t val)
{
	SPI_NssLow();
	SPI_Transmit(val);
	SPI_NssHigh();
}

/**
 * @brief TDC_WriteReg  写寄存器 32位
 * @param uint8_t op 操作码
 * @param uint32_t val 写入数据
 * @return none
 * @notice:
 */
void TDC_WriteReg(uint8_t op, uint32_t val)
{
	u32_t uval32;
	uval32.word = val;

	SPI_NssLow();
	SPI_Transmit(op);
	SPI_Transmit(uval32.buf[3]);
	SPI_Transmit(uval32.buf[2]);
	SPI_Transmit(uval32.buf[1]);
	SPI_Transmit(uval32.buf[0]);
	SPI_NssHigh();
}

/**
 * @brief TDC_ReadByte 读一个字节
 * @param uint8_t op 操作码
 * @return uint8_t val 读出数据
 * @notice:
 */
uint8_t TDC_ReadByte(uint8_t op)
{
	SPI_NssLow();
	SPI_Transmit(op);
	for(int i = 0; i < DELAY_TIME; i++);
	uint8_t val = SPI_Transmit(0x00);
	SPI_NssHigh();
	return val;
}

/**
 * @brief TDC_Read3Bytes 读三个字节
 * @param uint8_t op 操作码
 * @return uint32_t val 读出数据
 * @notice: SPI传输为MSB-first模式 ARM Cortex-M3为LSB-first模式 因此返回值为LSB
 */
uint32_t TDC_Read3Bytes(uint8_t op)
{
	uint8_t high_byte, mid_byte, low_byte;
	SPI_NssLow();
	SPI_Transmit(op);
	for(int i = 0; i < DELAY_TIME; i++);
	high_byte = SPI_Transmit(0x00);
	mid_byte = SPI_Transmit(0x00);
	low_byte = SPI_Transmit(0x00);
	SPI_NssHigh();
	return ((uint32_t)high_byte << 16) | ((uint32_t)mid_byte << 8) | low_byte;
}

/* ================================ 应用层 ================================ */

/**
 * @brief MS1005_START 测试模式下产生START脉冲
 * @param none
 * @return none
 * @notice:
 */
void MS1005_START(void)
{
	TDC_START_PORT->BSRR = TDC_START_PIN;
	TDC_START_PORT->BRR = TDC_START_PIN;
}

/**
 * @brief MS1005_STOP 测试模式下产生STOP脉冲
 * @param none
 * @return none
 * @notice:
 */
void MS1005_STOP(void)
{
	TDC_STOP1_PORT->BSRR = TDC_STOP1_PIN;
	TDC_STOP1_PORT->BRR = TDC_STOP1_PIN;
}

/**
 * @brief TDC_HardwareReset 硬件复位
 * @param none
 * @return none
 * @notice: none
 */
static void TDC_HardwareReset(void)
{
	GPIO_ResetBits(TDC_RSTN_PORT, TDC_RSTN_PIN);
	Delay(500);
	GPIO_SetBits(TDC_RSTN_PORT, TDC_RSTN_PIN);
	Delay(200);
}

/**
 * @brief TDC_SoftwareReset 软件复位
 * @param none
 * @return none
 * @notice: TDC software reset
 */
static inline void TDC_SoftwareReset(void)
{
	TDC_WriteByte(RESET_DEVICE);
}

/**
 * @brief MS1005_Initialization MS1005初始化
 * @param none
 * @return none
 * @notice: MS1005 initialization
 */
void MS1005_AllReset(void)
{
	TDC_HardwareReset();
	TDC_SoftwareReset();
}

/**
 * @brief TDC_ReadCal 读取校准值
 * @param none
 * @return uint32_t 校准值
 * @notice: TDC read calibration value
 */
uint32_t TDC_ReadCal(void)
{
	return TDC_Read3Bytes(READ_CAL_DATA);
}

/**
 * @brief TDC_ReadState 读取状态值
 * @param none
 * @return uint32_t 状态值
 * @notice: TDC read State value
 */
uint32_t TDC_ReadState(void)
{
	return TDC_Read3Bytes(READ_STAT_RES);
}

/**
 * @brief TDC_ReadNCalResult 读取非校准模式下的结果值（输出为24bit整形）
 * @param uint8_t op 操作码（STOP通道结果寄存器地址）
 * @return uint32_t 结果值
 * @notice: stop1: B0~B9 | stop2: C0~C9
 */
uint32_t TDC_ReadNCalResult(uint8_t op)
{
	return TDC_Read3Bytes(op);
}

/**
 * @brief TDC_ReadCalResult 读取校准模式下的结果值（输出为浮点数）
 * @param uint8_t op 操作码（STOP通道结果寄存器地址）
 * @return float 结果值 8位整数 + 16位小数
 * @notice: stop1: B0~B9 | stop2: C0~C9
 */
float TDC_ReadCalResult(uint8_t op)
{
	uint32_t raw_data = TDC_Read3Bytes(op);
	uint8_t integer_part, frac_high, frac_low;
	integer_part = (raw_data >> 16) & 0xFF;
	frac_high = (raw_data >> 8) & 0xFF;
	frac_low = raw_data & 0xFF;
	uint16_t fraction_part = ((uint16_t)frac_high << 8) | frac_low;

	return (float)integer_part + ((float)fraction_part / 65536.0f);
}

/**
 * @brief TDC_Ncal1PulseCfg 非校准模式1脉冲配置
 * @param none
 * @return none
 * @notice: TDC non-calibration mode 1 pulse configuration
 */
void TDC_NcalModeCfg(void)
{
	TDC_WriteReg(WRITE_CONFIG_RES, 0x0B0C5060);
}

/**
 * @brief TDC_Cal1PulseCfg 校准模式1脉冲配置
 * @param none
 * @return none
 * @notice: TDC calibration mode 1 pulse configuration
 */
void TDC_CalModeCfg(void)
{
	TDC_WriteReg(WRITE_CONFIG_RES, TDC_CFG_CMD);
}

/**
 * @brief TDC_MeasureStart 测量开始
 * @param none
 * @return none
 * @notice: TDC measure start
 */
void TDC_MeasureStart(void)
{
	// TBD 应先硬件复位，再软件复位，最后初始化和启动测量
	TDC_WriteByte(INIT_DEVICE); // 初始化
	//TDC_WriteByte(START_CAL_TDC);    // 启动测量
}

/**
 * @brief TDC_Init TDC初始化
 * @param none
 * @return none
 * @notice: TDC initialization
 */
void TDC_Init(void)
{
	TDC_GpioConfig();
	MS1005_NVIC_Configuration();
	MS1005_SPI_Configuration(); // MS1005 SPI初始化配置(STM32 SPI1)
	MS1005_AllReset();
	/* 配置工作模式 */
	// TDC_Ncal1PulseCfg();
	// TDC_CalPulseCfg();
	/* 自检：写入配置并读回低8位以验证 SPI 通信 */
	// TDC_WriteRegVerify(WRITE_CONFIG_RES, TDC_CFG_CMD, READ_CONFIG_RES);
}

/**
 * @brief TDC_WriteRegVerify 写寄存器后读回验证（低8位）
 * @param op_write 写操作码
 * @param val  写入的32位值
 * @param read_op 读操作码，用于读取验证字节
 */
void TDC_WriteRegVerify(uint8_t op_write, uint32_t val, uint8_t read_op)
{
	TDC_WriteReg(op_write, val);
	Delay(1); 
	uint8_t read = TDC_ReadByte(read_op);
	if (read == (uint8_t)(val & 0xFF))
	{
		printf("SPI OK: wrote 0x%08X, read 0x%02X\r\n", val, read);
	}
	else
	{
		printf("SPI FAIL: wrote 0x%08X, read 0x%02X\r\n", val, read);
	}
}
