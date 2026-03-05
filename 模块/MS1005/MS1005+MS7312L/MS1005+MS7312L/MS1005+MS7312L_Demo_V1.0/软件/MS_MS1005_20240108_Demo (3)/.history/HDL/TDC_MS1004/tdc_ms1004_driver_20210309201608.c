/**
 * @file tdc_ms1004_driver.c
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

#include "tdc_ms1004_driver.h"

/**
 * @brief 设置 TDC 配置寄存器的值
 * @return config_res： 配置寄存器的值 
 */
TDC_CONFIG_RES Set_TDC_Config_Res(void)
{
    TDC_CONFIG_RES config_res;

    config_res.RFEDGE1 = STOP_TRIGGER_MODE_UP_OR_DOWN;
    config_res.RFEDGE2 = STOP_TRIGGER_MODE_UP_OR_DOWN;

    config_res.RESERVED1 = 0;

    config_res.EN_INT = TIMEOUT_INIT;

    config_res.EN_SEL_TIMO = TDC_FUNCTION_OPEN;

    return config_res;
}

/**
 * @brief TDC_CONFIG_RES 转换成 uint32_t
 */
uint32_t TDC_Config_Res_Package(TDC_CONFIG_RES res)
{
    uint32_t res_value = 0x00;

    res_value |= (res.RFEDGE1 << 0);
    res_value |= (res.RFEDGE2 << 1);
    res_value |= (res.RESERVED1 << 0);

}
