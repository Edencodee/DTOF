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

uint16_t Compensation[COMPENSATION_SIZE] = {0};
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

/********************************************************************************************************
**函数信息 ：Get_RevisedPoint(Lidar_PointTypeDef *point)                      
**功能描述 : 对测量点数据进行修正、补偿及筛选
**输入参数 ：point:测量点数据
**输出参数 ：result_flag: 测量值是否有效
**备注     ：
********************************************************************************************************/
FlagStatus Get_RevisedPoint(Lidar_PointTypeDef *point)
{
    FlagStatus result_flag = RESET;

    //强度在范围里
    if (INSIDE == Compare_Range(point->intensity, IntensityRange))
    {
        //距离校正+温度补偿
        point->distance = point->distance - Compensation[point->intensity];// - config.Compensation[point->intensity] + 50;

        if (INSIDE == Compare_Range(point->distance,  DistanceRange))
        {
            result_flag = SET;
        }
    }

    if (!result_flag)
    {
        point->distance  = InValid_Dist;
        point->intensity = InValid_Itst;     
    }
    return result_flag;
}

/********************************************************************************************************
**函数信息 ：Judge_PointValidity(Lidar_PointTypeDef *point)                      
**功能描述 : 判断测量数据是否有效
**输入参数 ：*point
**输出参数 ：result_flag: 测量值是否有效
**备注     ：SET : valid point; RESET: invalid point
********************************************************************************************************/
FlagStatus Judge_PointValidity(Lidar_PointTypeDef *point)
{
    FlagStatus result_flag = RESET;

    if ((INSIDE == Compare_Range(point->distance,  DistanceRange)) &&
        (INSIDE == Compare_Range(point->intensity, IntensityRange)))
    {
        result_flag = SET;
    }
    
    return result_flag;
}
/********************************************************************************************************
**函数信息 ：Point_Process(Lidar_PointTypeDef *point, u8 point_num)                      
**功能描述 : 测量点分析
**输入参数 ：point：测量点地址
**输入参数 ：point_num：测量点数目
**输入参数 ：len: 写入数据长度, 小于512
**输出参数 ：valid_point_num：有效点数目
********************************************************************************************************/
u8 Point_Screening(Lidar_PointTypeDef *point, u8 point_num, POINT_Analysis point_analysis)
{
    uint8_t valid_point_num = 0;
    Lidar_PointTypeDef *p_point = point; 
    //Lidar_PointTypeDef tmp_point;    
    
    for(int i = 0; i < point_num; i++)
    {
        if (point_analysis(p_point))
        {
            *(point + valid_point_num) = *p_point;
            valid_point_num++;
        }
        p_point++;
    }
  
    return valid_point_num;
}

Lidar_PointTypeDef Point_Average(Lidar_PointTypeDef *point, u8 point_num)
{
    uint32_t meas_dist_sum = 0, meas_itst_sum = 0;
    Lidar_PointTypeDef tmp_point;
    
    if(!point_num)
    {
        return invalid_point;
        return tmp_point;
    }
    for (int i = 0; i < point_num; i++)
    {
        meas_dist_sum += point->distance;
        meas_itst_sum += point->intensity;        
    }
    
    tmp_point.distance  = meas_dist_sum/point_num;
    tmp_point.intensity = meas_itst_sum/point_num;    

    return tmp_point;    
}
/********************************************************************************************************
**函数信息 ：Compare_Range(u16 value, const RANGETypedef range)                      
**功能描述 : 数据范围界定
**输入参数 ：value: 数据值
**输入参数 ：range：界定范围
**输出参数 ：LOWSIDE:数据小于界定范围；
			HIGHSIDE:数据大于界定范围；
            INSIDE： 数据在界定范围里。
********************************************************************************************************/
RangeCompareTypedef Compare_Range(uint16_t value, const RANGETypedef range)
{
    if (value < range.min)
    {
       return LOWSIDE;
    } 
    else if (value >= range.max)   //[, )
    {
        return HIGHSIDE;
    }
    else   // [,)
    {
        return INSIDE;
    }
}
