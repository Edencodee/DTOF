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

#include "drv_temp.h"
#include "tdc_driver.h"
#include "meas_dist.h"
#include "tim.h"
#include "drv_usart.h"
#include "main.h"



const RANGETypedef APDCalibration_IntensityRange  = {350 , 370};  //调整增益时目标靶的强度范围
const RANGETypedef TIM_Pulse_Range  = {50 , 200};                 // APD 增益可调范围


void Wait_Temp_Stabilize(void);


/**
 * @brief 进入校正模式
 */
void Goto_Calibration_Mode(void)
{

    HV_TIM_SetPFM(HV_TIM_PERIOD);
    HV_TIM_SetPWM(HV_TIM_PULSE);    
    /* step1. wait for the temperature to stabilize */
    printf("\n/*** 第一步： 等待温度稳定  ****/\n");
    Wait_Temp_Stabilize();
    delay_ms(10);

    /* step2. reset optical attenuators */
    printf("\n/*** 第二步： 衰减片电机复位  ****/\n");
    SendReceive_Command(0x74);  //衰减片复位
    delay_ms(500);
	
    /* step3. reset 500mm Target */
    printf("\n/*** 第三步： 500mm靶归位  ****/\n");
    SendReceive_Command(0x52);  //500mm靶归位
    delay_ms(500);

    /* step4. reset 150mm Target */
    printf("\n/*** 第四步： 150mm靶归位  ****/\n");
//    SendReceive_Command(0x61);  //150mm靶归位
    delay_ms(500); 

    /* step5. adjust the gain of APD */
    printf("\n/*** 第五步： 调整高压  ****/\r\n");
    TIM_Pulse = Set_APDGain(APDCalibration_IntensityRange);//调整高压
    printf("\r\nHV_PWM = %d\n", TIM_Pulse);
    /* step6. measure the intensity of white paper at the distance of 500mm */

    /* step7. measure the intensity of strong reflective cloth at the distance of 500mm */

    /* step8. rotate optical attenuators to calibrate distance and intensity */

    /* step9. verify calibration data */
}

void Wait_Temp_Stabilize(void)
{
    int16_t current_temp = Get_Temp_Value();
    int16_t max_temp = current_temp, min_temp = current_temp;
    Lidar_PointTypeDef tmp_point;

    for (int cnt = 0; cnt < 10; cnt++)
    {
        max_temp = 2000;
        min_temp = 1000;
        while(max_temp < min_temp + 10)
        {
            current_temp = Get_Temp_Value();

            max_temp = current_temp;
            min_temp = current_temp;

            for (int i = 0; i < 10; i++)
            {
                current_temp = Get_Temp_Value();

                max_temp = MAX(max_temp, current_temp);
                min_temp = MIN(min_temp, current_temp);

                for (int meas_cnt = 0; meas_cnt < 500; meas_cnt++)
                {
                    tmp_point = Meas_Mode(CorrectionMode, Calibration_Point_Num);
                    delay_us(100);                
                }
                printf("now_temp = %5d, max_temp = %5d, min_temp = %5d, dist = %d, itst = %d\n",
                                current_temp, max_temp, min_temp,tmp_point.distance, tmp_point.intensity);
            }
        }
    }
}
/**
 * @brief 调整 APD 增益
 * @param  range:  目标靶强度范围
 * @return uint16_t 
 */
uint16_t Set_APDGain(RANGETypedef range)
{
   Lidar_PointTypeDef tmp_point;
   u16 tmp_HV_TIM_PULSE = HV_TIM_PULSE, tmp_HV_TIM_PERIOD = HV_TIM_PERIOD;
   u8 result = 0;
   
   while(!result)
   {
       tmp_point = Meas_Mode(CorrectionMode, Calibration_Point_Num);
       
       printf("\r\nR2 = %4d, Pulse = %4d\n", tmp_point.intensity, tmp_HV_TIM_PULSE);
       
       switch((u8)Compare_Range(tmp_point.intensity, APDCalibration_IntensityRange))
       {
           case LOWSIDE:   //强度偏低 
               tmp_HV_TIM_PULSE++;                  
               break;
           case HIGHSIDE:  //强度偏高
               tmp_HV_TIM_PULSE--;
               break;
           case INSIDE:    //强度在范围里                
               result = 1;
               break;
           default:
               break;
       }
       if (INSIDE == Compare_Range(tmp_HV_TIM_PULSE, TIM_Pulse_Range))
       {
           HV_TIM_SetPWM(tmp_HV_TIM_PULSE);
       }
       else//error
       {
           printf("%c%c%c", 0xFA, 0xE6, 0xE0); // Error //FA E6 E0 = Err06
       }
       delay_ms(500);    
   }
   
   return tmp_HV_TIM_PULSE;
}

void Send_Command(u8 code)
{
    printf("%c%c%c", CommandHeader, code, (CommandHeader + code) & 0xFF);
}
uint8_t command_rx_buffer[64] = {0};
uint8_t UART_RX_Command(void)
{
    uint16_t command_code = 0xFF;
    uint16_t command_len = 0;
    if (SET == SPAD_Uart_Rx_Flag )
    {
        SPAD_Uart_Rx_Flag = RESET;
        command_len = __kfifo_get(&spad_fifo, command_rx_buffer, SPAD_RX_SIZE);

        for (uint16_t i = 0; i + 3 <= command_len; i++)
        {
            if (CommandHeader == command_rx_buffer[i])
            {
                if (command_rx_buffer[i+2] == ((command_rx_buffer[i] + command_rx_buffer[i+1])&0xFF))
                {
                    command_code = command_rx_buffer[i + 1];
                    //printf("\ncommand code = 0x%x\n", command_code);
                    break;
                }
            }
        }
    }

    return command_code;
}
void SendReceive_Command(u8 code)
{
    uint16_t timeout = 5000;
    uint8_t Command_RS232 = 0x00;

    Send_Command(code);

    while (1)
    {
		Command_RS232 = UART_RX_Command();
		if (Command_RS232 == code)
		{
			Command_RS232 = 0;
			// printf("%c%c%c",0xFA,0xCC,0xFA+0xCC);
			delay_ms(300);
			break;
		}
 
       delay_ms(1);
       
       if(!(timeout--))
       {
           timeout = 5000;
           Send_Command(code);
       }
   }
}
