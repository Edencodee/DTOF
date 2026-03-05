

#include "delay.h"


static uint32_t fac_us=0;							//us延时倍乘数
static uint32_t fac_ms=0;							//us延时倍乘数


/*
 *  初始化延迟函数
 *  SYSTICK的时钟固定为AHB时钟的1/8
 *  SYSCLK:系统时钟频率
 */
void delay_init(uint8_t SYSCLK)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK

	fac_us = SYSCLK;						    //不论是否使用OS,fac_us都需要使用
  fac_ms = fac_us * 1000;  
}	


void delay_us(uint32_t nus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};	
}

void delay_ms(uint16_t nms)
{
	uint16_t ms_cnt = 0;
	
	for (ms_cnt = 0; ms_cnt < nms; ms_cnt++)
	{
		delay_us(1000);
	}
}

