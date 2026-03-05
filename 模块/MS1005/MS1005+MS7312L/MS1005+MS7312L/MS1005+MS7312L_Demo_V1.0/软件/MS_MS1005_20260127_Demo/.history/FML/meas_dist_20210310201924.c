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
const Lidar_PointTypeDef invalid_point = {InValid_Dist, InValid_Itst}; //无效diao

typedef FlagStatus (*POINT_Analysis)(Lidar_PointTypeDef *point);


