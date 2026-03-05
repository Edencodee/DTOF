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

/*
#define REG_READ_BIT(value, m, n)
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
 * @brief TDC_CONFIG_RES 打包成 uint32_t
 */
uint32_t TDC_Config_Res_Package(TDC_CONFIG_RES res)
{
    uint32_t res_value = 0x00;

    res_value |= (res.RFEDGE1      << 0 );
    res_value |= (res.RFEDGE2      << 1 );
    res_value |= (res.RESERVED1    << 2 );
    res_value |= (res.EN_INT       << 4 );
    res_value |= (res.EN_SEL_TIMO  << 7 );
    res_value |= (res.NEG_START    << 8 );
    res_value |= (res.NEG_STOP1    << 9 );
    res_value |= (res.NEG_STOP2    << 10);
    res_value |= (res.RESERVED2    << 11);
    res_value |= (res.NO_CAL_AUTO  << 12);
    res_value |= (res.CALIBRATE    << 13);
    res_value |= (res.SEL_TSTO2    << 14);
    res_value |= (res.EN_FAST_INIT << 16);
    res_value |= (res.EN_ERR_VAL   << 17);
    res_value |= (res.DOUBLE_RES   << 18);
    res_value |= (res.START_CLKHS  << 19);
    res_value |= (res.DIV_CLKHS    << 20);
    res_value |= (res.SEL_TIMO     << 22);
    res_value |= (res.HITIN1       << 24);

    return res_value;
}
/**
 * @brief 将uint32_t 解析为 TDC Config Res
 * @param  res_value:
 * @return TDC_CONFIG_RES 
 */
TDC_CONFIG_RES TDC_Config_Res_Parse(uint32_t res_value)
{
    TDC_CONFIG_RES config_res;

    config_res.RFEDGE1      = (res_value >> 0 )
    config_res.RFEDGE2      = (res_value >> 1 )
    config_res.RESERVED1    = (res_value >> 2 )
    config_res.EN_INT       = (res_value >> 4 )
    config_res.EN_SEL_TIMO  = (res_value >> 7 )
    config_res.NEG_START    = (res_value >> 8 )
    config_res.NEG_STOP1    = (res_value >> 9 )
    config_res.NEG_STOP2    = (res_value >> 10)
    config_res.RESERVED2    = (res_value >> 11)
    config_res.NO_CAL_AUTO  = (res_value >> 12)
    config_res.CALIBRATE    = (res_value >> 13)
    config_res.SEL_TSTO2    = (res_value >> 14)
    config_res.EN_FAST_INIT = (res_value >> 16)
    config_res.EN_ERR_VAL   = (res_value >> 17)
    config_res.DOUBLE_RES   = (res_value >> 18)
    config_res.START_CLKHS  = (res_value >> 19)
    config_res.DIV_CLKHS    = (res_value >> 20)
    config_res.SEL_TIMO     = (res_value >> 22)
    config_res.HITIN1       = (res_value >> 24)
}
