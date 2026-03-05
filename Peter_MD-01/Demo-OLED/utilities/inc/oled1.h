#ifndef __OLED_H
#define __OLED_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"

typedef union {
	uint32_t word;
	uint8_t buf[4];
} u32_t;

#define OLED_CS_CLR()	GPIO_BC	(GPIOA) = GPIO_PIN_13;
#define OLED_CS_SET()	GPIO_BOP(GPIOA) = GPIO_PIN_13;
#define OLED_DC_CLR()	GPIO_BC	(GPIOA) = GPIO_PIN_10;
#define OLED_DC_SET()	GPIO_BOP(GPIOA) = GPIO_PIN_10;
#define OLED_DIN_CLR()	GPIO_BC	(GPIOC) = GPIO_PIN_9 ;
#define OLED_DIN_SET()	GPIO_BOP(GPIOC) = GPIO_PIN_9 ;
#define OLED_CLK_CLR()	GPIO_BC	(GPIOA) = GPIO_PIN_8 ;
#define OLED_CLK_SET()	GPIO_BOP(GPIOA) = GPIO_PIN_8 ;

extern const uint8_t bojke[];

void OLED_Fill(uint8_t val);
// void OLED_CLS(void);
void OLED_Init(void);
//void OLED_6x8Str(uint8_t x, uint8_t y, char *str);
void OLED_8x16Str(uint8_t x, uint8_t y, char *str);
void OLED_16x16CN(uint8_t x, uint8_t y, uint16_t *cn, int size);
void OLED_12x24Str(uint8_t x, uint8_t y, char *str);
void OLED_24x24CN(uint8_t x, uint8_t y, uint16_t *cn, int size);
void OLED_16x32Str(uint8_t x, uint8_t y, char *str);
void OLED_32x32CN(uint8_t x, uint8_t y, uint16_t *cn, int size);
void OLED_Wave(uint16_t *wave);
void OLED_FullBmp(const uint8_t *bmp);

#ifdef cplusplus
}
#endif

#endif
