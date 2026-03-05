#include "main.h"

#define FLASH_WRITE_CHK		1000

__IO bool g_isLogEn = true;

/* software init ---------------------------------------------------------------------------- */
void AppInit (void)
{
	/* 1. menu init ------------------------------------------------------------------------- */
	MenuInit();
	/* 2. range and threshold --------------------------------------------------------------- */
	u32_t flag = {.word = g_spara.flag};
	g_isLogEn = (flag.buf[1] == 1) ? true : false;
	uint8_t type = flag.buf[0] - 1;
	const int32_t range_2[3] = {5000, 10000, 20000};
	sregMax.q1thr1 =  range_2[type];
	sregMax.q1thr2 =  range_2[type];
	sregMax.q2thr1 =  range_2[type];
	sregMax.q2thr2 =  range_2[type];
	sregMin.q1thr1 = -range_2[type];
	sregMin.q1thr2 = -range_2[type];
	sregMin.q2thr1 = -range_2[type];
	sregMin.q2thr2 = -range_2[type];
	const uint32_t range[3] = {10000, 20000, 40000};
	g_sreg.range = range[type];
	const uint32_t measD[3] = {30000, 50000, 85000};
	g_sreg.measDist = measD[type];
	/* 3. wave type setting ----------------------------------------------------------------- */
	ResetWaveType();
	/* 4. cpuTs ----------------------------------------------------------------------------- */
	CpuTsTmrInit();
	uint32_t t1 = CpuTsTmrRead();
	delay_1ms(100);
	uint32_t t2 = CpuTsTmrRead();
	wlog("CpuTs:");
	info("SystemCoreClock = %d", SystemCoreClock);
	info("Delay_ms(100) time: %fus", (t2 - t1) * TS);
	/* 5. cali para ------------------------------------------------------------------------- */
	wlog("CALI PARA:");
	wlog("flag: %x", g_spara.flag);
	wlog("offs: %d", g_spara.offs10);
	wlog("xs ys:");
	for (int i = 0; i < 20; i++) {
		if (!isnan(g_spara.xs[i])) {
			info("%02d %.2f\t%d", i, g_spara.xs[i], g_spara.ys[i]);
		}
	}
}
/* ------------------------------------------------------------------------------------------ */
/* host debug ------------------------------------------------------------------------------- */
static void Debug (uint16_t* p)
{
	const uint8_t head[] = {0xA5, 0x5A};

	UART_SEND_DATA((uint8_t*)head, 2);
	for (int i = 0; i < PIXELS; i++) {
		u16_t temp = {.word = p[i] >> 1};
		u16_t ps = {0};
		ps.buf[kLsb] = temp.buf[kMsb];
		ps.buf[kMsb] = temp.buf[kLsb];
		UART_SEND_DATA(ps.buf, 2);
	}
}
/* ------------------------------------------------------------------------------------------ */
/* csi-frame handers, called every sample period -------------------------------------------- */
void HandleHistogram(uint32_t* p)
{
	if (ChkFlag(kFlagDebugW)) {
		Debug(p);
		ClrFlag(kFlagDebugW);
		return;
	}
/* Copilot Coding=============================================== */
	/* measurement processing */
	Meas_HandleHistogram(p);

	/* update outputs */
	DacHandler();
	OutputTask();
	if (g_sreg.contiune) { /* modbus contiune mode */
		MODBUS_ContiuneOut();
	}
/* Copilot Coding End=============================================== */
}
/* ------------------------------------------------------------------------------------------ */
/* timer0 update call back ------------------------------------------------------------------ */
void Timer3UpdateCallBack (void)
{
	ShortChkTask();
}
/* ------------------------------------------------------------------------------------------ */
/* sysTick interrupt call back -------------------------------------------------------------- */
void SysTickCallBack (void)
{
	static int32_t s_1msCnt = 0;
	static int32_t s_1sCnt = 0;

	if (!ChkFlag(kFlagInit)) {
		return;
	}

	/* 1. feed dog -------------------------------------------------------------------------- */
	if (LoopCntInc(&s_1msCnt, 1)) {
		fwdgt_counter_reload();
	}
	/* -------------------------------------------------------------------------------------- */
	/* 2. key & led routine ----------------------------------------------------------------- */
	BtnTask();
	LedTask();
	InputTask();
	/* -------------------------------------------------------------------------------------- */
	/* 3. oled routine ---------------------------------------------------------------------- */
	OledTask((uint16_t*)g_inaAdcBuf);
	/* -------------------------------------------------------------------------------------- */
	/* 4. flash write ----------------------------------------------------------------------- */
	if (LoopCntInc(&s_1sCnt, FLASH_WRITE_CHK)) {
		FlashTask();
	}
}
/* ------------------------------------------------------------------------------------------ */
/* recv tmo call back ----------------------------------------------------------------------- */
void RecvTimeoutCallBack (void* userData, uint16_t len)	
{
	uint8_t *p = userData;

	MODBUS_RecvHandler(p, len);
}
/* ------------------------------------------------------------------------------------------ */
