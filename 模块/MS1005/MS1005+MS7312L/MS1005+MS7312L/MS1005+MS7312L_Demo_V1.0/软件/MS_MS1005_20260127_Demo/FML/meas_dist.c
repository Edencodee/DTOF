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

#include "stm32g4xx_hal.h" // SystemCoreClock
#include "core_cm4.h"      // DWT/CORE debug registers
#include <stdint.h>
#include <stdio.h>
#include <math.h>   // add for sqrt

#define TDC_Debug_Mode     0//TDC 调试模式

static Lidar_PointTypeDef meas_point[MAX_POINT_NUM];

const RANGETypedef IntensityRange = {MIN_Intensity, MAX_Intensity};    //强度范围
const RANGETypedef DistanceRange  = {MIN_Distance,  MAX_Distance};     //距离范围
const Lidar_PointTypeDef invalid_point = {InValid_Dist, InValid_Itst}; //无效点

typedef FlagStatus (*POINT_Analysis)(Lidar_PointTypeDef *point);

int16_t Compensation[COMPENSATION_SIZE] = {0};

// 简单的DWT计时器启用，供高精度耗时统计使用
static void dwt_counter_enable(void)
{
	static uint8_t enabled = 0;
	if (!enabled) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 使能DWT
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
		enabled = 1;
	}
}

// ====== 测量耗时封装与宏开关 ======
// 将 MEAS_TIME_PROFILE 设为 1 开启测量耗时打印；设为 0 则完全关闭（零开销）
#ifndef MEAS_TIME_PROFILE
#define MEAS_TIME_PROFILE 1
#endif

typedef struct {
    uint32_t start;
} meas_prof_t;

#if MEAS_TIME_PROFILE
static inline void meas_prof_begin(meas_prof_t *p)
{
    dwt_counter_enable();
    p->start = DWT->CYCCNT;
}

static inline void meas_prof_end_log(meas_prof_t *p, const char *tag)
{
    uint32_t end = DWT->CYCCNT;
    uint32_t cycles = end - p->start;
    uint32_t us = (uint32_t)(((uint64_t)cycles * 1000000ULL) / SystemCoreClock);
    SEGGER_RTT_printf(0, "%s: %lu cycles, %lu us\r\n", tag, (unsigned long)cycles, (unsigned long)us);
}
#else
static inline void meas_prof_begin(meas_prof_t *p) { (void)p; }
static inline void meas_prof_end_log(meas_prof_t *p, const char *tag) { (void)p; (void)tag; }
#endif
// ====== 结束 ======


/**
 * @brief  单次测量函数
 * @return 原始测量点数据
 */
Lidar_PointTypeDef Meas_SinglePoint(void)
{
	Lidar_PointTypeDef tmp_point;
	TDC_RESTypeDef tdc_res;
#if MEAS_TIME_PROFILE
	meas_prof_t prof;
	meas_prof_begin(&prof);
#endif
	tdc_res = TDC_Meas();
	tmp_point.distance  = ((tdc_res.RES_0 * 37481) >> 16); //dist mm
	tmp_point.intensity = ((float)tdc_res.RES_1 / 65536 * 2500); //PW:100PS
#if MEAS_TIME_PROFILE
	meas_prof_end_log(&prof, "Meas_SinglePoint");
#endif
	return tmp_point;
}

/**
 * @brief  基于1秒窗口统计测距帧率与平均单次耗时
 * @param  out_count: 1秒内完成的测量次数（可为NULL忽略）
 * @param  out_avg_us: 平均单次耗时（微秒，基于DWT周期换算，可为NULL忽略）
 */
void Meas_Stats_1s(uint32_t *out_count, uint32_t *out_avg_us)
{
	uint32_t window_cycles;
	uint64_t sum_cycles = 0;
	uint32_t count = 0;

	dwt_counter_enable();
	uint32_t start_cycles = DWT->CYCCNT;

	while (1) {
		uint32_t call_start = DWT->CYCCNT;
		Meas_SinglePoint();
		uint32_t call_end = DWT->CYCCNT;

		sum_cycles += (uint32_t)(call_end - call_start);
		count++;

		uint32_t now = DWT->CYCCNT;
		window_cycles = now - start_cycles;
		if (window_cycles >= SystemCoreClock) { // 约1秒窗口
			break;
		}
	}

	if (out_count) {
		*out_count = count;
	}

	if (out_avg_us) {
		// 平均单次耗时 = (sum_cycles / count) * 1e6 / SystemCoreClock
		uint32_t avg_cycles = (count == 0) ? 0 : (uint32_t)(sum_cycles / count);
		*out_avg_us = (uint32_t)(((uint64_t)avg_cycles * 1000000ULL) / SystemCoreClock);
	}

	// 默认直接输出结果，便于串口/RTT调试
	SEGGER_RTT_printf(0, "Meas_Stats_1s: count=%lu, avg=%lu us\r\n",
				  (unsigned long)count,
				  (unsigned long)((count == 0) ? 0 : (uint32_t)(((uint64_t)(sum_cycles / count) * 1000000ULL) / SystemCoreClock)));
}

void Meas_Stats_1s_Print(void)
{
	Meas_Stats_1s(NULL, NULL);
}

// 简单重复精度测试：输入测距次数（0~20000），逐次输出每次测距值（RTT通道1），
// 最后在RTT通道0输出有效样本的均值/最小/最大。
void Meas_Repeat_Simple(uint32_t cnt)
{
	if (cnt > 20000U) cnt = 20000U;

	uint64_t sum = 0;
	uint32_t valid = 0;
	uint32_t min_v = 0xFFFFFFFFu;
	uint32_t max_v = 0u;

	for (uint32_t i = 0; i < cnt; i++) {
		Lidar_PointTypeDef p = Meas_SinglePoint();
		SEGGER_RTT_printf(0, "%d\r\n", p.distance);

		if (p.distance != InValid_Dist) {
			sum += p.distance;
			valid++;
			if (p.distance < min_v) min_v = p.distance;
			if (p.distance > max_v) max_v = p.distance;
		}
	}

	uint32_t mean_v = (valid == 0) ? 0 : (uint32_t)(sum / valid);
	SEGGER_RTT_printf(0, "RepeatSimple: req=%lu, valid=%lu, mean=%lu, min=%lu, max=%lu\r\n",
					  (unsigned long)cnt,
					  (unsigned long)valid,
					  (unsigned long)mean_v,
					  (unsigned long)((valid == 0) ? 0 : min_v),
					  (unsigned long)((valid == 0) ? 0 : max_v));
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


