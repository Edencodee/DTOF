/**
 * @file meas_dist.c
 * @brief
 * @author luoyinlong
 * @version 1.0
 * @date 2022/4/24
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

int16_t Compensation[COMPENSATION_SIZE] = {0};

/**
 * @brief  单次测量函数
 * @return 原始测量点数据
 */
Lidar_PointTypeDef Meas_SinglePoint(void)
{
	Lidar_PointTypeDef tmp_point;
	//TDC_RESTypeDef tdc_res;

	//Eden
	TDC_RESULTTypeDef tdc_result;

	tdc_result = TDC_Meas();
//   tmp_point.intensity = (tdc_res.RES_1 * 2500 ) >> 16;   //100ps
//
//	if( tmp_point.intensity >5000)//500nS  calibration mode :4M T=250ns*2
//
//	   {
//			 tmp_point.intensity =0;
//			  tmp_point.distance =65535;
//			 return tmp_point;
//		 }


	//tmp_point.distance  = ((tdc_res.RES_0 * 37481) >> 16); //dist mm
	//tmp_point.intensity  = ((float)tdc_res.RES_1 / 65536 * 2500); //PW:100PS

	//Eden
	//tmp_point.distance  = ((tdc_result.Result_0 * 37481) >> 16); //dist mm
	//tmp_point.distance  = (tdc_result.Result_0  * 1500.0f); //dist mm
	//tmp_point.intensity  = ((float)tdc_result.Result_1 / 65536 * 2500); //PW:100PS
	//SEGGER_RTT_printf(0,"Dist = %.3f   PW = %.3f\r\n",tdc_result.Result_0,tdc_result.Result_1);
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
	if (INSIDE == Compare_Range(point->intensity, IntensityRange)) {
		//距离校正+温度补偿
		point->distance = point->distance - Compensation[point->intensity];// - config.Compensation[point->intensity] + 50;

		if (INSIDE == Compare_Range(point->distance,  DistanceRange)) {
			result_flag = SET;
		}
	}

	if (!result_flag) {
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
	    (INSIDE == Compare_Range(point->intensity, IntensityRange))) {
		result_flag = SET;
	}

	return result_flag;
}

/**
 * @brief 测量点筛选
 * @param  point: 待筛选测量点首地址
 * @param  point_num: 待筛选测量点数目
 * @param  point_analysis: 筛选方法
 * @return valid_point_num: 筛选后数组中有效点数目
 */
uint8_t Point_Screening(Lidar_PointTypeDef *point, u8 point_num, POINT_Analysis point_analysis)
{
	uint8_t valid_point_num = 0;
	Lidar_PointTypeDef *p_point = point;
	//Lidar_PointTypeDef tmp_point;

	for(int i = 0; i < point_num; i++) {
		if (point_analysis(p_point)) {
			*(point + valid_point_num) = *p_point;
			valid_point_num++;
		}
		p_point++;
	}

	return valid_point_num;
}
/**
 * @brief 测量点平均
 * @param  point: 测量点起始地址
 * @param  point_num: 测量点数目
 * @return tmp_point: 平均后的测量点
 */
Lidar_PointTypeDef Point_Average(Lidar_PointTypeDef *point, u8 point_num)
{
	uint32_t meas_dist_sum = 0, meas_itst_sum = 0;
	Lidar_PointTypeDef tmp_point;

	if(!point_num) {
		return invalid_point;
	}
	for (int i = 0; i < point_num; i++) {
		meas_dist_sum += point->distance;
		meas_itst_sum += point->intensity;
	}

	tmp_point.distance  = meas_dist_sum / point_num;
	tmp_point.intensity = meas_itst_sum / point_num;

	return tmp_point;
}

/********************************************************************************************************
**函数信息 ：Meas_Point(u8 point_num)
**功能描述 : 多次测量
**输入参数 ：point_num 测量点数
**输出参数 ：point_num 测量点数
**备注     ：校正、正常测距时都调用此函数
********************************************************************************************************/
u8 Meas_Point(Lidar_PointTypeDef *point, u8 point_num)
{
	if(point_num > MAX_POINT_NUM) {
		point_num = MAX_POINT_NUM;
#if Debug
		printf("Meas_Point Error!point_num is too long!\n");
#endif
	}
	for( int i = 0; i < point_num; i++) {
		*(point++) = Meas_SinglePoint();

		//如果SPI使用硬件方式（速度慢），不需delay
#if USE_SPI_SOFT
		meas_delay(150);
#endif
	}

	return point_num;
}
/********************************************************************************************************
**函数信息 ：Meas_Mode(u8 mode_selection, u8 point_num)
**功能描述 : 测量模式
**输入参数 ：mode_selection：测量模式选择
**输入参数 ：point_num：单个测量点测量次数
**输出参数 ：测量点
********************************************************************************************************/
Lidar_PointTypeDef Meas_Mode(u8 mode_selection, u8 point_num)
{
	Lidar_PointTypeDef tmp_point;
	u8 valid_point_num;

	switch(mode_selection) {
	case CorrectionMode://校正模式
		Meas_Point(meas_point, point_num);//测量多个点
		//获取有效数据个数
		valid_point_num = Point_Screening(meas_point, point_num, Judge_PointValidity);
		//数据点平均
		tmp_point = Point_Average(meas_point, valid_point_num);

		break;
	case FixPointMode://单点模式
		Meas_Point(meas_point, point_num);//测量多个点
		valid_point_num = Point_Screening(meas_point, point_num, Get_RevisedPoint);
		tmp_point = Point_Average(meas_point, valid_point_num);
		break;
	case RotationMode://旋转模式
		break;
	default:
		break;
	}

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
	if (value < range.min) {
		return LOWSIDE;
	} else if (value >= range.max) { //[, )
		return HIGHSIDE;
	} else { // [,)
		return INSIDE;
	}
}

/********************************************************************************************************
**函数信息 ：Multi_Meas_Mode(u8 meas_cnt, u8 mode_selection, u8 point_num)
**功能描述 : 多点测量取平均值
**输入参数 ：meas_cnt: 测量数据点个数
**输入参数 ：mode_selection : 测量模式
**输入参数 ：point_num: 单个测量数据点测量次数
**输出参数 ：meas_cnt个测量点平均值
********************************************************************************************************/
Lidar_PointTypeDef Multi_Meas_Mode(u8 meas_cnt, u8 mode_selection, u8 point_num)
{
	u32 meas_dist_sum = 0, meas_itst_sum = 0;
	Lidar_PointTypeDef tmp_point;
	u8 valid_point_num = 0;

	for (int i = 0; i < meas_cnt; i++) { //只测meas_cnt次
		tmp_point = Meas_Mode(mode_selection, point_num);

		if (Judge_PointValidity(&tmp_point)) {
			meas_dist_sum += tmp_point.distance;
			meas_itst_sum += tmp_point.intensity;
			valid_point_num++;
		}
	}
	if(!valid_point_num) {
		return invalid_point;
	}

	tmp_point.distance  = meas_dist_sum / valid_point_num;
	tmp_point.intensity = meas_itst_sum / valid_point_num;

	return tmp_point;
}

