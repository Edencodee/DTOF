/**
 * @file meas_dist.h
 * @brief 
 * @author jinf (jinf_li@foxmail.com)
 * @version 1.0
 * @date 2021-03-10
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-03-10 <td>1.0     <td>jinf    <td>内容
 * </table>
 */

#ifndef __MEAS_DIST_H
#define __MEAS_DIST_H

#include "tdc_driver.h"



typedef struct
{
    uint16_t min; 
    uint16_t max;
}RANGETypedef;

typedef enum{
    LOWSIDE   = 0, //低于范围
    INSIDE    = 1, //在范围内
    HIGHSIDE  = 2, //大于范围
}RangeCompareTypedef;

typedef enum{  
    CorrectionMode = 0,  //校正模式
    FixPointMode = 1,    //定点模式
    RotationMode = 2,    //旋转模式  
}MEASMODE;

//强度范围： MIN_Intensity - MAX_Intensity

#if DUAL_CHANNEL  
//双通道 stop1上升沿,stop2下降沿
#define MIN_Intensity     (200)	
#else   //单通道模式 stop1
#define MIN_Intensity     (350)
#endif

#define MAX_Intensity     (5000)
//距离范围： MIN_Distance - MAX_Distance
#define MIN_Distance      (50)     //mm
#define MAX_Distance      (60000)  //mm
//无效数据
#define InValid_Dist      (65535)
#define InValid_Itst      (0


#endif /* __MEAS_DIST_H */

