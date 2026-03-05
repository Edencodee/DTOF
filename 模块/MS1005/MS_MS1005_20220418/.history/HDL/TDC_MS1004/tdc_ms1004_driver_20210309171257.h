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
    uint32_t RFEDGE1      : 1; //0,  通道 1 的边沿敏感性: 0 = 上升或下降沿, 1 = 上升或下降沿
    uint32_t RFEDGE2      : 1; //1,  通道 2 的边沿敏感性: 0 = 上升或下降沿, 1 = 上升或下降沿
    uint32_t RESERVED1    : 2; //2-3,必须设置位默认值
    uint32_t EN_INT       : 3; //4:6,INIT输出中断源选择
    uint32_t EN_SEL_TIMO  : 1; //7,
    uint32_t NEG_START    : 1; //8,
    uint32_t NEG_STOP1    : 1; //9,
    uint32_t NEG_STOP2    : 1; //10,
    uint32_t RESERVED1    : 2; //11,
    uint32_t NO_CAL_AUTO  : 1; //12, 必须设置位默认值开启/关闭产生校准值（如需开启CALIBRATE，该位则必须设置为0）
    uint32_t CALIBRATE    : 1; //13, 开启/关闭在ALU 中的校准, 0 = 校准关闭, 1 = 校准开启
    uint32_t SEL_TSTO2    : 2; //14:15
    uint32_t EN_FAST_INIT : 1; //16, 启动快速初始化功能, 0 = 关闭功能, 1 = 启动功能
    uint32_t EN_ERR_VAL   : 1; //17,
    uint32_t DOUBLE_RES   : 1; //18, 双精度测量开启
    uint32_t START_CLKHS  : 1; //19, 设置晶振的开启和关闭
    uint32_t DIV_CLKHS    : 2; //20:21, 设置 CLKHS 高速参考时钟的分频因数
    uint32_t SEL_TIMO     : 2; //22:23, 溢出时间
}TDC_CONFIG_RES

#ifdef __cplusplus
}
#endif

#endif
