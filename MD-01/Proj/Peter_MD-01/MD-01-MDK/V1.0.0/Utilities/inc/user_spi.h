#ifndef __USER_SPI_H
#define __USER_SPI_H

#ifdef cplusplus
extern "C"
{
#endif

#include "gd32f4xx.h"
#include "wtdint.h"

#define SYNC_PORT			GPIOA
#define SYNC_PIN			GPIO_PIN_11
#define SYNC_GPIO_CLK		RCU_GPIOA

	void SPI_UserInit (void);
	void SPI_WriteDAC (u16_t val);
	void SPI_WriteOled(uint8_t *buf, uint16_t size);
	void SPI_WaitOled (void);

#ifdef cplusplus
}
#endif

#endif
