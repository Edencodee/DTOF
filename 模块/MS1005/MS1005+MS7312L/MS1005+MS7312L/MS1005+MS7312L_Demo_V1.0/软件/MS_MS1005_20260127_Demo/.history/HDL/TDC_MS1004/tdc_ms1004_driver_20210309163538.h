/**
 * @file tdc_ms1004_driver.h
 * @brief 
 * @author jinf (jinf_li@foxmail.com)
 * @version 1.0
 * @date 2021-03-09
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-03-09 <td>1.0     <td>jinf    <td>内容
 * </table>
 */

#ifndef __TDC_MS1004_DRIVER_H
#define __TDC_MS1004_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
    uint32_t rfedge1 : 1;     //通道 1 的边沿敏感性: 0 = 上升或下降沿, 1 = 上升或下降沿
    uint32_t RFEDGE2 : 1;     //通道 2 的边沿敏感性: 0 = 上升或下降沿, 1 = 上升或下降沿
    uint32_t reserved : 2;    //必须设置位默认值
    uint32_t EN_INT : 3;      //INIT输出中断源选择
    uint32_t EN_SEL_TIMO : 1; //
    uint32_t NEG_START : 1;
}TDC_CONFIG_RES

#ifdef __cplusplus
}
#endif

#endif
