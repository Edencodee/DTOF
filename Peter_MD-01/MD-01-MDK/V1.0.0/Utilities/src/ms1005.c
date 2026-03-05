#include "ms1005.h"
#include "user_spi.h"
#include "systick.h"

#define DELAY_TIME  10

uint32_t TDC_Res1[10];
uint32_t TDC_Res2[10];

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
}

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

static void TDC_HardwareReset (void)
{
	GPIO_BC(TDC_RSTN_PORT) = TDC_RSTN_PIN;
	delay_1ms(500);
	GPIO_BOP(TDC_RSTN_PORT) = TDC_RSTN_PIN;
	delay_1ms(200);
}

static uint8_t SPI0_Transmit (uint8_t val) 
{
	/* transmit */
	spi_i2s_data_transmit(SPI0, val);
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	/* receive */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
	return spi_i2s_data_receive(SPI0);
}

static void SPI0_NssHigh (void) 
{
	for(int i = 0; i < DELAY_TIME; i++);
	GPIO_BOP(TDC_SSN_PORT) = TDC_SSN_PIN;
	for(int i = 0; i < 2 * DELAY_TIME; i++);
}

static void SPI0_NssLow (void) 
{
	for(int i = 0; i < 2 * DELAY_TIME; i++);
	GPIO_BC(TDC_SSN_PORT) = TDC_SSN_PIN;
	for(int i = 0; i < DELAY_TIME; i++);
}

static void TDC_WriteByte (uint8_t val)
{
	SPI0_NssLow();
	SPI0_Transmit(val);
	SPI0_NssHigh();
}

inline static void TDC_SoftwareReset (void)
{
	TDC_WriteByte(TDC_POWER_ON_RESET);
}

uint8_t TDC_ReadByte (uint8_t op) 
{	
	SPI0_NssLow();
	SPI0_Transmit(op);
	for(int i = 0; i < DELAY_TIME; i++);
	uint8_t val = SPI0_Transmit(0x00);
	SPI0_NssHigh();
	
	return val;
}

inline uint8_t TDC_ReadCfgLow8 (void) 
{
	return TDC_ReadByte(0xBC);
}

uint32_t TDC_Read3Bytes (uint8_t op)
{	
	SPI0_NssLow();
	SPI0_Transmit(op);
	for(int i = 0; i < DELAY_TIME; i++);
	uint8_t val0 = SPI0_Transmit(0x00);
	uint8_t val1 = SPI0_Transmit(0x00);
	uint8_t val2 = SPI0_Transmit(0x00);
	SPI0_NssHigh();
		
	u32_t res = {.buf = {val2, val1, val0, 0}};
	return res.word;
}

inline uint32_t TDC_ReadCal (void) 
{
	return TDC_Read3Bytes(0xBB);
}

inline uint32_t TDC_ReadState (void) 
{
	return TDC_Read3Bytes(0xBD);
}

inline uint32_t TDC_ReadRes (uint8_t op) // stop1: B0~B9 / stop2: C0~C9
{
	return TDC_Read3Bytes(op);
}

float TDC_ReadResF (uint8_t op) // stop1: B0~B9 / stop2: C0~C9
{
	u32_t uval32;
	uval32.word = TDC_Read3Bytes(op);
	return uval32.buf[0] + ((uint16_t)uval32.buf[1] << 8 | uval32.buf[2]) / 65536.0f; // TBD
}

void TDC_WriteReg (uint8_t op, uint32_t val)
{
	u32_t uval32;
	uval32.word = val;
	
	SPI0_NssLow();
	SPI0_Transmit(op);
	SPI0_Transmit(uval32.buf[3]);
	SPI0_Transmit(uval32.buf[2]);
	SPI0_Transmit(uval32.buf[1]);
	SPI0_Transmit(uval32.buf[0]);
	SPI0_NssHigh();
}

void TDC_Ncal1PulseCfg (void) 
{
	TDC_WriteReg(TDC_CFG_REG, 0x0B0C5060); // TBD, 0x0B045060 @ 30ps
}

void TDC_Cal1PulseCfg (void) 
{
	TDC_WriteReg(TDC_CFG_REG, 0x0BCA60D0); // TBD, 0x0BCE60D0 @ 30ps
}

void TDC_MeasureStart (void)
{
	TDC_WriteByte(TDC_INIT);
	TDC_WriteByte(TDC_START_CAL_TDC);
}

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
