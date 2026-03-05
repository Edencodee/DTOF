#include "ms1005.h"
#include "user_spi.h"
#include "systick.h"

#define TDC_TEST_MODE  	0
#define DELAY_TIME  	10


/**
 * @brief 0x33 0x3a 0x64 0x20 校准模式1脉冲配置命令
 * @notice: 配置详情参考规格书
 * 			0x33   STOP1 STOP2通道使能，两脉冲输出
 * 			0x3a   双通道单精度
 * 			0x64   STOP1上升沿有效 STOP2下降沿有效，自动产生校准值
 * 			0x20   Endhits 中断触发
 */
#define TDC_CFG_CMD     0x333a6420

uint32_t TDC_Res1[10];
uint32_t TDC_Res2[10];

/* ================================ 硬件抽象层 ================================ */

/**
 * @brief MS1005 GPIO初始化配置
 * @param none
 * @return none
 * @notice: 
 */
static void TDC_GpioConfig (void)
{
	/* TDC_NRST configure */
	rcu_periph_clock_enable(TDC_RSTN_CLK);
	gpio_mode_set(TDC_RSTN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TDC_RSTN_PIN);
	gpio_output_options_set(TDC_RSTN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TDC_RSTN_PIN);
	GPIO_BC(TDC_RSTN_PORT) = TDC_RSTN_PIN;

	/* TDC_INTN configure */
	rcu_periph_clock_enable(TDC_INTN_CLK);
	gpio_mode_set(TDC_INTN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, TDC_INTN_PIN);
	
	/* TDC_SSN configure */
	rcu_periph_clock_enable(TDC_SSN_CLK);
	gpio_mode_set(TDC_SSN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TDC_SSN_PIN);
	gpio_output_options_set(TDC_SSN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TDC_SSN_PIN);
	GPIO_BOP(TDC_SSN_PORT) = TDC_SSN_PIN;
	
#if TDC_TEST_MODE
	/* TDC_SSN configure */
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

#endif
}

/**
 * @brief MS1005 INIT外部中断配置
 * @param none
 * @return none
 * @notice: 
 */
static void TDC_ExtiConfig (void)
{
	/* enable INTN EXTI interrupt to the lowest priority */
	nvic_irq_enable(TDC_INTN_IRQn, 1U, 0U);
	/* connect INTN EXTI line to INTN pin */
	syscfg_exti_line_config(TDC_INTN_PORT_SOURCE, TDC_INTN_PIN_SOURCE);
	/* configure INTN EXTI line */
	exti_init(TDC_INTN_LINE, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_flag_clear(TDC_INTN_LINE);
}

/* ================================ 通信层 ================================ */

/**
 * @brief MS1005 SPI 片选高电平 使能传输
 * @param none
 * @return none
 * @notice: MS1005 SPI 片选高电平 使能传输
 */
 static void SPI_NssHigh (void) 
{
	for(int i = 0; i < DELAY_TIME; i++);
	GPIO_BOP(TDC_SSN_PORT) = TDC_SSN_PIN;
	for(int i = 0; i < 2 * DELAY_TIME; i++);
}

/**
 * @brief MS1005 SPI 片选低电平 停止传输
 * @param none
 * @return none
 * @notice: MS1005 SPI 片选低电平 停止传输
 */
static void SPI_NssLow (void) 
{
	for(int i = 0; i < 2 * DELAY_TIME; i++);
	GPIO_BC(TDC_SSN_PORT) = TDC_SSN_PIN;
	for(int i = 0; i < DELAY_TIME; i++);
}

/**
 * @brief MS1005 SPI SPI传输，同时接收响应数据，一发一收
 * @param uint8_t val 传输数据
 * @return uint8_t val 响应数据
 * @notice: MS1005 SPI 片选高电平 使能传输
 */
 static uint8_t SPI_Transmit (uint8_t val) 
{
	/* transmit */
	spi_i2s_data_transmit(SPI0, val);
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	/* receive */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));

	return spi_i2s_data_receive(SPI0);
}

/**
 * @brief TDC_WriteByte 写一个字节
 * @param uint8_t val 写入数据
 * @return none
 * @notice: 
 */
 static void TDC_WriteByte (uint8_t val)
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
void TDC_WriteReg (uint8_t op, uint32_t val)
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
uint8_t TDC_ReadByte (uint8_t op) 
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
 * @notice: 
 */
uint32_t TDC_Read3Bytes (uint8_t op)
{	
	SPI_NssLow();
	SPI_Transmit(op);
	for(int i = 0; i < DELAY_TIME; i++);
	uint8_t val0 = SPI_Transmit(0x00);
	uint8_t val1 = SPI_Transmit(0x00);
	uint8_t val2 = SPI_Transmit(0x00);
	SPI_NssHigh();
		
	u32_t res = {.buf = {val2, val1, val0, 0}};
	return res.word;
}


/* ================================ 应用层 ================================ */

/**
 * @brief TDC_HardwareReset 硬件复位
 * @param none
 * @return none
 * @notice: none
 */
static void TDC_HardwareReset (void)
{
	GPIO_BC(TDC_RSTN_PORT) = TDC_RSTN_PIN;
	delay_1ms(500);
	GPIO_BOP(TDC_RSTN_PORT) = TDC_RSTN_PIN;
	delay_1ms(200);
}

/**
 * @brief TDC_SoftwareReset 软件复位
 * @param none
 * @return none
 * @notice: TDC software reset
 */
inline static void TDC_SoftwareReset (void)
{
	TDC_WriteByte(OPERATE_CODE.RESET_DEVICE);
}

/**
 * @brief TDC_ReadCal 读取校准值
 * @param none
 * @return uint32_t 校准值
 * @notice: TDC read calibration value
 */
inline uint32_t TDC_ReadCal (void) 
{
	return TDC_Read3Bytes(RESULT_RES.CAL);
}

/**
 * @brief TDC_ReadState 读取状态值
 * @param none
 * @return uint32_t 状态值
 * @notice: TDC read State value
 */
inline uint32_t TDC_ReadState (void) 
{
	return TDC_Read3Bytes(OPERATE_CODE.READ_STAT_RES);
}

/**
 * @brief TDC_ReadRes 读取结果值
 * @param uint8_t op 操作码（STOP通道结果寄存器地址）
 * @return uint32_t 结果值
 * @notice: stop1: B0~B9 | stop2: C0~C9
 */
inline uint32_t TDC_ReadRes (uint8_t op) 
{
	return TDC_Read3Bytes(op);
}

/**
 * @brief TDC_ReadResF 读取结果值（转化为浮点数）
 * @param uint8_t op 操作码（STOP通道结果寄存器地址）
 * @return float 结果值
 * @notice: stop1: B0~B9 | stop2: C0~C9
 */
float TDC_ReadResF (uint8_t op) 
{
	u32_t uval32;
	uval32.word = TDC_Read3Bytes(op);
	return uval32.buf[0] + ((uint16_t)uval32.buf[1] << 8 | uval32.buf[2]) / 65536.0f; // TBD
}

/**
 * @brief TDC_Ncal1PulseCfg 非校准模式1脉冲配置
 * @param none
 * @return none
 * @notice: TDC non-calibration mode 1 pulse configuration
 */
void TDC_NcalPulseCfg (void) 
{
	TDC_WriteReg(OPERATE_CODE.WRITE_CONFIG_RES, 0x0B0C5060); 
}

/**
 * @brief TDC_Cal1PulseCfg 校准模式1脉冲配置
 * @param none
 * @return none
 * @notice: TDC calibration mode 1 pulse configuration
 */
void TDC_CalPulseCfg (void) 
{
	TDC_WriteReg(OPERATE_CODE.WRITE_CONFIG_RES, TDC_CFG_CMD); 
}


/**
 * @brief TDC_MeasureStart 测量开始
 * @param none
 * @return none
 * @notice: TDC measure start
 */
void TDC_MeasureStart (void)
{
	//TBD 应先硬件复位，再软件复位，最后初始化和启动测量
	TDC_WriteByte(OPERATE_CODE.INIT_DEVICE);           	// 初始化 
	TDC_WriteByte(OPERATE_CODE.START_CAL_TDC);	// 启动测量
}


/**
 * @brief TDC_Init TDC初始化
 * @param none
 * @return none
 * @notice: TDC initialization
 */
void TDC_Init (void)
{
	TDC_GpioConfig();
	TDC_ExtiConfig();
	TDC_HardwareReset();
	TDC_SoftwareReset();
//	TDC_Ncal1PulseCfg();
	TDC_Cal1PulseCfg();
	TDC_MeasureStart();
}
