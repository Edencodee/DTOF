
#include "encoder_processing.h"
#include "bsp.h"
#include "delay.h"

#include "tim.h"
#include "comp.h"

#define Using_Cnt
#define UsingCOMP   0

// Capture timer values
uint16_t encoder_proc_capture_old = 0;
// Captured value of timer - encoder event
uint16_t encoder_proc_capture_now = 0;


// Mirror rotation period, in us
uint32_t encoder_proc_rotation_period_us = 0;

//Current speed - rotations per minute
volatile uint16_t current_speed = 0;

// Previous time of zero crossing, DWT ticks
uint32_t encoder_proc_zero_prev_timestamp = 0;

//// Previous encoder event time, ms
//uint32_t encoder_proc_event_timestamp_ms = 0;

// Time between encoder events, in timer ticks
volatile uint16_t enc_period;

//Number of encoder events from zero detection
volatile uint16_t encoder_proc_event_cnt  = 0; 

volatile uint16_t rotation = 0;


uint16_t enc_capt[ENCODER_HOLES_CNT] = {0};


uint8_t encoder_proc_check_zero_cross(uint16_t capture_value);
static uint16_t Encoder_Zero_Correct(uint16_t *time);
static uint8_t  Encoder_Zero_Check(uint16_t *time);
	
//初始化
static void Encoder_GPIO_Init(void);
static void Encoder_Timer_Init(void);
static void Encoder_NVIC_Init(void);

uint16_t tmp_event_cnt  = 0;

void Encoder_Proc_Init(void)
{
#if UsingCOMP
	MX_COMP2_Init();
	MX_TIM15_Init();
	
	 /*## Start the Input Capture in interrupt mode ##########################*/	
  if(HAL_TIM_IC_Start_IT(&htim15, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }	
#else
	
	MX_TIM17_Init();
	 /*## Start the Input Capture in interrupt mode ##########################*/	
  if(HAL_TIM_IC_Start_IT(&htim17, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }		
#endif	
	//Encoder_GPIO_Init();
	//Encoder_Timer_Init();
	//Encoder_NVIC_Init();


}

// Interrupt from timer (connected to encoder comparator)

/**
  * @brief  Input capture callback in non blocking mode 
  * @param  htim : htim handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
	  
    encoder_proc_capture_old = encoder_proc_capture_now;
    encoder_proc_capture_now = HAL_TIM_ReadCapturedValue(htim, ENCODER_TIMER_CH);

    
    if (encoder_proc_capture_now > encoder_proc_capture_old) 
      enc_period = encoder_proc_capture_now - encoder_proc_capture_old;
    else if(encoder_proc_capture_now < encoder_proc_capture_old) 
      enc_period = 0xFFFF - encoder_proc_capture_old + encoder_proc_capture_now + 1;
    else
		{
			/* If capture values are equal, we have reached the limit of frequency
				 measures */
			Error_Handler();
		}
    //encoder_proc_event_cnt++;


		encoder_proc_check_zero_cross(enc_period);
	  //printf("n=%2d,t= %5d\n", encoder_proc_event_cnt, enc_period);

  }// end of interrupt check
	
	
}

// Calculating average period
// If "time" 1.5 times bigger than average value, return 1
uint8_t encoder_proc_check_zero_cross(uint16_t capture_value)
{
#ifdef Using_Time	
	static uint16_t times[4];
	static uint8_t pos = 0;//pos = 0-3
	uint16_t avr_time = 0;//average period
	uint16_t result;

	times[pos] = time;
	pos++;
	pos &= 3;
	avr_time = (times[0] + times[1] + times[2] + times[3]) / 4;
	time = time * 4;
	result = time / avr_time;
	
	if (result >= 5) 
		return 1;
	else 
		return 0;//6/4 = 1.5
#endif

#ifdef Using_Cnt
	
	static uint8_t  encoder_sync_flag = 0;
	
	encoder_proc_rotation_period_us += capture_value;
	
	enc_capt[encoder_proc_event_cnt++] = capture_value;
	
	
	if(encoder_proc_event_cnt >= ENCODER_HOLES_CNT)//旋转一圈
	{
		if(!encoder_sync_flag)
		{
			encoder_proc_event_cnt = ENCODER_HOLES_CNT - Encoder_Zero_Correct(enc_capt);
			encoder_sync_flag = 1;
		}
		else
		{
			encoder_sync_flag = Encoder_Zero_Check(enc_capt);//检测位置是否准确
			
			current_speed = ((60000000)/encoder_proc_rotation_period_us);
			
			encoder_proc_rotation_period_us = 0;
		}
		
		encoder_proc_event_cnt %= ENCODER_HOLES_CNT;
	}
	return encoder_proc_event_cnt;
	
#endif
}

static uint16_t Encoder_Zero_Correct(uint16_t *capture_value)
{
	uint8_t pos = 0;
	
	for(int i = 1; i < ENCODER_HOLES_CNT; i++)
	{
		if (*(capture_value+i) < *(capture_value+pos))
		{
			pos = i;
		}
	}
	//选出最小齿所在标号
	return pos;
	
}
static uint8_t  Encoder_Zero_Check(uint16_t *capture_value)
{
	uint8_t result = 0;
	
	if ((*capture_value) * 5 < (*(capture_value+1) * 2))
	{
		result = 1;
	}
	return result;
}
static void Encoder_GPIO_Init(void)
{
	
}

static void Encoder_Timer_Init(void)
{
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    TIM_ICInitTypeDef  TIM_ICInitStructure;

//	RCC_APB2PeriphClockCmd(ENCODER_TIMER_CLK, ENABLE);

//	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

//	TIM_TimeBaseStructure.TIM_Prescaler = ENCODER_TIMER_PRESCALER;
//	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
//    //Setting Clock Segmentation
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//    ///TIM Upward Counting Mode
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);

//	// channel1 - input capture
//	TIM_ICStructInit(&TIM_ICInitStructure);
//	TIM_ICInitStructure.TIM_Channel = ENCODER_TIMER_CH;
//	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;//TIM_ICPolarity_Rising;//TIM_ICPolarity_BothEdge;
//	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//	TIM_ICInitStructure.TIM_ICFilter = 12;//uart produce some noise
//	TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);

//	TIM_ARRPreloadConfig(ENCODER_TIMER, ENABLE);

//	TIM_ClearITPendingBit(ENCODER_TIMER, ENCODER_TIMER_IT_FLAG);
//	TIM_ITConfig(ENCODER_TIMER, ENCODER_TIMER_IT_FLAG, ENABLE);

//	TIM_Cmd(ENCODER_TIMER, ENABLE);
	
}

static void Encoder_NVIC_Init(void)
{


}



