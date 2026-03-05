/**
 * @file calibration.c
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
 * <tr><td>2021-03-10 <td>1.0     <td>jinf    <td>lidar data calibration
 * </table>
 */

#include "calibration.h"


#include 
/**
 * @brief 进入校正模式
 */
void Goto_Calibration_Mode(void)
{
    /* step1. wait for the temperature to stabilize */
    Wait_Temp_Stabilize();

    /* step2. reset optical attenuators */

    /* step3. reset 500mm Target */

    /* step4. reset 150mm Target */

    /* step5. adjust the gain of APD */

    /* step6. measure the intensity of white paper at the distance of 500mm */

    /* step7. measure the intensity of strong reflective cloth at the distance of 500mm */

    /* step8. rotate optical attenuators to calibrate distance and intensity */

    /* step9. verify calibration data */
}

void Wait_Temp_Stabilize(void)
{
    int16_t current_temp = Get_Temp_Value();
}