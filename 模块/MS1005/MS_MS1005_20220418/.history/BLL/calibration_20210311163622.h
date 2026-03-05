/**
 * @file calibration.h
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

#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include "bsp.h"


#define CommandHeader   0xFA

typedef struct
{
	u16 flag;	
	s16 temp;
	u16 duc;
}CAL_Param;

void SendReceive_Command(u8 code);
void Goto_Calibration_Mode(void);

#endif /* __CALIBRATION_H */

