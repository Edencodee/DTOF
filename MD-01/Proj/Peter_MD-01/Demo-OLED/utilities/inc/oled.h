#ifndef __OLED_H
#define __OLED_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"
//#include "user_flash.h"

typedef enum {
	kRTriAgl = 0,
	kLTriAgl,
	kBar,
	kDash
} graph_t;

#define OLED_CS_CLR()	GPIO_BC	(OLED_CS_PORT) = OLED_CS_PIN;
#define OLED_CS_SET()	GPIO_BOP(OLED_CS_PORT) = OLED_CS_PIN;
#define OLED_DC_CLR()	GPIO_BC	(OLED_DC_PORT) = OLED_DC_PIN;
#define OLED_DC_SET()	GPIO_BOP(OLED_DC_PORT) = OLED_DC_PIN;

extern const uint8_t hm[];
extern const uint8_t lock[];
extern const uint8_t unlock[];

void OLED_Init(void);
void OLED_8x16StrUp2Buf	(uint8_t x, uint8_t y, char *str);
void OLED_8x16StrDn2Buf	(uint8_t x, uint8_t y, char *str);
void OLED_12x32Str2Buf	(uint8_t x, uint8_t y, char *str);
void OLED_10x32Str2Buf	(uint8_t x, uint8_t y, char *str);
void OLED_24x32CN2Buf	(uint8_t x, uint8_t y, uint16_t cn);
void OLED_32Str2Buf 	(uint8_t x, uint8_t y, char *str);
void OLED_Wave		(uint16_t *wave, uint8_t scale);
void OLED_WaveSmall	(uint16_t *wave, uint8_t scale);
void OLED_Wave2Buf		(uint16_t *wave, uint8_t scale);
void OLED_WaveSmall2Buf (uint16_t *wave, uint8_t scale);
void OLED_Fill		(uint8_t val);
void OLED_FullBmp	(const uint8_t *bmp);
void OLED_BufClr	(void);
void OLED_LoadBuf	(void);
void OLED_Bar2Buf	(void);
void OLED_Dash2Buf	(void);
void OLED_Cross2Buf	(void);
void OLED_Frame2Buf (void);
//void OLED_SetDir (dsp_dir_t dir);

#ifdef cplusplus
}
#endif

#endif
