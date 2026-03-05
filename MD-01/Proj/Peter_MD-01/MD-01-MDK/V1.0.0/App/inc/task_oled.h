#ifndef __TASK_OLED_H
#define __TASK_OLED_H

#ifdef cplusplus
extern "C" {
#endif

	#include "main.h"
	
	extern __IO bool g_isSetByKey;
	
	void MenuInit   (void);
	void OledTask   (uint16_t* p);
	bool OledChkBsy (void);
	void BtnTask    (void);
	void InputTask	(void);
	
	/* member function */
	void ResetWaveType(void);
	int32_t getThre(int group, int idx);
	int32_t getCurrThre(int group);
	
	/* counter increase function, return true when over max */
	bool LoopCntInc(int32_t *cnt, int32_t max);
	bool CntInc(int32_t *cnt, int32_t max);

#ifdef cplusplus
}
#endif

#endif
