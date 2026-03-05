#include "main.h"

void DacHandler (void)
{
	u16_t dac;

	if (ChkFlag(kFlagOverRange)) {
		if (g_sreg.analog == k4_20mA) {
			if (g_sreg.keep == kKeepOn) {
				dac.word = 0xffff;	/* 2.5V	*/
			} else {
				dac.word = 13106;	/* 0.5V */
			}
		} else {
			if (g_sreg.keep == kKeepOn) {
				dac.word = 0x0fff;	/* 2.5V */
			} else {
				dac.word = 0;		/* 0V  */
			}
		}
	} else {
		const int32_t range = g_sreg.range;
		if (range == 0) {
			return;
		}
		float scal = (float)g_sreg.dist10 / 10.0f / range;
		if (scal > 0.5f) {
			scal = 0.5f;
		} else if (scal < -0.5f) {
			scal = -0.5f;
		}
		scal += 0.5f; /* (-0.5~0.5) -> (0~1) */

		if (g_sreg.analog == k4_20mA) {
			dac.word = scal * 52429 + 13106;
		} else {
			dac.word = scal * 0xffff;
		}
#if DEBUG
		static int32_t s_1sCnt = 0;
		if (LoopCntInc(&s_1sCnt, 1000)) {
			wlog("dist %d, Scal %f, dac %d", g_sreg.dist10 / 10, scal, dac.word);
		}
#endif
	}
	
	SPI_WriteDAC(dac);
}
