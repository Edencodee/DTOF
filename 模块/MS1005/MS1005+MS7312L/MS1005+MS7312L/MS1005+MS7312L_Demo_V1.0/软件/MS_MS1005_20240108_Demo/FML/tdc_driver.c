
#include "./tdc_driver.h"

#include "spi.h"

#include "usart.h"

//RTT
#include "SEGGER_RTT.h"

#define BITLENGTH 8

#define DUAL_CHANNEL 1

#define USE_TDC_MS1005 1

#define TDC_Debug_Mode 1

#define TDC_SPI SPI1

//SPI是否使用软件方式
#define USE_SPI_SOFT 1

static uint32_t SPITimeout = TDCT_LONG_TIMEOUT;

static void Tdc_Spi_Init(void);
static void Tdc_Hardware_Reset(void);
void Tdc_Register_Config(void);
static void Tdc_Signal_Init(void);
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

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


//Eden
float HEX_FLOAT(uint8_t data_0, uint8_t data_1, uint8_t data_2)
{
    float REG_TEMP = 0.0f;
    float REG_TEMP0 = 0.0f;
    float REG_TEMP1 = 0.0f;
    float DEC_TEMP = 0.0f;
    uint8_t i;
    unsigned long t = 0x00010000;
    DEC_TEMP = data_0;
    for (i = 8; i > 0; i--)
    {
        REG_TEMP0 = data_2 & 0x01;
        REG_TEMP1 = REG_TEMP0 / t;
        REG_TEMP = REG_TEMP + REG_TEMP1;
        data_2 >>= 1;
        t >>= 1;
    }
    for (i = 8; i > 0; i--)
    {
        REG_TEMP0 = data_1 & 0x01;
        REG_TEMP1 = REG_TEMP0 / t;
        REG_TEMP = REG_TEMP + REG_TEMP1;
        data_1 >>= 1;
        t >>= 1;
    }
    REG_TEMP = DEC_TEMP + REG_TEMP;
    return REG_TEMP;
}

float TDC_ReadWrite_Data24_Float(uint32_t senddata)
{
    int i = 0;
    uint32_t temp = 0;
    for (i = 0; i < 3; i++) {
        temp = (temp << 8) | TDC_ReadWrite_Data8((senddata >> 16) & 0xFF);  // 3 bytes continous
        senddata <<= 8;
    }
    // 提取字节：高8位整数，中8位和低8位小数
    uint8_t data_0 = (temp >> 16) & 0xFF;  // 整数部分
    uint8_t data_1 = (temp >> 8) & 0xFF;   // 小数高8位
    uint8_t data_2 = temp & 0xFF;          // 小数低8位
    return HEX_FLOAT(data_0, data_1, data_2);
}



uint8_t TDC_ReadWrite_Data8(uint8_t senddata)
{
#if USE_SPI_SOFT

	SPITimeout = TDCT_FLAG_TIMEOUT;

	/* 等待发送缓冲区为空，TXE事件 */
	//while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) == RESET)
	while ((SPI1->SR & 2) == 0) {
		if ((SPITimeout--) == 0)
			return SPI_TIMEOUT_UserCallback(0);
	}

	/* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
	//WRITE_REG(hspi1.Instance->DR, senddata);
	*((__IO uint8_t *)&hspi1.Instance->DR) = senddata;  //注意不要直接写 hspi1.Instance->DR = senddata;

	SPITimeout = TDCT_FLAG_TIMEOUT;

	/* 等待接收缓冲区非空，RXNE事件 */
	//while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) == RESET)
	while ((SPI1->SR & 1) == 0) {
		if ((SPITimeout--) == 0)
			return SPI_TIMEOUT_UserCallback(1);
	}

	/* 读取数据寄存器，获取接收缓冲区数据 */
	return READ_REG(hspi1.Instance->DR);

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
	int i = 0;
	u32 temp = 0;
	for (i = 0; i < 3; i++) {
		temp = (temp << 8) | TDC_ReadWrite_Data8((senddata >> 16) & 0xFF);  // 3 bytes continous
		senddata <<= 8;
	}
	return temp;
}


uint32_t TDC_ReadWrite_Data32(uint32_t senddata)
{
	int i = 0;
	uint32_t temp = 0;
	for (i = 0; i < 4; i++) {
		temp = (temp << 8) | TDC_ReadWrite_Data8((senddata >> 24) & 0xFF);  // 4 bytes continous
		senddata <<= 8;
	}
	return temp;
}

void Tdc_Register_Config(void)
{
	uint32_t  REG0 = 0, REG1 = 0;
	uint8_t Read_REG = 0;

	//REG0 = 0x22582410;// end hits  NO CAL
	REG0 = 0x22586410;
	//REG0 = 0x33586410;  
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

TDC_RESULTTypeDef TDC_Meas(void)
{
	//Eden
	//TDC_RESTypeDef tdc_res;
	TDC_RESULTTypeDef tdc_result;
	float Result_2;

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
//Eden
			// tdc_res.RES_0 = 0xFFFFFF;
			// tdc_res.RES_1 = 0;
			// return tdc_res;
			tdc_result.Result_0 = 0xFFFFFF;
			tdc_result.Result_1 = 0;
			return tdc_result;
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

	delay_ms(10);

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xB0);
	//Eden
	//tdc_res.RES_0 = TDC_ReadWrite_Data24(0);
	tdc_result.Result_0 = TDC_ReadWrite_Data24_Float(0);
	TDC_NSS_1; // one instruntion too fast
	//SEGGER_RTT_printf(0,"RES0= %d\r\n",tdc_res.RES_0);
//		 printf("RES0= %d\n",tdc_res.RES_0);

	__nop();
	__nop();
	__nop();

	delay_ms(10);

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xB1);
	//Eden
	//tdc_res.RES_0 = TDC_ReadWrite_Data24(0);
	Result_2 = TDC_ReadWrite_Data24_Float(0);
	TDC_NSS_1; // one instruntion too fast
	//SEGGER_RTT_printf(0,"RES0= %d\r\n",tdc_res.RES_0);
//		 printf("RES0= %d\n",tdc_res.RES_0);

	__nop();
	__nop();
	__nop();


	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xC0);

	//Eden
	//tdc_res.RES_1 = TDC_ReadWrite_Data24(0);
	tdc_result.Result_1 = TDC_ReadWrite_Data24_Float(0);
	//SEGGER_RTT_printf(0,"RES0= %d   RES1= %d\r\n",tdc_res.RES_0,tdc_res.RES_1);
	SEGGER_RTT_printf(0,"RES0= %.3f   RES1= %.3f\r\n",tdc_result.Result_0 *1000 ,tdc_result.Result_1 *1000);
//		printf("RES1= %x\n",tdc_res.RES_1);
	TDC_NSS_1;
	__nop();
	__nop();
	__nop();

	
	
	//Eden
	//tdc_res.RES_1 = tdc_res.RES_1 - tdc_res.RES_0; //MS1005 pulsewidth
	tdc_result.Result_1 = (tdc_result.Result_1 - tdc_result.Result_0) * 1000.0f; //MS1005 pulsewidth
	tdc_result.Result_0 = tdc_result.Result_0 * 1000.0f; //ns
	SEGGER_RTT_printf(0,"Dist = %.1f   PW = %.3f\r\n",tdc_result.Result_0 * 150.0f,tdc_result.Result_1);

	//return tdc_res;
	return tdc_result;
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
