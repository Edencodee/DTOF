/**
 * @file meas_dist.c
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

#include "meas_dist.h"

#define TDC_Debug_Mode     0//TDC 调试模式

static Lidar_PointTypeDef meas_point[MAX_POINT_NUM];

const RANGETypedef IntensityRange = {MIN_Intensity, MAX_Intensity};    //强度范围
const RANGETypedef DistanceRange  = {MIN_Distance,  MAX_Distance};     //距离范围
const Lidar_PointTypeDef invalid_point = {InValid_Dist, InValid_Itst}; //无效点

typedef FlagStatus (*POINT_Analysis)(Lidar_PointTypeDef *point);


/********************************************************************************************************
**函数信息 ：Meas_SinglePoint(void)                      
**功能描述 : 单次测量函数
**输入参数 ：无
**输出参数 ：测量点数据（未经校正）
********************************************************************************************************/
Lidar_PointTypeDef Meas_SinglePoint(void)
{
    Lidar_PointTypeDef tmp_point;
    TDC_RESTypeDef tdc_res;
    
    tdc_res = TDC_Meas();
    
    tmp_point.distance  = (tdc_res.RES_0 * 37481) >> 16;
    tmp_point.intensity = (tdc_res.RES_1 * 2500 ) >> 16;
    
    return tmp_point;
}


