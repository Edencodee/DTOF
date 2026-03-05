#ifndef __SYSTICK_H
#define __SYSTICK_H

#ifdef cplusplus
extern "C" {
#endif

#include <stdint.h>

	/* configure systick */
	void systick_config(void);
	/* delay a time in milliseconds */
	void delay_1ms(uint32_t count);
	/* delay decrement */
	void delay_decrement(void);

#ifdef cplusplus
}
#endif

#endif /* SYSTICK_H */
