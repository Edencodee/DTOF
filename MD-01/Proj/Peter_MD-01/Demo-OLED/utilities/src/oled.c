#include "oled.h"
#include "lib.h"
#include "user_gpio.h"
#include "systick.h"
#include "wtdint.h"
#include <string.h>
#include "user_spi.h"

#define WR_MUL	(SPI1_DMA & 1)

static volatile uint8_t g_oledBuf[4][128];

#if !WR_MUL
static void OLED_WrDat (uint8_t dat)
{
	OLED_CS_CLR();
	
    OLED_DC_SET();
#if SPI1_DMA
	SPI_WriteOled(&dat, 1);
#else
    for (int i = 0; i < 8; i++) {
        if ((dat << i) & 0x80) {
            OLED_DIN_SET();
        } else {
            OLED_DIN_CLR();
        }
        OLED_CLK_CLR();
        OLED_CLK_SET();
    }
#endif	
	
	OLED_CS_SET();
}

static void OLED_WrCmd (uint8_t cmd)
{
    OLED_DC_CLR();
#if SPI1_DMA
	SPI_WriteOled(&cmd, 1);
#else
    for (int i = 0; i < 8; i++) {
        if ((cmd << i) & 0x80) {
            OLED_DIN_SET();
        } else {
            OLED_DIN_CLR();
        }
        OLED_CLK_CLR();
        OLED_CLK_SET();
    }
#endif	
}
#else
static void OLED_WrDats (uint8_t* dats, int size)
{
    OLED_DC_SET();
	SPI_WriteOled(dats, size);
}

static void OLED_WrCmds (uint8_t* cmds, int size)
{
    OLED_DC_CLR();
	SPI_WriteOled(cmds, size);
}
#endif

static void OLED_GpioInit (void) 
{
#if !SPI1_DMA
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	GPIO_BC(GPIOA) = GPIO_PIN_8;
	
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_9);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	GPIO_BC(GPIOC) = GPIO_PIN_9;
#endif
}

void OLED_Init (void) 
{
	OLED_GpioInit();
//	OLED_CS_SET();
	
	/* power up Vdd (RES# as low state) */
	OledSet(kOledVen);
	OledClr(kOledRes);
	/* power stabilized (delay recommended) */
	// ...
	/* power up Vddb (100ms delay recommended) */
	delay_1ms(100);
	/* set RES# as high (3us delay minimum) */
	OledSet(kOledRes);
	delay_1ms(1);
#if !WR_MUL	
	/* set display off */
    OLED_WrCmd(0xae);
	/* initial settings configuration ---------------------- */
	/* 1. set display clock divide ratio/oscilator frequency */
    OLED_WrCmd(0xd5);
    OLED_WrCmd(0x80);
	/* 2. set multiplex ratio */
    OLED_WrCmd(0xa8);
    OLED_WrCmd(0x1f); // (0x3f TBD)
	/* 3. set display offset */
    OLED_WrCmd(0xd3);
    OLED_WrCmd(0x00);
	/* 4. set display start line */
    OLED_WrCmd(0x40);
	/* 5. set charge pump */
    OLED_WrCmd(0x8d);
    OLED_WrCmd(0x14);
	/* 6. set segment re-map */
    OLED_WrCmd(0xa1);
	/* 7. set com output scan direction */
    OLED_WrCmd(0xc8);
	/* 8. set com pin hardware configuration */
    OLED_WrCmd(0xda);
    OLED_WrCmd(0x12);
	/* 9. set contrast control */
	OLED_WrCmd(0x81);
	OLED_WrCmd(0x58); // TBD
	/* 10. set pre-charge period */
    OLED_WrCmd(0xd9);
    OLED_WrCmd(0x22); // (0xf1 TBD)
	/* 11. set VCOMH deselect level */
    OLED_WrCmd(0xdb);
    OLED_WrCmd(0x20); // (0x40 TBD)
	/* 12. set entire display on/off */
    OLED_WrCmd(0xa4);
	/* 13. set normal/inverse display */
    OLED_WrCmd(0xa6);
	/* 14. clear screen */
	OLED_Fill(0x00);
	/* 15. set display on */
    OLED_WrCmd(0xaf);
	/* ----------------------------------------------------- */
#else
	uint8_t cmds[] = {
		0xae,
		0xd5, 0x80,
		0x00, 0xf0,
		0xd6, 0xff, ////3
		0x40,
		0x8d, 0x14,
		0xa1,		////6
		0xcF,		////X-axis flipping
		0xda, 0x12,
		0x81, 0x58, ////9
		0xd9, 0x22,
		0xdb, 0x20,
		0xa4,		////12
		0xa0,		////Y-axis flipping
		0xaf
	};
	
//	if (g_sreg.dspDir == kDirRes) {
//		cmds[10] = 0xA0;
//		cmds[11] = 0xC0;
//	}
	
	OLED_WrCmds((uint8_t*)cmds, 23);
	SPI_WaitOled();
	OLED_Fill(0xf0);
#endif
}

static int OLED_HalfSizeChk(char c) 
{
	return (c == '.') || (c == '-') || (c == '}') || \
		   (c == '`') || (c == '_') || (c == ';') || \
		   (c == ',') || (c == '!') || (c == ' ') || \
		   (c == '{') || (c == '[') || (c == ']') || \
		   (c == '|') || (c == ':') || (c == ']') || \
		   (c == '(') || (c == ')') || \
		   (c == '\'')
	;
}

static void OLED_12x32C2Buf (uint8_t x, uint8_t y, char a)
{
	uint8_t c = a - 32;
	
	int y1 = y + 1;
	int y2 = y + 2;
	int y3 = y + 3;
	for (int i = 0; i < 12; i++) {
		g_oledBuf[y ][x + i] = f12x32[c][i + 36];
		g_oledBuf[y1][x + i] = f12x32[c][i + 24];
		g_oledBuf[y2][x + i] = f12x32[c][i + 12];
		g_oledBuf[y3][x + i] = f12x32[c][i];
	}
}

static int32_t chkAscii(uint8_t c) 
{
	for (int i = 0; i < USED_ASCII; i++) {
		if (c == ascii[i]) {
			return i;
		}
	}
	return -1;
}

void OLED_8x16StrDn2Buf (uint8_t x, uint8_t y, char *str)
{		
    for (int a = 0; str[a] != '\0'; a++) {
		int32_t c;
		if ((c = chkAscii(str[a])) == -1) {
			return;
		}
		
		int y1 = y + 1;
        for (int i = 0; i < 8; i++) {
            g_oledBuf[y ][x + i] = f8x16dn[c][i + 8];
			g_oledBuf[y1][x + i] = f8x16dn[c][i];
        }
		
        x += OLED_HalfSizeChk(str[a]) ? 4 : 8;
		if (x > 120) {
            x = 0;
            y = (y + 2) % 4;
        }
    }
}

void OLED_8x16StrUp2Buf (uint8_t x, uint8_t y, char *str)
{		
    for (int a = 0; str[a] != '\0'; a++) {
		int32_t c;
		if ((c = chkAscii(str[a])) == -1) {
			return;
		}
		
		int y1 = y + 1;
        for (int i = 0; i < 8; i++) {
            g_oledBuf[y ][x + i] = f8x16up[c][i + 8];
			g_oledBuf[y1][x + i] = f8x16up[c][i];
        }
		
        x += OLED_HalfSizeChk(str[a]) ? 4 : 8;
		if (x > 120) {
            x = 0;
            y = (y + 2) % 4;
        }
    }
}

void OLED_12x32Str2Buf (uint8_t x, uint8_t y, char *str) 
{	
	for (int a = 0; str[a] != '\0'; a++) {
        uint8_t c = str[a] - 32;

		int y1 = y + 1;
		int y2 = y + 2;
		int y3 = y + 3;
        for (int i = 0; i < 12; i++) {
			g_oledBuf[y ][x + i] = f12x32[c][i + 36];
			g_oledBuf[y1][x + i] = f12x32[c][i + 24];
			g_oledBuf[y2][x + i] = f12x32[c][i + 12];
			g_oledBuf[y3][x + i] = f12x32[c][i];
        }
		
		x += OLED_HalfSizeChk(str[a]) ? 6 : 12;
		if (x > 116) {
            x = 0;
        }
    }
}

void OLED_10x32Str2Buf (uint8_t x, uint8_t y, char *str)
{
	for (int a = 0; str[a] != '\0'; a++) {
		int32_t c;
		if ((c = chkAscii(str[a])) == -1) {
			return;
		}

		int y1 = y + 1;
		int y2 = y + 2;
		int y3 = y + 3;
        for (int i = 0; i < 10; i++) {
			g_oledBuf[y ][x + i] = f10x32[c][i + 30];
			g_oledBuf[y1][x + i] = f10x32[c][i + 20];
			g_oledBuf[y2][x + i] = f10x32[c][i + 10];
			g_oledBuf[y3][x + i] = f10x32[c][i];
        }
		
		x += OLED_HalfSizeChk(str[a]) ? 5 : 10;
		if (x > 118) {
            x = 0;
        }
    }
}

void OLED_24x32CN2Buf (uint8_t x, uint8_t y, uint16_t cn)
{
	int idx = -1;
	int libSize = sizeof(gb2312) / 2;
	for (int i = 0; i < libSize; i++) {
		if (cn == gb2312[i]) {
			idx = i;
		}
	}
	if (idx == -1) {
		return;
	}
	
	int y1 = y + 1;
	int y2 = y + 2;
	int y3 = y + 3;
	for (int i = 0; i < 24; i++) {
		g_oledBuf[y ][x + i] = f24x32[idx][i + 72];
		g_oledBuf[y1][x + i] = f24x32[idx][i + 48];
		g_oledBuf[y2][x + i] = f24x32[idx][i + 24];
		g_oledBuf[y3][x + i] = f24x32[idx][i];
	}
}

void OLED_32Str2Buf (uint8_t x, uint8_t y, char *str)
{
	for (int a = 0; str[a] != '\0'; a++) {
        uint8_t c = str[a];
		
		if (c > 0x7f) {
			uint16_t cn = *(uint16_t*)(str + a++);
			if (x > 104) {
				x = 0;
			}
			OLED_24x32CN2Buf(x, y, cn);
			x += 24;
		} else {	
			if (x > 116) {
				x = 0;
			}
			OLED_12x32C2Buf(x, y, c);
			x += OLED_HalfSizeChk(str[a]) ? 6 : 12;
		}
	}
}

void OLED_Wave2Buf (uint16_t *wave, uint8_t scale) 
{	
	for (int i = 0; i < 72; i++) {
		int shift = ((wave[i] - 750) >> scale); /* shift 1 ~ 31 */
		if (shift < 1) {
			shift = 1;
		}
		u32_t val = {.word = (1ul << shift) - 1};
		g_oledBuf[0][56 + i] = val.buf[0];
		g_oledBuf[1][56 + i] = val.buf[1];
		g_oledBuf[2][56 + i] = val.buf[2];
		g_oledBuf[3][56 + i] = val.buf[3];
	}
}

void OLED_WaveSmall2Buf (uint16_t *wave, uint8_t scale) 
{	
	for (int i = 0; i < 48; i++) {
		int shift = ((wave[i] - 700) >> scale); /* shift 1 ~ 15 */
		if (shift < 1) {
			shift = 1;
		}
		u16_t val = {.word = (1ul << shift) - 1};
		g_oledBuf[2][i] = val.buf[0];
		g_oledBuf[3][i] = val.buf[1];
	}
}

static void OLED_HoriAddrMode (void)
{
#if !WR_MUL	
	/* horizontal addressing mode */ 
	OLED_WrCmd(0x20);
	OLED_WrCmd(0x00);
	/* Set the column start and end address */
	OLED_WrCmd(0x21);
	OLED_WrCmd(0);
	OLED_WrCmd(127);
	/* Set the page start and end addreess */
	OLED_WrCmd(0x22);
	OLED_WrCmd(0);
	OLED_WrCmd(3);
#else
	const uint8_t cmds[8] = {
		0x20, 0x00,
		0x21, 0x00, 0x7F,
		0x22, 0x00, 0x03
	};
	OLED_WrCmds((uint8_t*)cmds, 8);
	SPI_WaitOled();
#endif	
}

void OLED_Fill (uint8_t val)
{
	memset((uint8_t*)g_oledBuf, val, 512);
	OLED_HoriAddrMode();
#if WR_MUL
	OLED_WrDats((uint8_t*)g_oledBuf, 512);
#else
	for (int i = 0; i < 512; i++) {
		OLED_WrDat(val);
	}
#endif
}

void OLED_FullBmp (const uint8_t *bmp)
{
	OLED_HoriAddrMode();
#if WR_MUL
	OLED_WrDats((uint8_t*)bmp, 512);
#else	
	for (int i = 0; i < 512; i++) {
		OLED_WrDat(bmp[i]);
	}
#endif
}

void OLED_BufClr (void) 
{
	memset((uint8_t*)g_oledBuf, 0, 512);
}

void OLED_LoadBuf (void) 
{	
	OLED_HoriAddrMode();
#if WR_MUL
	OLED_WrDats((uint8_t*)g_oledBuf, 512);
#else	
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 128; x++) {
			OLED_WrDat(g_oledBuf[y][x]);
		}
	}
#endif	
}

void OLED_Bar2Buf (void) 
{
	g_oledBuf[0][51] = 0xff;
	g_oledBuf[0][52] = 0xff;
	g_oledBuf[1][51] = 0xff;
	g_oledBuf[1][52] = 0xff;
	g_oledBuf[2][51] = 0xff;
	g_oledBuf[2][52] = 0xff;
	g_oledBuf[3][51] = 0xff;
	g_oledBuf[3][52] = 0xff;
}

void OLED_Dash2Buf (void) 
{
	g_oledBuf[0][51] = 0xe6;
	g_oledBuf[0][52] = 0xe6;
	g_oledBuf[1][51] = 0x66;
	g_oledBuf[1][52] = 0x66;
	g_oledBuf[2][51] = 0x66;
	g_oledBuf[2][52] = 0x66;
	g_oledBuf[3][51] = 0xe6;
	g_oledBuf[3][52] = 0xe6;
}

void OLED_Cross2Buf (void) 
{
	for (int i = 0; i < 3; i++) {
		g_oledBuf[1][i] |= 0x80;
		g_oledBuf[2][i] |= 0x01;
	}
	
	for (int i = 5; i < 43; i += 4) {
		g_oledBuf[1][i] |= 0x80;
		g_oledBuf[2][i] |= 0x01;
		g_oledBuf[1][i + 1] |= 0x80;
		g_oledBuf[2][i + 1] |= 0x01;
	}
	
	for (int i = 45; i < 48; i++) {
		g_oledBuf[1][i] |= 0x80;
		g_oledBuf[2][i] |= 0x01;
	}
}

void OLED_Frame2Buf (void)
{
	const uint8_t buf1[6] = {0x3F, 0x3F, 0x03, 0x03, 0x03, 0x03};
	memcpy((uint8_t*)g_oledBuf[0], buf1, 6);
//	g_oledBuf[0][0] = 0x3F;
//	g_oledBuf[0][1] = 0x3F;
//	g_oledBuf[0][2] = 0x03;
//	g_oledBuf[0][3] = 0x03;
//	g_oledBuf[0][4] = 0x03;
//	g_oledBuf[0][5] = 0x03;
	const uint8_t buf2[6] = {0x03, 0x03, 0x03, 0x03, 0x3F, 0x3F};
	memcpy((uint8_t*)g_oledBuf[0] + 122, buf2, 6);
//	g_oledBuf[0][122] = 0x03;
//	g_oledBuf[0][123] = 0x03;
//	g_oledBuf[0][124] = 0x03;
//	g_oledBuf[0][125] = 0x03;
//	g_oledBuf[0][126] = 0x3F;
//	g_oledBuf[0][127] = 0x3F;
	
	const uint8_t buf3[6] = {0xFC, 0xFC, 0xC0, 0xC0, 0xC0, 0xC0};
	memcpy((uint8_t*)g_oledBuf[3], buf3, 6);
//	g_oledBuf[3][0] = 0xFC;
//	g_oledBuf[3][1] = 0xFC;
//	g_oledBuf[3][2] = 0xC0;
//	g_oledBuf[3][3] = 0xC0;
//	g_oledBuf[3][4] = 0xC0;
//	g_oledBuf[3][5] = 0xC0;
	const uint8_t buf4[6] = {0xC0, 0xC0, 0xC0, 0xC0, 0xFC, 0xFC};
	memcpy((uint8_t*)g_oledBuf[3] + 122, buf4, 6);
//	g_oledBuf[3][122] = 0xC0;
//	g_oledBuf[3][123] = 0xC0;
//	g_oledBuf[3][124] = 0xC0;
//	g_oledBuf[3][125] = 0xC0;
//	g_oledBuf[3][126] = 0xFC;
//	g_oledBuf[3][127] = 0xFC;
}

//void OLED_SetDir (dsp_dir_t dir) {
//	uint8_t cmds[2];
//	
//	switch (dir) {
//	case kDirFwd: {
//		cmds[0] = 0xA1;
//		cmds[1] = 0xC8;	
//	} break;
//	
//	case kDirRes: {
//		cmds[0] = 0xA0;
//		cmds[1] = 0xC0;	
//	} break;
//	}
//	
//	OLED_WrCmds((uint8_t*)cmds, 2);
//	SPI_WaitOled();
//}
