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
    u16 max;
}RANGETypedef;
typedef enum{
    LOWSIDE   = 0,
    INSIDE    = 1,
    HIGHSIDE  = 2,
}RangeCompareTypedef;

typedef enum{  
    CorrectionMode = 0,  //校正模式
    FixPointMode = 1,    //定点模式
    RotationMode = 2,    //旋转模式  
}MEASMODE;


#endif /* __MEAS_DIST_H */

