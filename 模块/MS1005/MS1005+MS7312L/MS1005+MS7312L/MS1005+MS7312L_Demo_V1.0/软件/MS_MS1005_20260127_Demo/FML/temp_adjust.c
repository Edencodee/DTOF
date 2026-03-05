

#include "temp_adjust.h"
#include "meas_dist.h"
#include "cmu.h"
#include "main.h"
#include "delay.h"
#include "uart.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "flash.h"
#include "board_abstract.h"
#include "calibration.h"

void ApdHV_TempAdjust(void)
{
	int16_t CurTemp = GetTempValue();
	uint16_t channelpulse = cal_param.duc + ((CurTemp - cal_param.temp) >> 5);

	if(channelpulse > 250)
		channelpulse = 250;

	HV_TIM_SetPWM(channelpulse);
}