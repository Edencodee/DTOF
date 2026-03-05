#ifndef __USER_TIMER_H
#define __USER_TIMER_H

#ifdef cplusplus
extern "C"
{
#endif

#include "gd32f4xx.h"

	void TIMER_ReConfig (float intTime, uint16_t period); 
	void TIMER_UserInit (uint16_t intTime, uint16_t period);

#ifdef cplusplus
}
#endif

#endif /* USER_TIMER_H */
