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
#include "drv_flash.h"

#include "bubbleSort.h"


#define LongTargetValue  493
#define ShortTargetValue 150

typedef struct {
    u16 intensity;
    s16 value;
}CalibrationTypeDef;

const RANGETypedef APDCalibration_IntensityRange  = {350 , 370};  //调整增益时目标靶的强度范围
const RANGETypedef TIM_Pulse_Range  = {50 , 200};                 // APD 增益可调范围
//s16 Compensation[COMPENSATION_SIZE] = {0};
const u16 Standard_Target_Distance = LongTargetValue;

__align(4) CAL_Param cal_param = {1, 2500, HV_TIM_PULSE};

//旋转衰减片测出来的强度值及距离补偿值补偿
static CalibrationTypeDef calibration[250] = {0, 0};


void Wait_Temp_Stabilize(void);
void Send_Command(u8 code);
void MultiSend_Command(u8 code, u8 cnt);
void SendReceive_Command(u8 code);
uint16_t Set_APDGain(RANGETypedef range);
static void Complete_CompensationValue(CalibrationTypeDef *arr, u16 num);
static uint16_t Verify_Calibration_data(int16_t *arr);

/**
 * @brief 进入校正模式
 */
void Goto_Calibration_Mode(void)
{
    uint16_t TIM_Pulse = HV_TIM_PULSE;
    Lidar_PointTypeDef point;
    u16 WhiteTarget_Reflect = 0, StrongTarget_Reflect = 0;
    u16 Calibration_num = 0;
    uint16_t last_intensity = 0;
    u8 Calibration_Step = 0;    


    HV_TIM_SetPFM(HV_TIM_PERIOD);
    HV_TIM_SetPWM(HV_TIM_PULSE);

    delay_ms(10);

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
	printf("\n/*** 第六步： 测500mm白靶强度  ****/\n");
    delay_ms(300);
    SendReceive_Command(0x52);  //测500mm白靶强度
    delay_ms(500); 
    SendReceive_Command(0x43);  //测500mm白靶强度
    delay_ms(500); 
    point = Multi_Meas_Mode(100, CorrectionMode, Calibration_Point_Num);
    WhiteTarget_Reflect = point.intensity;

    /* step7. measure the intensity of strong reflective cloth at the distance of 500mm */
	printf("\n/*** 第七步： 测500mm强反强度  ****/\n");    
    SendReceive_Command(0x4C);  //测500mm强反强度
    delay_ms(500); 
    point = Multi_Meas_Mode(100, CorrectionMode, Calibration_Point_Num);
    StrongTarget_Reflect = point.intensity;
    last_intensity = StrongTarget_Reflect;

    /* step8. rotate optical attenuators to calibrate distance and intensity */
	printf("\n/*** 第八步： 衰减片旋转校正  ****/\n");
    RANGETypedef TargetRange = {500, WhiteTarget_Reflect};//StrongTarget_Reflect
    
    while(3 != Calibration_Step)
    {
        Calibration_Step = 0;
        
        point = Multi_Meas_Mode(100, CorrectionMode, Calibration_Point_Num);

        switch((u8)Compare_Range(point.intensity, TargetRange))
        {
            case LOWSIDE:   //偏低 
                //Calibration_Step = 3; //退出校正 
                MultiSend_Command(0x73, 4);
                break;
            case HIGHSIDE:  //偏高
                if ( point.intensity > StrongTarget_Reflect - 50)//在透明区域
                    MultiSend_Command(0x73, 25);
                else
                    MultiSend_Command(0x73, 8);
                break;
            case INSIDE:    //在范围里                
                MultiSend_Command(0x73, 6);
                break;
            default:
                break;
        }  
        delay_ms(500);
		//delay_ms(500);
        
        Calibration_Step = 1;
        
        point = Multi_Meas_Mode(100, CorrectionMode, Calibration_Point_Num);
        switch((u8)Compare_Range(point.intensity, IntensityRange))
        {
        case LOWSIDE:   //偏低 
            Calibration_Step = 3; //退出校正                 
            break;
        case HIGHSIDE:  //偏高

            break;
        case INSIDE:    //在范围里
            if (point.intensity < last_intensity)
            {
                last_intensity = point.intensity;
                calibration[Calibration_num].intensity = point.intensity;
                calibration[Calibration_num].value = point.distance - Standard_Target_Distance;    
                
                printf("num = %3d, pulse = %4d, compensation = %4d\n", 
                        Calibration_num, calibration[Calibration_num].intensity, calibration[Calibration_num].value);
            
                Calibration_num++;
            }
            break;
        default:
            break;                
        }     
        if(Calibration_num >= 180)
        {
           Calibration_Step = 3; //退出校正  
        }
    }
    
    //校正数据以强度值排序
    Struct_BubbleSort(calibration, Calibration_num, sizeof(CalibrationTypeDef), 
                         0        , sizeof(uint16_t) , 
                         compare_uint16);  
    Complete_CompensationValue(calibration, Calibration_num);//补全校正数据
                         
    //校正数据判断
    if(!Verify_Calibration_data(Compensation))
    {
        while(1)
        {
            Send_Command(0xEE); 
            delay_ms(500);
            printf("校正数据错误\n");
            delay_ms(500); 
            
        }
    }
    
    u16 pagesize = COMPENSATION_SIZE/512;

    Flash_Write_Byte(COMPENSATION_DATA_ADDRESS, uint8_t * data_buffer, uint32_t length);
    for(u16 page_num = 0; page_num < pagesize; page_num++)
    { 
        while(!FLASH_Write_16Page((u16*)Compensation+page_num*512, COMPENSATION_DATA_ADDRESS+page_num*1024, 512))
            ;
    }
    
    if (COMPENSATION_SIZE != pagesize*512)
    {
        while(!FLASH_Write_16Page((u16*)Compensation+pagesize*512, COMPENSATION_DATA_ADDRESS+pagesize*1024, COMPENSATION_SIZE - pagesize*512))
            ;        
    }
	
	cal_param.flag = CAL_COMPLETE_FLAG;
	cal_param.duc  = TIM_Pulse;
	cal_param.temp = Get_Temp_Value();//GetTempValue();
	
	FLASH_Write_16Page((u16*)&cal_param.flag, CALIBRATION_DATA_ADDRESS, sizeof(CAL_Param)/2);
	
	
    printf("\r\nCOMPENSATION DATA has saved to flash!\n\n"); 
    delay_ms(100);
                            
                         
	delay_ms(500);										 
    SendReceive_Command(0x74);  //衰减片复位
    delay_ms(500);
    
    SendReceive_Command(0x52);  //500mm靶归位
    delay_ms(500);                          
    
    SendReceive_Command(0x43);  //测500mm白靶强度
    delay_ms(500); 
    point = Multi_Meas_Mode(100, FixPointMode, Calibration_Point_Num);
    printf("\r\ntest result: dist = %5d, itst = %4d\n", point.distance, point.intensity);                     
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
void MultiSend_Command(u8 code, u8 cnt)
{
    for(int i = 0; i < cnt; i++)
    {
        printf("%c%c%c", CommandHeader, code, (CommandHeader + code) & 0xFF);
        delay_ms(100);
    }
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

static void Complete_CompensationValue(CalibrationTypeDef *arr, u16 num)
{
    int x = 0;
    int cnt  = 0;//the count of number
    s32 index_low = 0, index_high = (s32)arr[cnt].intensity;//intensity
    s32 value_low = (s32)arr[cnt].value, value_high = (s32)arr[cnt].value;
    s32 slope = 0;
    
    for( cnt = 0; cnt < num; cnt++)
    {
        index_high = (s32)arr[cnt].intensity;
        value_high   = (s32)arr[cnt].value;  
        
        if(index_high != index_low)
        {            
           
            slope = (value_high - value_low) * 100 /(index_high - index_low);
            for(x = index_low; x < index_high; x++)
            {
                Compensation[x] = (s16)(value_low + slope*(x - index_low)/100);
                printf(" %4d,  %4d\n", x, Compensation[x]);
            }
        }
        else
        {
            x = index_low;
            Compensation[x] = (s16)(value_low + value_high)/2;
            printf(" %4d,  %4d\n", x, Compensation[x]);
        }
        index_low = index_high; 
        value_low = value_high;          
    }
    
    for(x = index_low; x < IntensityRange.max; x++)
    {
        Compensation[x] = (s16)value_low;
        printf(" %4d,  %4d\n", x, Compensation[x]);
    }
    
    
}
/*检验校正补偿数据是否递减
*/
static uint16_t Verify_Calibration_data(int16_t *arr)
{
    
    for(int i = 1; i < COMPENSATION_SIZE; i++)//正常情况下是递减数组
    {
        if(arr[i] > arr[i-1] + 2) 
        {
            printf("\n校正数据错误：强度：%d  %d\n", i, arr[i]);
            return 0;
        }        
    }
    
    printf("\n校正数据正确！\n");
    return 1;
}