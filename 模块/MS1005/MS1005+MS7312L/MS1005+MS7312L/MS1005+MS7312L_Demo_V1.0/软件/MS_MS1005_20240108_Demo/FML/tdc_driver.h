
#ifndef __TDC_DRIVER_H
#define __TDC_DRIVER_H

#include "board.h"

/*TDC中断等待超时时间*/
#define TDCT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define TDCT_LONG_TIMEOUT         ((uint32_t)0x2000)


/*信息输出*/
#define SPI_DEBUG_ON         1

#define SPI_INFO(fmt,arg...)           printf("<<-SPI-INFO->> "fmt"\n",##arg)
#define SPI_ERROR(fmt,arg...)          printf("<<-SPI-ERROR->> "fmt"\n",##arg)
#define SPI_DEBUG(fmt,arg...)          do{\
                                          if(SPI_DEBUG_ON)\
                                          printf("<<-SPI-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


typedef struct lidar_point {
	uint16_t distance;   //unit: mm
	uint16_t intensity;
} Lidar_PointTypeDef;

typedef struct {
	uint32_t RES_0;		//TDC寄存器1的值，即start到第一个stop信号的值。
	uint32_t RES_1;		//TDC寄存器2的值，即第一个stop信号到第二个stop信号的值。
} TDC_RESTypeDef;

typedef struct {
	float Result_0;		//TDC寄存器1的值，即start到第一个stop信号的值。
	float Result_1;		//TDC寄存器2的值，即第一个stop信号到第二个stop信号的值。
} TDC_RESULTTypeDef;

//Eden
float HEX_FLOAT(uint8_t data_0, uint8_t data_1, uint8_t data_2);
float TDC_ReadWrite_Data24_Float(uint32_t senddata);

void Tdc_Driver_Init(void);
uint32_t TDC_ReadWrite_Data24(uint32_t senddata);
uint8_t TDC_ReadWrite_Data8(uint8_t senddata);


//Lidar_PointTypeDef Meas_SinglePoint(void);
//TDC_RESTypeDef     TDC_Meas(void);
TDC_RESULTTypeDef     TDC_Meas(void);


#endif // !__TDC_DRIVER_H
