
#include "./tdc_driver.h"

#include "spi.h"

#include "usart.h"
#include "delay.h"  // delay_us/delay_ms

#define BITLENGTH 8

#define DUAL_CHANNEL 1

#define USE_TDC_MS1005 1

#define TDC_Debug_Mode 1

#define TDC_SPI SPI1

//SPI是否使用软件方式
#define USE_SPI_SOFT 1

static uint32_t SPITimeout = TDCT_LONG_TIMEOUT;

// NSS切换所需的最小高电平保持时间（按器件时序要求调整）
#ifndef TDC_NSS_GAP_US
#define TDC_NSS_GAP_US 10u
#endif

// 可切换实现：当前使用delay_us，亦可改为少量__NOP()或DWT周期延时
static inline void TDC_NSS_Gap(void)
{
	// 若已知仅需极短脉冲，可改为若干__NOP();
	// 例如：for(volatile int i=0;i<6;i++){ __NOP(); }
	delay_us(TDC_NSS_GAP_US);
}

static void Tdc_Spi_Init(void);
static void Tdc_Hardware_Reset(void);
void Tdc_Register_Config(void);
static void Tdc_Signal_Init(void);
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);
static HAL_StatusTypeDef TDC_Spi_TxRx_DMA(uint8_t *tx, uint8_t *rx, uint16_t len);
static volatile int8_t spi_dma_state = 0; // 0 busy, 1 done, -1 error

void Tdc_Driver_Init(void)
{

	Tdc_Signal_Init();

	Tdc_Hardware_Reset(); //备注：硬件复位要在SPI使能之前
	printf("tdc has reset\n");
	Tdc_Spi_Init();
	printf("tdc spi init ok\n");

	__HAL_SPI_ENABLE(&hspi1);

	Tdc_Register_Config();

	printf("tdc res init ok\n");
}

static void Tdc_Spi_Init(void)
{
#if USE_SPI_SOFT
	MX_SPI1_Init();
#else

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	/**SPI1 GPIO Configuration
	PB3     ------> SPI1_SCK
	PB4     ------> SPI1_MISO
	PB5     ------> SPI1_MOSI
	*/

	GPIO_InitStruct.Pin = TDC_MISO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TDC_MISO_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TDC_SCK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TDC_SCK_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TDC_MOSI_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TDC_MOSI_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TDC_NSS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TDC_NSS_GPIO_Port, &GPIO_InitStruct);

	SCK_L;
	TDC_NSS_1;
	MOSI_L;

#endif
}

static void Tdc_Signal_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = TDC_START_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(TDC_START_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TDC_RSTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TDC_RSTN_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TDC_INTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP; //GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TDC_INTN_GPIO_Port, &GPIO_InitStruct);

	TDC_NSS_1;
	TDC_START_0;
	TDC_RSTN_0;
}
static void Tdc_Hardware_Reset(void)
{
	TDC_RSTN_0;
	delay_us(1000);
	TDC_RSTN_1;
	delay_us(1000);
}

uint8_t TDC_ReadWrite_Data8(uint8_t senddata)
{
#if USE_SPI_SOFT
	uint8_t tx = senddata;
	uint8_t rx = 0;
	if (TDC_Spi_TxRx_DMA(&tx, &rx, 1) != HAL_OK) {
		return SPI_TIMEOUT_UserCallback(0);
	}
	return rx;

#else
	uint8_t i = 0;
	uint8_t recvdata = 0x00;

	for (i = 0; i < BITLENGTH; i++) {
		SCK_H;
		// Tx data use MSB mode
		if (senddata & ((0x01) << (BITLENGTH - 1))) {
			MOSI_H;
		} else {
			MOSI_L;
		}
		senddata <<= 1;

		SCK_L;

		recvdata <<= 1;
		if ((MISO_IN) != 0) {
			recvdata++;
		}
	}
	return recvdata;
#endif
}

uint32_t TDC_ReadWrite_Data24(uint32_t senddata)
{
	uint8_t tx[3];
	uint8_t rx[3] = {0};
	tx[0] = (senddata >> 16) & 0xFF;
	tx[1] = (senddata >> 8) & 0xFF;
	tx[2] = senddata & 0xFF;

	if (TDC_Spi_TxRx_DMA(tx, rx, 3) != HAL_OK) {
		SPI_TIMEOUT_UserCallback(0);
		return 0;
	}

	return ((uint32_t)rx[0] << 16) | ((uint32_t)rx[1] << 8) | rx[2];
}


uint32_t TDC_ReadWrite_Data32(uint32_t senddata)
{
	uint8_t tx[4];
	uint8_t rx[4] = {0};
	tx[0] = (senddata >> 24) & 0xFF;
	tx[1] = (senddata >> 16) & 0xFF;
	tx[2] = (senddata >> 8) & 0xFF;
	tx[3] = senddata & 0xFF;

	if (TDC_Spi_TxRx_DMA(tx, rx, 4) != HAL_OK) {
		SPI_TIMEOUT_UserCallback(0);
		return 0;
	}

	return ((uint32_t)rx[0] << 24) | ((uint32_t)rx[1] << 16) | ((uint32_t)rx[2] << 8) | rx[3];
}

void Tdc_Register_Config(void)
{
	uint32_t  REG0 = 0, REG1 = 0;
	uint8_t Read_REG = 0;

	REG0 = 0x22582410;// end hits  NO CAL
	TDC_NSS_0;
	TDC_ReadWrite_Data8(0x50);
	TDC_NSS_1;

	__nop();
	__nop();
	__nop();


	TDC_NSS_0;
	TDC_ReadWrite_Data8(0x80);
	TDC_ReadWrite_Data32(REG0);
	TDC_NSS_1;

	__nop();
	__nop();
	__nop();

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0x70); // reset
	TDC_NSS_1;
	TDC_NSS_0;

	TDC_ReadWrite_Data8(0xBC);
	Read_REG = TDC_ReadWrite_Data8(0x00);
	TDC_NSS_1;


	printf("config regist: 0x%02x 0x%02x\n", Read_REG, REG0 & 0xFF);


}

//#define TDC_Debug_Mode     1//TDC 调试模式

TDC_RESTypeDef TDC_Meas(void)
{
	TDC_RESTypeDef tdc_res;

	uint32_t TDCTimeout = TDCT_LONG_TIMEOUT;
	uint32_t status = 0;
	uint32_t  CAL;
	int i = 0;
	int a = 0;

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0x70); // reset
	TDC_NSS_1;

	TDC_START_0;
	TDC_START_1;
	TDC_START_1;
	TDC_START_0;

	while (GET_TDC_INTN) { //wait for INTN = LOW ?

		if ((TDCTimeout--) <= 10) { //Timeout check

			tdc_res.RES_0 = 0xFFFFFF;
			tdc_res.RES_1 = 0;
			return tdc_res;
		}
	}
//		TDC_NSS_0;
//    TDC_ReadWrite_Data8(0xbd);
//		status= TDC_ReadWrite_Data24(0);
//    printf("status0= %x\n",status);
//    TDC_NSS_1;
//    __nop();
//    __nop();
//    __nop();

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xbb);
	CAL = TDC_ReadWrite_Data24(0);
//   printf("CAL= %d\n",CAL);
	TDC_NSS_1;


//delay_ms(1);
	//TDC_NSS_Gap();

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xB0);
	tdc_res.RES_0 = TDC_ReadWrite_Data24(0);
	TDC_NSS_1; // one instruntion too fast
//		 printf("RES0= %d\n",tdc_res.RES_0);
	__nop();
	__nop();
	__nop();


	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xC0);
	tdc_res.RES_1 = TDC_ReadWrite_Data24(0);
//		printf("RES1= %x\n",tdc_res.RES_1);
	TDC_NSS_1;
	__nop();
	__nop();
	__nop();

	tdc_res.RES_1 = tdc_res.RES_1 - tdc_res.RES_0; //MS1005 pulsewidth


	return tdc_res;
}

/**
  * @brief  等待超时回调函数
  * @param  None.
  * @retval None.
  */
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
	/* 等待超时后的处理,输出错误信息 */
	SPI_ERROR("SPI 等待超时!errorCode = %d", errorCode);
	return 0;
}

static HAL_StatusTypeDef TDC_Spi_TxRx_DMA(uint8_t *tx, uint8_t *rx, uint16_t len)
{
	spi_dma_state = 0;

	if (HAL_SPI_TransmitReceive_DMA(&hspi1, tx, rx, len) != HAL_OK) {
		spi_dma_state = -1;
		return HAL_ERROR;
	}

	uint32_t start = HAL_GetTick();
	while (spi_dma_state == 0) {
		if ((HAL_GetTick() - start) > 10) { // ~10ms 超时保护
			HAL_SPI_DMAStop(&hspi1);
			spi_dma_state = -1;
			return HAL_TIMEOUT;
		}
	}

	return (spi_dma_state > 0) ? HAL_OK : HAL_ERROR;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1) {
		spi_dma_state = 1;
	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1) {
		spi_dma_state = -1;
	}
}
