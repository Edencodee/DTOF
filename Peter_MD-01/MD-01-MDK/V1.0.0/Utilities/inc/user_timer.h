#ifndef __USER_TIMER_H
#define __USER_TIMER_H

#ifdef cplusplus
extern "C"
{
#endif

#include "gd32f4xx.h"

	#define LD_TMR		TIMER1
	
	void TIMER_UserInit(void);

#ifdef cplusplus
}
#endif

#endif /* USER_TIMER_H */
