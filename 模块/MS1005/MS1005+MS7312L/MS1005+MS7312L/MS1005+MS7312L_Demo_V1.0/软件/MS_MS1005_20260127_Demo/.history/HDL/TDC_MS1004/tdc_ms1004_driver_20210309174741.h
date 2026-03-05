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
/**
 * @brief 配置寄存器
 */
typedef struct
{
    uint32_t RFEDGE1      : 1; //0,  通道 1 的边沿敏感性: 0 = 上升或下降沿, 1 = 上升或下降沿
    uint32_t RFEDGE2      : 1; //1,  通道 2 的边沿敏感性: 0 = 上升或下降沿, 1 = 上升或下降沿
    uint32_t RESERVED1    : 2; //2-3,必须设置为默认值
    uint32_t EN_INT       : 3; //4:6,INIT输出中断源选择
    uint32_t EN_SEL_TIMO  : 1; //7,  溢出时间功能开启（和SEL_TIMO 功能配合使用）： 0 = 关闭, 1 = 开启
    uint32_t NEG_START    : 1; //8,  反向 start 通道 1 输入：0 = 上升沿触发, 1 = 下降沿触发
    uint32_t NEG_STOP1    : 1; //9,  反向 stop 通道 1 输入：0 = 上升沿触发, 1 = 下降沿触发
    uint32_t NEG_STOP2    : 1; //10, 反向 stop 通道 2 输入：0 = 上升沿触发, 1 = 下降沿触发
    uint32_t RESERVED1    : 2; //11, 必须设置为默认值
    uint32_t NO_CAL_AUTO  : 1; //12,    必须设置位默认值开启/关闭产生校准值（如需开启CALIBRATE，该位则必须设置为0）
    uint32_t CALIBRATE    : 1; //13,    开启/关闭在ALU 中的校准： 0 = 校准关闭, 1 = 校准开启
    uint32_t SEL_TSTO2    : 2; //14:15  定义EN_START 管脚功能（如果设SEL_TSTOP2 > 0 ，那么EN_START 内部上拉高电平）
    uint32_t EN_FAST_INIT : 1; //16,    启动快速初始化功能, 0 = 关闭功能, 1 = 启动功能
    uint32_t EN_ERR_VAL   : 1; //17,    由于时间溢出强迫ALU 写入0xFFFFFFFF 到结果寄存器: 0 = 关闭, 1 = 开启
    uint32_t DOUBLE_RES   : 1; //18,    双精度测量开启: 0 = 关闭, 1 = 开启
    uint32_t START_CLKHS  : 1; //19,    设置晶振的开启和关闭
    uint32_t DIV_CLKHS    : 2; //20:21, 设置 CLKHS 高速参考时钟的分频因数
    uint32_t SEL_TIMO     : 2; //22:23, 溢出时间选择
    uint32_t HITIN1       : 4; //24:27, 在stop通道1的预期脉冲数
    uint32_t HITIN2       : 4; //28:31, 在stop通道2的预期脉冲数
} TDC_CONFIG_RES;

/**
 * @brief 状态寄存器
 */
typedef struct
{
    uint32_t STOP1_RESULT : 4; //0:3   STOP1 结果地址指针
    uint32_t RESERVED1    : 1; //4     默认 0
    uint32_t STOP1_CNT    : 4; //5:8   STOP1 脉冲数
    uint32_t STOP2_CNT    : 4; //9:12  STOP2 脉冲数
    uint32_t TDC_OUT      : 1; //13    TDC 溢出
    uint32_t TIMEOUT      : 1; //14    时间溢出
    uint32_t RESERVED2    : 1; //15    默认 0
    uint32_t STOP2_RESULT : 4; //16:19 STOP2 结果地址指针
    uint32_t RESERVED3    : 4; //20:23 默认 0
    uint32_t RESERVED4    : 8; //24:31 默认 0
} TDC_STAT_RES;


#ifdef __cplusplus
}
#endif

#endif
