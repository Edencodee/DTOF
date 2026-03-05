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
 * @brief 读取寄存器 res中 第m开始的n位数, n>0
 * @notice: REG_READ_BIT(res, 3, 2) : 读取res中3：4的值
 */
#define REG_READ_BIT(res, m, n) ((res>>m)&((uint32_t)(pow(2,n) - 1)))
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

    config_res.RFEDGE1      = REG_READ_BIT(res_value, 0 1
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.RFEDGE2      = REG_READ_BIT(res_value, 1 1
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.RESERVED1    = REG_READ_BIT(res_value, 2 1
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.EN_INT       = REG_READ_BIT(res_value, 4 1
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.EN_SEL_TIMO  = REG_READ_BIT(res_value, 7 1
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.NEG_START    = REG_READ_BIT(res_value, 8 1
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.NEG_STOP1    = REG_READ_BIT(res_value, 9 1
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.NEG_STOP2    = REG_READ_BIT(res_value, 101
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.RESERVED2    = REG_READ_BIT(res_value, 111
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.NO_CAL_AUTO  = REG_READ_BIT(res_value, 121
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.CALIBRATE    = REG_READ_BIT(res_value, 131
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.SEL_TSTO2    = REG_READ_BIT(res_value, 141
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.EN_FAST_INIT = REG_READ_BIT(res_value, 161
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.EN_ERR_VAL   = REG_READ_BIT(res_value, 171
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.DOUBLE_RES   = REG_READ_BIT(res_value, 181
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.START_CLKHS  = REG_READ_BIT(res_value, 191
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.DIV_CLKHS    = REG_READ_BIT(res_value, 201
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.SEL_TIMO     = REG_READ_BIT(res_value, 221
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
    config_res.HITIN1       = REG_READ_BIT(res_value, 241
1
2
3
1
1
1
1
1
1
1
2
1
1
1
1
2
2
4
4, n);
}
