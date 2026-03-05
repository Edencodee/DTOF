/**
 * @file drv_temp.h
 * @brief 
 * @author jinf (jinf_li@foxmail.com)
 * @version 1.0
 * @date 2021-03-05
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-03-05 <td>1.0     <td>jinf    <td>内容
 * </table>
 */



#ifndef __DRV_TEMP_H
#define __DRV_TEMP_H

#include "bsp.h"

/**
 * @brief 获取实时温度值
 * @return temperatur : 0xFFFF 获取失败others: 单位 0.01摄氏度
    
 */
uint16_t Get_Temp_Value(void);

#endif /* __DRV_TEMP_H */
