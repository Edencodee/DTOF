
#include "./tdc_driver.h"

#include "spi.h"

#include "usart.h"

#define BITLENGTH   8 

#define DUAL_CHANNEL 1

#define USE_TDC_MS1004 1

#define TDC_Debug_Mode 0


//SPI是否使用软件方式
#define USE_SPI_SOFT  1

static void Tdc_Spi_Init(void);
static void Tdc_Hardware_Reset(void);
void Tdc_Register_Config(void);
static void Tdc_Signal_Init(void);

void Tdc_Driver_Init(void)
{
    Tdc_Spi_Init();
    printf("tdc spi init ok\n");
    
    Tdc_Signal_Init();
    
    Tdc_Hardware_Reset();
    printf("tdc has reset\n");
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
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TDC_START_GPIO_Port, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = TDC_RSTN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TDC_RSTN_GPIO_Port, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = TDC_INTN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;//GPIO_NOPULL;
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
        
//    printf("spi send step 0\n");
//    //while(!SPI_GetFlagStatus(TDC_SPI, SPI_FLAG_TXEPT));
//    WRITE_REG(TDC_SPI->TDR, senddata);
//    printf("spi send step 1\n");
//    while(1)
//    {
//        if(SPI_GetFlagStatus(TDC_SPI, SPI_FLAG_TXEPT)) 
//        {
//            break;
//        } 
//    }
//    printf("spi send step 2\n");
//    while (1) 
//    {
//        if(SPI_GetFlagStatus(TDC_SPI, SPI_FLAG_RXAVL)) 
//        {
//            printf("spi send step 3\n");   
//            return READ_REG(TDC_SPI->RDR);;
//        }
//    }
    uint8_t recvdata = 0;
    switch (HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&senddata, (uint8_t *)&recvdata, 1, 1000))
    {
        case HAL_OK:  
         // printf("tdc spi ok\n");
          break;
        case HAL_TIMEOUT:
          printf("tdc spi time out\n");
        /* An Error Occur ______________________________________________________ */
        case HAL_ERROR:
          /* Call Timeout Handler */
          //Error_Handler();
          printf("tdc spi error\n");
          break;        
        default:
          printf("tdc error\n");
          break;        
    }
    
    return recvdata;
#else
    uint8_t i = 0;
    uint8_t recvdata = 0x00;

    for(i = 0; i < BITLENGTH; i++) { 
        SCK_H;
        // Tx data use MSB mode
        if(senddata & ((0x01) << (BITLENGTH - 1))) {
            MOSI_H;
        }
        else {
            MOSI_L;
        }
        senddata <<= 1;  

        SCK_L;
        
        recvdata <<= 1 ;
        if((MISO_IN) != 0) {
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
    for( i = 0; i < 3; i++)
    {
        temp = (temp << 8) | TDC_ReadWrite_Data8((senddata >> 16) & 0xFF);
        senddata <<= 8;
    }
    return temp;
}
uint32_t TDC_ReadWrite_Data32(uint32_t senddata)
{
    int i = 0;
    uint32_t temp = 0;
    for( i = 0; i < 4; i++)
    {
        temp = (temp << 8) | TDC_ReadWrite_Data8((senddata >> 24) & 0xFF);
        senddata <<= 8;
    }
    return temp;
}

void Tdc_Register_Config(void)
{
    unsigned int REG0;

	//??? stop1???,stop2???
   REG0 = 0x225A6490;  //Dual

	//  REG0 = 0x035A6091;  //Single
	
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
}

//#define TDC_Debug_Mode     1//TDC 调试模式

TDC_RESTypeDef  TDC_Meas(void)
{
    TDC_RESTypeDef tdc_res;
    
    uint32_t  TDCTimeout = TDCT_LONG_TIMEOUT;
	  uint32_t  status = 0;
   

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0x70); // ????????GP2???????
	TDC_NSS_1;


    TDC_START_0;  
    TDC_START_1;
    TDC_START_0;

	
	while (GET_TDC_INTN)
	{
        
		if((TDCTimeout--) <= 10)
        {
          //assert_failed(uerrorCode[4],3);
          //break;
			tdc_res.RES_0 = 0xFFFFFF;
			tdc_res.RES_1 = 0;
			return tdc_res;
        }
	}

#if USE_TDC_MS1022
	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xb0);
	tdc_res.RES_0 = TDC_ReadWrite_Data32(0);
	TDC_NSS_1;

	TDC_NSS_0;
	TDC_ReadWrite_Data32(0x81124200); // ?CH //2015/10/7 //2016_5_20
	TDC_NSS_1;

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xb1);
	tdc_res.RES_1 = TDC_ReadWrite_Data32(0);
	TDC_NSS_1;

	// =================================================================

	TDC_NSS_0;
	TDC_ReadWrite_Data32(0x81000000);
	TDC_NSS_1;

	// =================================================================

	TDC_NSS_0;
	TDC_ReadWrite_Data32(0x81014200); // ?CH //2015/10/7 //2016_5_20
	TDC_NSS_1;  
#endif

#if USE_TDC_MS1004
	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xB0);
	tdc_res.RES_0 = TDC_ReadWrite_Data24(0);
	TDC_NSS_1;


	
	delay_us(1);  
	TDC_NSS_0;	
	
#if DUAL_CHANNEL
	//??? stop1???,stop2???
	TDC_ReadWrite_Data8(0xC0); 	
#else
	//????? stop1
	
	TDC_ReadWrite_Data8(0xB1);
#endif	
	
	tdc_res.RES_1 = TDC_ReadWrite_Data24(0);
	TDC_NSS_1;

#if TDC_Debug_Mode

	TDC_NSS_0;
	TDC_ReadWrite_Data8(0xbd);
	status = TDC_ReadWrite_Data24(0);
	TDC_NSS_1;	

	printf("sta = %06x\n", status);
	printf("res0 = %x, res1 = %x\n", tdc_res.RES_0, tdc_res.RES_1);
#endif

	tdc_res.RES_1 = tdc_res.RES_1 - tdc_res.RES_0;  //MS1004????????stop?start????
#endif
    return tdc_res;  
    
}

/********************************************************************************************************
**???? :Meas_SinglePoint(void)                      
**???? : ??????
**???? :?
**???? :?????(????)
********************************************************************************************************/
Lidar_PointTypeDef Meas_SinglePoint(void)
{
    Lidar_PointTypeDef tmp_point;
    TDC_RESTypeDef tdc_res;
    
    tdc_res = TDC_Meas();
    
    tmp_point.distance  = (tdc_res.RES_0 * 37481) >> 16;
   // tmp_point.intensity = (tdc_res.RES_1 * 2500 ) >> 16;
    tmp_point.intensity = (tdc_res.RES_1 * 37481 ) >> 17;
	
    return tmp_point;
}

