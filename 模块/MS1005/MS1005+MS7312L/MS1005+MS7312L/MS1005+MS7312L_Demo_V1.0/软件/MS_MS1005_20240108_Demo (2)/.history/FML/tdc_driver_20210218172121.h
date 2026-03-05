
#ifndef __TDC_DRIVER_H
#define __TDC_DRIVER_H

#include "board.h"

/*TDC中断等待超时时间*/
#define TDCT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define TDCT_LONG_TIMEOUT         ((uint32_t)0x1000)


/*信息输出*/
#define FLASH_DEBUG_ON         1

#define SPI_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define SPI_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define SPI_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                 SPI                         }while(0)


typedef struct lidar_point
{
    uint16_t distance;   //unit: mm
    uint16_t intensity;  
}Lidar_PointTypeDef;

typedef struct
{
    uint32_t RES_0;		//TDC???1??,?start????stop?????	
    uint32_t RES_1;		//TDC???2??,????stop??????stop?????	
}TDC_RESTypeDef;



void Tdc_Driver_Init(void);
uint32_t TDC_ReadWrite_Data24(uint32_t senddata);
uint8_t TDC_ReadWrite_Data8(uint8_t senddata);


Lidar_PointTypeDef Meas_SinglePoint(void);
TDC_RESTypeDef     TDC_Meas(void);


#endif // !__TDC_DRIVER_H
