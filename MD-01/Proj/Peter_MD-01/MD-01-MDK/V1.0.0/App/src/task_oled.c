#include "main.h"

/* micros @ ms */
#define TRANS_DSP_LTMO		2000
#define TRANS_DSP_TMO		500
#define SCR_OFF_TMO 		10000
#define RE_FLASH_TMO		100
#define CLICK_LIFE			200
#define LOCK_TMO			30000
#define MENU_TMO			20000
#define DLCLICK_WAIT		1100
#define LCLICK_WAIT			1000
#define LCLICK_STEP			20
#define THR_DSP_TMO			5000
#define TEACH_M_TMO			20000

#define AUTO_DSP_REV		1

typedef void (*dsp_func_t)(uint8_t x, uint8_t y, char *str);

typedef enum {
	kMsFunc = 0,
	kMsMenu,
	kMsTransient
} mstate_t;

typedef enum {
	kTdNone = 0,
	kTdLock,
	kTdUnlock,
	kTdFunc,
	kTdMenu,
	kTdThrh1,
	kTdThrh2,
	kTdThrGrp1,
	kTdThrGrp2
} trans_dsp_t;

typedef enum {
	kMenuSelect = 0,
	kMenuSet
} menu_state_t;

typedef enum {
	kWaveSmall = 0,
	kWaveBig,
	kWaveNone
} wave_type_t;

typedef struct {
	uint8_t name[16];
	uint32_t len;
} name_t;

typedef struct {
	name_t nameCn;
	name_t nameEn;
	name_t *sonNameCn;
	name_t *sonNameEn;
	uint32_t numOfSon;
	uint32_t regIdx;
	uint32_t nextMenu;
	uint32_t preMenu;
} menu_t;

typedef enum {
	kFuncGen2P = 0,
	kFuncGenLmtUp,
	kFuncGenLmtDn,
	kFuncWin1P,
	kFuncWin2P,
	kFuncWin3P,
	kFuncUpDnLmt,
	kFuncOk,
	kFuncErr,
	kFuncM,
	kFuncM1,
	kFuncM2,
	kFuncM3,
	kFuncUp,
	kFuncDn,
	kFuncMTmo,
	kFuncDiffLow
} func_tips_t;


/* Meun ------------------------------------------------------------------------------------- */
/* 1. menu mode ----------------------------------------------------------------------------- */
static name_t modeSonNameCn[] = {
	{{0xEA, 0xB1, 0xBC, 0xD7, '\0'}, 4},				// biao zhun
	{{0xDF, 0xB8, 0xD9, 0xCB, '\0'}, 4},				// gao su
	{{0xDF, 0xB8, 0xAB, 0xBE, 0xC8, 0xB6, '\0'}, 6}		// gao jing du
};
static name_t modeSonNameEn[] = {
	{"Standard",	8},
	{"HiSpeed",		7},
	{"HiPrecise",	9}
};
static menu_t modeMenu = {
	.nameCn 	= {{0xA4, 0xB9, 0xF7, 0xD7, 0xA3, 0xC4, 0xBD, 0xCA, '\0'}, 8},
	.nameEn 	= {"Mode", 4},							// gong zuo mo shi
	.sonNameCn	= modeSonNameCn,
	.sonNameEn	= modeSonNameEn,
	.numOfSon	= 3,
	.regIdx		= kRegMod,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* 2. menu nonc ----------------------------------------------------------------------------- */
static name_t noncSonNameCn[] = {
	{{0xA3, 0xB3, 0xAA, 0xBF, '\0'}, 4},				// chang kai
	{{0xA3, 0xB3, 0xD5, 0xB1, '\0'}, 4}					// chang bi
};
static name_t noncSonNameEn[] = {
	{"NO", 2},
	{"NC", 2}
};
static menu_t nonc1Menu = {
	.nameCn 	= {{0xA3, 0xB3, 0xAA, 0xBF, 0xA3, 0xB3, 0xD5, 0xB1, 'N', '\0'}, 9},
	.nameEn 	= {"NONC-N", 6},						// chang kai chang bi N
	.sonNameCn	= noncSonNameCn,
	.sonNameEn	= noncSonNameEn,
	.numOfSon	= 2,
	.regIdx		= kRegNONC1,
	.nextMenu	= 0,
	.preMenu	= 0
};

static menu_t nonc2Menu = {
	.nameCn 	= {{0xA3, 0xB3, 0xAA, 0xBF, 0xA3, 0xB3, 0xD5, 0xB1, 'P', '\0'}, 9},
	.nameEn 	= {"NONC-P", 6},						// chang kai chang bi P
	.sonNameCn	= noncSonNameCn,
	.sonNameEn	= noncSonNameEn,
	.numOfSon	= 2,
	.regIdx		= kRegNONC2,
	.nextMenu	= 0,
	.preMenu	= 0
};

/* ------------------------------------------------------------------------------------------ */
/* 3. menu out ------------------------------------------------------------------------------ */
static name_t outSonNameCn[] = {
	{{0xA8, 0xCD, 0xA3, 0xB3, 0xA3, 0xC4, 0xBD, 0xCA, '\0'}, 8},	// tong chang mo shi
//	{{0xBB, 0xD2, 0xE3, 0xB5, 0xBE, 0xCA, 0xCC, 0xBD, '\0'}, 8},	// yi dian shi jiao
//	{{0xBD, 0xC1, 0xE3, 0xB5, 0xBE, 0xCA, 0xCC, 0xBD, '\0'}, 8},	// liang dian shi jiao
//	{{0xFD, 0xC8, 0xE3, 0xB5, 0xBE, 0xCA, 0xCC, 0xBD, '\0'}, 8}		// san dian shi jiao
	{{0xCF, 0xC9, 0xC2, 0xCF, 0xDE, 0xCF, '\0'}, 6}					// shang xia xian
};
static name_t outSonNameEn[] = {
	{"General", 7},
//	{"1P", 		2},
//	{"2P", 		2},
//	{"3P", 		2},
	{"Limit",   5},
};
static menu_t outMenu = {
	.nameCn 	= {{0xEC, 0xBC, 0xE2, 0xB2, 0xE4, 0xCA, 0xF6, 0xB3, '\0'}, 8},
	.nameEn 	= {"MeasOut", 7}, 						// jian che shu chu
	.sonNameCn	= outSonNameCn,
	.sonNameEn	= outSonNameEn,
	.numOfSon	= 2, // 4
	.regIdx		= kRegOut,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* 4.menu analog ---------------------------------------------------------------------------- */
static name_t analogSonName[] = {
	{"4-20mA",	6},
	{"0-10V",	5},
};

static menu_t analogMenu = {
	.nameCn 	= {{0xA3, 0xC4, 0xE2, 0xC4, 0xA1, 0xD1, 0xF1, 0xD4, '\0'}, 6},	// mo ni xuan ze
	.nameEn 	= {"Analog", 6},
	.sonNameCn	= analogSonName,
	.sonNameEn	= analogSonName,
	.numOfSon	= 2,
	.regIdx		= kRegAnalog,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* 5. menu diff ----------------------------------------------------------------------------- */
static menu_t diffMenu = {
	.nameCn 	= {{0xA6, 0xD3, 0xEE, 0xB2, '\0'}, 4},	// ying cha
	.nameEn 	= {"Difference", 10},
	.sonNameCn	= 0,
	.sonNameEn	= 0,
	.numOfSon	= 0,
	.regIdx		= kRegDiff,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* 6. menu dly ------------------------------------------------------------------------------ */
static name_t dlySonNameCn[] = {
	{{0xDE, 0xCE, 0xD3, 0xD1, 0xB1, 0xCA, '\0'}, 6},	// wu yan shi
//	{{0xAA, 0xBF, 0xD3, 0xD1, 0xB1, 0xCA, '\0'}, 6},	// kai yan shi
	{{0xD3, 0xD1, 0xD9, 0xB3, 0xE4, 0xCA, 0xF6, 0xB3, '\0'}, 8},	// yan chi shu chu
//	{{0xD5, 0xB1, 0xD3, 0xD1, 0xB1, 0xCA, '\0'}, 6},	// bi yan shi
	{{0xE4, 0xCA, 0xF6, 0xB3, 0xD3, 0xD1, 0xA4, 0xB3, '\0'}, 8},	// shu chu yan chang
//	{{0xAA, 0xBF, 0xD5, 0xB1, 0xD3, 0xD1, 0xB1, 0xCA, '\0'}, 8},	// kai bi yan shi
	{{0xD3, 0xD1, 0xD9, 0xB3, 0xD3, 0xD1, 0xA4, 0xB3, '\0'}, 8}, 	// yan chi yan chang
	{{0xA5, 0xB5, 0xF6, 0xC2, 0xE5, 0xB3, '\0'}, 6}		// dan mai chong
};
static name_t dlySonNameEn[] = {
	{"DlyOff",   6},
	{"OnDly",    5},
	{"OffDly",   6},
	{"OnOffDly", 8},
	{"onePulse", 8},
};
static menu_t dlyMenu = {
	.nameCn 	= {{0xE4, 0xCA, 0xF6, 0xB3, 0xD3, 0xD1, 0xB1, 0xCA, '\0'}, 8},
	.nameEn 	= {"DelayMode", 9}, 					// shu chu yan shi
	.sonNameCn	= dlySonNameCn,
	.sonNameEn	= dlySonNameEn,
	.numOfSon	= 5,
	.regIdx		= kRegDly,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* 7. menu dlyT ----------------------------------------------------------------------------- */
static menu_t dlyTMenu = {
	.nameCn 	= {{0xD3, 0xD1, 0xD9, 0xB3, 0xB1, 0xCA, 0xE4, 0xBC, '\0'}, 8},
	.nameEn 	= {"DelayT", 6}, 						// yan chi shi jian
	.sonNameCn	= 0,
	.sonNameEn	= 0,
	.numOfSon	= 0,
	.regIdx		= kRegDlyT,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* 8. menu input ---------------------------------------------------------------------------- */
static name_t inSonNameCn[] = {
	{{0xD8, 0xB9, 0xD5, 0xB1, '\0'}, 4},				// guan bi
	{{0xF7, 0xB5, 0xE3, 0xC1, '\0'}, 4},				// tiao ling
	{{0xBE, 0xCA, 0xCC, 0xBD, '\0'}, 4},				// shi jiao
	{{0xD8, 0xB9, 0xD5, 0xB1, 0xA4, 0xBC, 0xE2, 0xB9, '\0'}, 8},	// guan bi ji guang
};
static name_t inSonNameEn[] = {
	{"None",   4},
	{"Offs",   4},
	{"Cali",   4},
	{"LaserOff",  8},
};
static menu_t inMenu = {
	.nameCn 	= {{0xE4, 0xCA, 0xEB, 0xC8, 0xA1, 0xD1, 0xF1, 0xD4, '\0'}, 8},
	.nameEn 	= {"InputFunc", 9}, 					// shu ru xuan zhe
	.sonNameCn	= inSonNameCn,
	.sonNameEn	= inSonNameEn,
	.numOfSon	= 4,
	.regIdx		= kRegIn,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* 9. menu dsp ------------------------------------------------------------------------------ */
static name_t dspSonNameCn[] = {
	{{0xEA, 0xB1, 0xBC, 0xD7, '\0'}, 4},				// biao zhun
	{{0xB4, 0xB7, 0xAA, 0xD7, '\0'}, 4},				// fan zhuan
	{{0xAB, 0xC6, 0xC6, 0xD2, '\0'}, 4}					// pian yi
};
static name_t dspSonNameEn[] = {
	{"Standard", 8},
	{"Reverse",  7},
	{"Offset",   6}
};
static menu_t dspMenu = {
	.nameCn 	= {{0xD4, 0xCF, 0xBE, 0xCA, 0xA3, 0xC4, 0xBD, 0xCA, '\0'}, 8},
	.nameEn 	= {"DispForm", 8},						// xian shi mo shi
	.sonNameCn	= dspSonNameCn,
	.sonNameEn	= dspSonNameEn,
	.numOfSon	= 3,
	.regIdx		= kRegDsp,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* A. menu keep ----------------------------------------------------------------------------- */
static name_t keepSonNameCn[] = {
	{{0xA3, 0xB1, 0xD6, 0xB3, 0xAA, 0xBF, '\0'}, 6},	// bao chi kai
	{{0xA3, 0xB1, 0xD6, 0xB3, 0xD8, 0xB9, '\0'}, 6}		// bao chi guan
};
static name_t keepSonNameEn[] = {
	{"KeepOn",  6},
	{"KeepOff", 7}
};
static menu_t keepMenu = {
	.nameCn 	= {{0xA3, 0xB1, 0xD6, 0xB3, '\0'}, 4},	// bao chi
	.nameEn 	= {"Keep", 4},
	.sonNameCn	= keepSonNameCn,
	.sonNameEn	= keepSonNameEn,
	.numOfSon	= 2,
	.regIdx		= kRegKeep,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* B. menu screen --------------------------------------------------------------------------- */
static name_t screenSonNameCn[] = {
	{{0xA3, 0xB3, 0xC1, 0xC1, '\0'}, 4},				// chang liang
	{{0xA8, 0xB6, 0xB1, 0xCA, 0xA8, 0xCF, 0xC1, 0xC6, '\0'}, 8}	// ding shi xi ping
};
static name_t screenSonNameEn[] = {
	{"NormalOn", 8},
	{"TimeOff",  7}
};
static menu_t screenMenu = {
	.nameCn 	= {{0xA8, 0xCF, 0xC1, 0xC6, 0xA1, 0xD1, 0xF1, 0xD4, '\0'}, 8},
	.nameEn 	= {"Screen", 6}, 						// xi ping xuan zhe
	.sonNameCn	= screenSonNameCn,
	.sonNameEn	= screenSonNameEn,
	.numOfSon	= 2,
	.regIdx		= kRegScreen,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* C. menu language ------------------------------------------------------------------------- */
static name_t langSonName[] = {
	{{0xF2, 0xBC, 0xE5, 0xCC, 0xD0, 0xD6, 0xC4, 0xCE, '\0'}, 8},	// jian ti zhong wen
	{"English", 7},
};
static menu_t langMenu = {
	.nameCn 	= {{0xEF, 0xD3, 0xD4, 0xD1, '\0'}, 4},  // yu yan
	.nameEn 	= {"Language", 8},
	.sonNameCn	= langSonName,
	.sonNameEn	= langSonName,
	.numOfSon	= 2,
	.regIdx		= kRegLanguage,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* D. menu addr ----------------------------------------------------------------------------- */
static menu_t addrMenu = {
	.nameCn 	= {{0xD8, 0xB5, 0xB7, 0xD6, '\0'}, 4},	// di zhi
	.nameEn 	= {"Address", 7},
	.sonNameCn	= 0,
	.sonNameEn	= 0,
	.numOfSon	= 0,
	.regIdx		= kRegAddr,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* E. menu bps ------------------------------------------------------------------------------ */
static name_t bpsSonName[] = {
	{"9600"	,	4},
	{"19200",	5},
	{"38400",	5},
	{"57600",	5},
	{"115200",	6},
	{"256000",	6},
	{"500000",	6}
};

static menu_t bpsMenu = {
	.nameCn 	= {{0xA8, 0xB2, 0xD8, 0xCC, 0xCA, 0xC2, '\0'}, 6},	// bo te lv
	.nameEn 	= {"Baudrate", 8},
	.sonNameCn	= bpsSonName,
	.sonNameEn	= bpsSonName,
	.numOfSon	= 7,
	.regIdx		= kRegBpsEnum,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* F. menu npn ------------------------------------------------------------------------------ */
static name_t npnSonName[] = {
	{"NPN", 3},
	{"PNP", 3}
};
static menu_t npnMenu = {
	.nameCn 	= {"NPN/PNP", 7},
	.nameEn 	= {"NPN/PNP", 7},
	.sonNameCn	= npnSonName,
	.sonNameEn	= npnSonName,
	.numOfSon	= 2,
	.regIdx		= kRegNPN,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* G. menu contiune ------------------------------------------------------------------------- */
static name_t switchSonNameCn[] = {
	{{0xD8, 0xB9, '\0'}, 2},							// guan
	{{0xAA, 0xBF, '\0'}, 2}								// kai
};
static name_t switchSonNameEn[] = {
	{"Off", 3},
	{"On",  2},
};
static menu_t contMenu = {
	.nameCn 	= {{0xAC, 0xC1, 0xF8, 0xD0, 0xA3, 0xC4, 0xBD, 0xCA, '\0'}, 8},
	.nameEn 	= {"Continue", 8},						// lian xu mo shi
	.sonNameCn	= switchSonNameCn,
	.sonNameEn	= switchSonNameEn,
	.numOfSon	= 2,
	.regIdx		= kRegContinue,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* H. menu reset ---------------------------------------------------------------------------- */
static name_t resetSonNameCn[] = {
//	{{0xD8, 0xD6, 0xF4, 0xC6, '\0'}, 4}, 				// chong qi
	{{0xB7, 0xC8, 0xCF, 0xC8, '\0'}, 4}, 				// que ren
//	{{0xD8, 0xD6, 0xC3, 0xD6, '\0'}, 4}, 				// chong zhi
//	{{0xF7, 0xB5, 0xD4, 0xCA, '\0'}, 4}, 				// tiao shi
	{{0xB5, 0xB7, 0xD8, 0xBB, '\0'}, 4}, 				// fan hui
};
static name_t resetSonNameEn[] = {
//	{"Reboot",  6},
	{"Confirm", 7},
//	{"Reset",   5},
//	{"Debug",   5},
	{"Return",  6},
};
static menu_t resetMenu = {
	.nameCn 	= {{0xB4, 0xB8, 0xBB, 0xCE, '\0'}, 4},	// fu wei
	.nameEn 	= {"Reset", 5},
	.sonNameCn	= resetSonNameCn,
	.sonNameEn	= resetSonNameEn,
	.numOfSon	= 2,//4,
	.regIdx		= kRegReset,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */
/* I. menu dspdir --------------------------------------------------------------------------- */
static name_t dspDirSonNameCn[] = {
	{{0xFD, 0xD5, 0xF2, 0xCF, '\0'}, 4},				// zheng xiang
	{{0xB4, 0xB7, 0xF2, 0xCF, '\0'}, 4}					// fang xiang
};
static name_t dspDirSonNameEn[] = {
	{"Forward", 7},
	{"Reverse", 7},
};
static menu_t dspDirMenu = {
	.nameCn 	= {{0xC1, 0xC6, 0xBB, 0xC4, 0xBD, 0xB7, 0xF2, 0xCF, '\0'}, 8},
	.nameEn 	= {"DspDir", 6}, 						// pi mu fang xiang
	.sonNameCn	= dspDirSonNameCn,
	.sonNameEn	= dspDirSonNameEn,
	.numOfSon	= 2,
	.regIdx		= kRegDspDir,
	.nextMenu	= 0,
	.preMenu	= 0
};
/* ------------------------------------------------------------------------------------------ */

/* 1. state variable ------------------------------------------------------------------------ */
__IO mstate_t g_mstate = kMsFunc;
__IO mstate_t g_mstateLast = kMsFunc;
__IO menu_state_t g_menuState = kMenuSelect;
__IO menu_t* g_pCurrMenu = &modeMenu;
__IO bool g_isSetByKey = false;
/* 2. thr variable -------------------------------------------------------------------------- */
const int32_t thrIdxs[][2] = {
	{kRegQ1Thr1, kRegQ1Thr2},
	{kRegQ2Thr1, kRegQ2Thr2}
};
__IO int32_t g_thrGrp = 0;
__IO int32_t g_thrIdx = 0;
__IO int32_t g_dspThrCnt = 0;
/* 3. pb variable --------------------------------------------------------------------------- */
__IO int32_t g_sclickLife[PB_N] = {0, 0, 0, 0};
__IO int32_t g_dclickLife[PB_N] = {0, 0, 0, 0};
__IO int32_t g_lclickLife[PB_N] = {0, 0, 0, 0};
__IO int32_t g_longClick [PB_N] = {INA, INA, INA, INA};
/* 4. tips variable ------------------------------------------------------------------------- */
__IO int32_t g_dspFuncCnt = 0;
__IO int32_t g_funcTipsIdx = kFuncGen2P;
__IO trans_dsp_t g_tispIndex = kTdNone;
/* 5. wave variable ------------------------------------------------------------------------- */
__IO int32_t g_waveScal[] = {5, 4};
__IO uint16_t g_wave[72];
__IO uint16_t g_waveSmall[48];
__IO bool g_isWaveUp = false;
__IO wave_type_t g_waveType = kWaveSmall;
/* 6. others variable ----------------------------------------------------------------------- */
__IO int32_t g_srOffCnt = 0;
__IO int32_t g_btnSetCnt = 0;
__IO bool g_isInputTeach = false;
const char overStr[] = "*****";

/* menu linked list init -------------------------------------------------------------------- */
void MenuInit (void)
{
	/* 1. next menu ------------------------------------------------------------------------- */
	modeMenu.nextMenu   = (uint32_t)(&nonc1Menu );
	nonc1Menu.nextMenu  = (uint32_t)(&nonc2Menu );
	nonc2Menu.nextMenu  = (uint32_t)(&outMenu   );
	outMenu.nextMenu    = (uint32_t)(&analogMenu);
	analogMenu.nextMenu = (uint32_t)(&diffMenu  );
	diffMenu.nextMenu   = (uint32_t)(&dlyMenu   );
	dlyMenu.nextMenu    = (uint32_t)(&dlyTMenu  );
	dlyTMenu.nextMenu   = (uint32_t)(&inMenu    );
	inMenu.nextMenu     = (uint32_t)(&dspMenu   );
	dspMenu.nextMenu    = (uint32_t)(&dspDirMenu);
	dspDirMenu.nextMenu = (uint32_t)(&keepMenu  );
	keepMenu.nextMenu   = (uint32_t)(&screenMenu);
	screenMenu.nextMenu = (uint32_t)(&langMenu	);
	langMenu.nextMenu   = (uint32_t)(&addrMenu	);
	addrMenu.nextMenu   = (uint32_t)(&bpsMenu	);
	bpsMenu.nextMenu    = (uint32_t)(&npnMenu	);
	npnMenu.nextMenu    = (uint32_t)(&contMenu  );
	contMenu.nextMenu   = (uint32_t)(&resetMenu	);
	resetMenu.nextMenu  = (uint32_t)(&modeMenu	);
	
	/* 2. pre menu -------------------------------------------------------------------------- */
	nonc1Menu.preMenu   = (uint32_t)(&modeMenu  );
	nonc2Menu.preMenu   = (uint32_t)(&nonc1Menu );
	outMenu.preMenu     = (uint32_t)(&nonc2Menu );
	analogMenu.preMenu  = (uint32_t)(&outMenu   );
	diffMenu.preMenu    = (uint32_t)(&analogMenu);
	dlyMenu.preMenu     = (uint32_t)(&diffMenu  );
	dlyTMenu.preMenu    = (uint32_t)(&dlyMenu 	);
	inMenu.preMenu      = (uint32_t)(&dlyTMenu  );
	dspMenu.preMenu     = (uint32_t)(&inMenu 	);
	dspDirMenu.preMenu  = (uint32_t)(&dspMenu	);
	keepMenu.preMenu    = (uint32_t)(&dspDirMenu);
	screenMenu.preMenu  = (uint32_t)(&keepMenu	);
	langMenu.preMenu    = (uint32_t)(&screenMenu);
	addrMenu.preMenu    = (uint32_t)(&langMenu	);
	bpsMenu.preMenu     = (uint32_t)(&addrMenu	);
	npnMenu.preMenu     = (uint32_t)(&bpsMenu	);
	contMenu.preMenu    = (uint32_t)(&npnMenu   );
	resetMenu.preMenu   = (uint32_t)(&contMenu	);
	modeMenu.preMenu    = (uint32_t)(&resetMenu );
}
/* ------------------------------------------------------------------------------------------ */
/* transient oled print control function ---------------------------------------------------- */
static void TransDisp (trans_dsp_t index)
{
	g_tispIndex = index;
	g_mstateLast = g_mstate;
	g_mstate = kMsTransient;
}

static void SetFuncTips (func_tips_t tips)
{
	g_dspFuncCnt = TRANS_DSP_TMO;
	g_funcTipsIdx = tips;
}

static void SetFuncLTips (func_tips_t tips)
{
	g_dspFuncCnt = TRANS_DSP_LTMO;
	g_funcTipsIdx = tips;
}
/* ------------------------------------------------------------------------------------------ */
/* counter running function ----------------------------------------------------------------- */
static bool RunningLifes (int32_t *lifes, int len)
{
	bool res = false;
	for (int i = 0; i < len; i++) {
		if (lifes[i] > 0) {
			lifes[i]--;
		}
		if (lifes[i] == 1) {
			res = true;
		}
	}
	return res;
}

static bool RunningLife (int32_t *life)
{
	if (*life > 0) {
		(*life)--;
	}
	if (*life == 1) {
		return true;
	}
	return false;
}

bool LoopCntInc (int32_t *cnt, int32_t max)
{
	if (++(*cnt) > max) {
		*cnt = 0;
		return true;
	}
	return false;
}

bool CntInc (int32_t *cnt, int32_t max)
{
	if (*cnt <= max) {
		(*cnt)++;
		return false;
	}
	return true;
}
/* ------------------------------------------------------------------------------------------ */
/* register function ------------------------------------------------------------------------ */
static void RegSetDef (int index)
{
	FLASH_RegSetDef(index);
	SetFlag(kFlagWriteReg);
}

static void RegSetMid (int index)
{
	FLASH_RegSetMid(index);
	SetFlag(kFlagWriteReg);
}

static int RegAdd (int index)
{
	int res = 0;

	if (FLASH_RegMaxChk(index)) {
		FLASH_IntRegAdd(index);
	} else {
		FLASH_RegSetMin(index);
		res = 1;
	}
	SetFlag(kFlagWriteReg);
	g_isSetByKey = true;

	return res;
}

static int RegSub (int index)
{
	int res = 0;

	if (FLASH_RegMinChk(index)) {
		FLASH_IntRegSub(index);
	} else {
		FLASH_RegSetMax(index);
		res = 1;
	}
	SetFlag(kFlagWriteReg);
	g_isSetByKey = true;

	return res;
}
/* ------------------------------------------------------------------------------------------ */
/* log function ----------------------------------------------------------------------------- */
static void logName (char* dst, char* src)
{
	memcpy(dst, src, 16);
	if (dst[0] > 0x7f) {
		for (int i = 0; dst[i] != '\0'; i += 2) {
			char temp = dst[i];
			dst[i] = dst[i + 1];
			dst[i + 1] = temp;
		}
	}
}

static void LogIntRegVal (int index, char *info, char *tip)
{
	char str[8] = {'\0'};
	int res = FLASH_GetIntRegStr(index, str);
	if (res != -1) {
		wlog("%s %s -> %s", info, tip, str);
	} else {
		wlog("%s %s -> %s", info, tip, "*****");
	}
}

static void logReg (int regIdx, char* info)
{
	if (g_pCurrMenu->numOfSon > 0) {
		char sonName[16];
		logName(sonName, (char*)g_pCurrMenu->sonNameEn[g_reg[regIdx]].name);
		wlog("%s %s -> %s", info, g_pCurrMenu->nameEn.name, sonName);
	} else {
		LogIntRegVal(regIdx, info, (char*)g_pCurrMenu->nameEn.name);
	}
}
/* ------------------------------------------------------------------------------------------ */
/* button check function -------------------------------------------------------------------- */
static bool BtnAvailableChk (void)
{
	return (g_mstate == kMsTransient) || (g_dspFuncCnt > 0);
}

static bool BothSclickChk (pb_t pb1, pb_t pb2)
{
	if ((g_sclickLife[pb1] > 0) && (g_sclickLife[pb2] > 0) && \
	    ((g_sclickLife[pb1] == 1) || (g_sclickLife[pb2] == 1))) {
		g_sclickLife[pb1] = 0;
		g_sclickLife[pb2] = 0;
		return true;
	}

	return false;
}

static bool SclickChk (pb_t pb)
{
	if (g_sclickLife[pb] == 1) {
		g_sclickLife[pb] = 0;
		return true;
	}

	return false;
}

static bool DclickChk (pb_t pb)
{
	if (g_dclickLife[pb] == 1) {
		g_dclickLife[pb] = 0;
		return true;
	}
	
	return false;
}

static bool LclickChk (pb_t pb)
{
	if (g_longClick[pb] == ACT) {
		return true;
	}
	
	return false;
}

static bool BothLclickChk (pb_t pb1, pb_t pb2)
{
	if ((g_longClick[pb1] == ACT) && (g_longClick[pb2] == ACT)) {
		return true;
	}
	
	return false;
}
/* ------------------------------------------------------------------------------------------ */
/* button task ------------------------------------------------------------------------------ */
void BtnTask (void)
{
	/* 0.  button click check --------------------------------------------------------------- */
	bool isPbSclick = RunningLifes((int32_t*)g_sclickLife, PB_N);
	bool isPbDclick = RunningLifes((int32_t*)g_dclickLife, PB_N);
	
	bool isPbLclick = false;
	for (int i = 0; i < PB_N; i++) {
		if (RunningLife((int32_t*)&g_lclickLife[i])) {
			g_longClick[i] = INA;
		}
		if (g_longClick[i] == ACT) {
			isPbLclick = true;
		}
	}

	/* 1. mask button during transient display ---------------------------------------------- */
	if (BtnAvailableChk()) {
		return;
	}
	/* 2. scan routine ---------------------------------------------------------------------- */
	for (int i = 0; i < PB_N; i++) {
		GPIO_PbScan(i);

		switch (GPIO_ReadPbStatus(i)) {
		case kLongClick: {
			g_longClick[i] = ACT;
			wlog("kLongClick %d", i);
		}
		break;

		case kLongClickRelease: {
			g_lclickLife[i] = CLICK_LIFE;
			wlog("kLongClickRelease %d", i);
		}
		break;

		case kSingleClick: {
			g_sclickLife[i] = CLICK_LIFE;
			wlog("kSingleClick %d", i);
		}
		break;
#if D_CLICK
		case kDoubleClick: {
			g_dclickLife[i] = CLICK_LIFE;
			wlog("kDoubleClick %d", i);
		}
		break;
#endif
		default:
			;
		}
	}
	
	if (g_isInputTeach) {
		ClrFlag(kFlagLock);
	}
	
	bool isBtnAct = (isPbLclick || isPbDclick || isPbSclick);
	if (isBtnAct) {		
		g_srOffCnt = 0; /* reset sreen off timer */
	}
	/* -------------------------------------------------------------------------------------- */
		
	switch (g_mstate) {
	/* 3. normal setting -------------------------------------------------------------------- */
	case kMsFunc: {
		/* 3.1. lock handler ---------------------------------------------------------------- */
		if (BothSclickChk(kPbM, kPbDn)) {
			if (ChkFlag(kFlagLock)) {
				ClrFlag(kFlagLock);
				TransDisp(kTdUnlock);
			} else {
				SetFlag(kFlagLock);
				TransDisp(kTdLock);
			}
			wlog("s_lockState = %s", ChkFlag(kFlagLock) ? "lock" : "unlock");
		}
		
		if (ChkFlag(kFlagLock)) {
			if (isBtnAct) {
				TransDisp(kTdLock); /* warning: display lock */
			}
			break; /* lock state */
		}	
#if TMO_LOCK
		else {
			static int32_t s_lockTmo = 0;
			if (isBtnAct) {
				s_lockTmo = 0;
			}
			if (LoopCntInc(&s_lockTmo, LOCK_TMO)) { /* timeout lock*/
				SetFlag(kFlagLock);
				TransDisp(kTdLock); /* display lock */
				wlog("lock timeout");
				break; /* lock state */
			}
		}
#endif	
		/* ---------------------------------------------------------------------------------- */
		/* 3.2. switch g_mstate between kMsFunc & kMsMenu ----------------------------------- */
		static int32_t s_setLclickCnt = 0;
		if (LclickChk(kPbM)) {
			if (LoopCntInc(&s_setLclickCnt, DLCLICK_WAIT)) { /* set long click over 2.1s(1000 + 1100ms)*/
				g_longClick[kPbM] = INA; /* clear M long clicked flag */
				g_mstate = (mstate_t)(1 - g_mstate);
				g_pCurrMenu = &modeMenu; 		/* reset menu to modeMenu */
				wlog("toggle kMsFunc/kMsMenu -> %s", (g_mstate == kMsFunc) ? "kMsFunc" : "kMsMenu");
			}
		} else {
			s_setLclickCnt = 0;
		}
		/* ---------------------------------------------------------------------------------- */
		/* 3.3. set double click switch small/big/none wave --------------------------------- */
		if (DclickChk(kPbM)) {
			const char waveTypeTips[][10] = {
				"Wave Small",
				"Wave Big",
				"Wave None"
			};
			g_waveType = (wave_type_t)((g_waveType + 1) % 3);
			wlog("switch small/big wave -> %s", waveTypeTips[g_waveType]);
		}
		/* ---------------------------------------------------------------------------------- */
		/* 3.4. teach ----------------------------------------------------------------------- */
		static int32_t s_teachBuf[3] = {0};
		if (g_dspFuncCnt > 0) {
			break;
		}
		switch (g_reg[kRegOut]) {
		case kGenOut: {
			/* get curr setting thr */
			int32_t index = thrIdxs[g_thrGrp][g_thrIdx];

			if (g_btnSetCnt >= 2) {
				g_btnSetCnt = 0; /* teach done */
				uint32_t diff = (s_teachBuf[0] > s_teachBuf[1]) ? \
				                s_teachBuf[0] - s_teachBuf[1] : s_teachBuf[1] - s_teachBuf[0];
				if (diff < (g_sreg.diff << 1)) {
					SetFuncLTips(kFuncDiffLow);
				} else {
					*(int32_t*)(g_reg + index) = (s_teachBuf[0] + s_teachBuf[1]) >> 1;
					SetFuncLTips(kFuncGen2P);		/* 1. M sclick twice -> kFuncGen2P	  */
					SetFlag(kFlagWriteReg);
				}
			} else if (g_btnSetCnt == 1) {
				static bool isUpLmt = false;
				static bool isDnLmt = false;

				if (isUpLmt) {
					isUpLmt = false;
					g_btnSetCnt = 0;
					*(int32_t*)(g_reg + index) = s_teachBuf[0] + (g_sreg.diff << 1);
					SetFlag(kFlagWriteReg);
					SetFuncLTips(kFuncGenLmtUp);	/* 2. M + UP sclick -> kFuncGenLmtUp  */
				} else if (isDnLmt) {
					isDnLmt = false;
					g_btnSetCnt = 0;
					*(int32_t*)(g_reg + index) = s_teachBuf[0] - (g_sreg.diff << 1);
					SetFlag(kFlagWriteReg);
					SetFuncLTips(kFuncGenLmtDn);	/* 3. M + DN sclick -> kFuncGenLmtDn  */
				}

				if (SclickChk(kPbUp)) {
					isUpLmt = true;
					SetFuncTips(kFuncUp);			/* UP sclick tip */
				} else if (SclickChk(kPbDn)) {
					isDnLmt = true;
					SetFuncTips(kFuncDn);			/* DN sclick tip */
				}
			}
		}
		break;

		case k1PTeach: {
			if (g_btnSetCnt >= 2) {
				g_btnSetCnt = 0;
				int32_t index1 = thrIdxs[g_thrGrp][0];
				int32_t index2 = thrIdxs[g_thrGrp][1];
				int32_t dist = g_sreg.dist10 / 10.0f;
				*(int32_t*)(g_reg + index1) = dist + g_sreg.diff;
				*(int32_t*)(g_reg + index2) = dist - g_sreg.diff;
				SetFuncLTips(kFuncWin1P);			/* M sclick twice -> kFuncWin1P */
				SetFlag(kFlagWriteReg);
			}
		}
		break;

		case k2PTeach: {
			if (g_btnSetCnt >= 2) {
				g_btnSetCnt = 0;
				
				uint32_t diff = (s_teachBuf[0] > s_teachBuf[1]) ? \
				                s_teachBuf[0] - s_teachBuf[1] : s_teachBuf[1] - s_teachBuf[0];
				if (diff < (g_sreg.diff << 1)) {
					SetFuncLTips(kFuncDiffLow);
				} else {
					/* set thr1/thr2 */
					int32_t index1 = thrIdxs[g_thrGrp][0];
					int32_t index2 = thrIdxs[g_thrGrp][1];
					*(int32_t*)(g_reg + index1) = s_teachBuf[0];
					*(int32_t*)(g_reg + index2) = s_teachBuf[1];
				
					SetFuncLTips(kFuncWin2P);			/* M sclick twice -> kFuncWin2P */
					SetFlag(kFlagWriteReg);
				}
			}
		}
		break;

		case k3PTeach: {
			if (g_btnSetCnt >= 3) {
				g_btnSetCnt = 0;
				/* sort buf */
				for (int i = 0; i < 3; i++) {
					for (int j = i; j < 3; j++) {
						if (s_teachBuf[i] > s_teachBuf[j]) {
							int32_t temp = s_teachBuf[i];
							s_teachBuf[i] = s_teachBuf[j];
							s_teachBuf[j] = temp;
						}
					}
				}
				uint32_t diff1 = s_teachBuf[1] - s_teachBuf[0];
				uint32_t diff2 = s_teachBuf[2] - s_teachBuf[1];
				uint32_t diff_2 = g_sreg.diff << 1;
				if ((diff1 < diff_2) || ((diff2 < diff_2))) {
					SetFuncLTips(kFuncDiffLow);
				} 
				/* set thr1 & thr2 */
				int32_t index1 = thrIdxs[g_thrGrp][0];
				int32_t index2 = thrIdxs[g_thrGrp][1];
				*(int32_t*)(g_reg + index1) = (s_teachBuf[0] + s_teachBuf[1]) >> 1;
				*(int32_t*)(g_reg + index2) = (s_teachBuf[2] + s_teachBuf[1]) >> 1;
				SetFuncLTips(kFuncWin3P);					/* M sclick 3 times -> kFuncWin3P */
				SetFlag(kFlagWriteReg);
			}
		}
		break;
		}
		
		/* record dist & M timeout */
		static int32_t s_setTmo = 0;
		if (SclickChk(kPbM) || g_isInputTeach) {
			s_setTmo = 0;
			
			if (ChkFlag(kFlagOverRange)) {
				SetFuncLTips(kFuncErr);
			} else {
				SetFuncTips((func_tips_t)(kFuncM1 + g_btnSetCnt));
				s_teachBuf[g_btnSetCnt++] = g_sreg.dist10 / 10.0f;
			}
		}	
		if (g_btnSetCnt > 0) {
			if (LoopCntInc(&s_setTmo, TEACH_M_TMO)) { /* teach 1 minut timeout */
				g_btnSetCnt = 0;
				wlog("M timeout");
				SetFuncTips(kFuncMTmo);
			}
			break;
		}
		/* ---------------------------------------------------------------------------------- */
		/* 3.5. set/clr zero ---------------------------------------------------------------- */
		if (BothSclickChk(kPbM, kPbUp)) {
#if AUTO_DSP_REV
			g_sreg.dsp = kDpReverse;
#endif
			if (g_sreg.dsp == kDpReverse) {
				if (g_sreg.offs != 0) {
					g_sreg.offs = 0;
					wlog("clear zero");
				} else {
					g_sreg.offs = g_sreg.dist10;
					wlog("set zero");
				}
				SetFlag(kFlagWriteReg);
			}
		}
		/* ---------------------------------------------------------------------------------- */
		/* 3.6. set threshold --------------------------------------------------------------- */
		if (BothSclickChk(kPbUp, kPbDn)) {	/* 1. up & dn single click switch thr1 & thr2 	*/
			g_thrIdx = 1 - g_thrIdx;
			wlog("select threshold %d", g_thrIdx);
			g_dspThrCnt = THR_DSP_TMO;
		} else if (SclickChk(kPbUp)) {		/* 2. up single click threshold++ 				*/
			int32_t index = thrIdxs[g_thrGrp][g_thrIdx];
			RegAdd(index);
			wlog("threshold[%d][%d]++", g_thrGrp, g_thrIdx);
			g_dspThrCnt = THR_DSP_TMO;
		} else if (SclickChk(kPbDn)) {		/* 3. dn single click threshold-- 				*/
			int32_t index = thrIdxs[g_thrGrp][g_thrIdx];
			RegSub(index);
			wlog("threshold[%d][%d]--", g_thrGrp, g_thrIdx);
			g_dspThrCnt = THR_DSP_TMO;
		}

		if (DclickChk(kPbUp)) {				/* 4. up double click threshold set def 		*/
			int32_t index = thrIdxs[g_thrGrp][g_thrIdx];
			RegSetDef(index);
			wlog("threshold[%d][%d] -> max", g_thrGrp, g_thrIdx);
			LogIntRegVal(index, "threshold", "max = ");
			g_dspThrCnt = THR_DSP_TMO;
		} else if (DclickChk(kPbDn)) {		/* 5. dn double click threshold set mid 		*/
			int32_t index = thrIdxs[g_thrGrp][g_thrIdx];	
			RegSetMid(index); /* or RegSetMin(index) */
			wlog("threshold[%d][%d] -> mid", g_thrGrp, g_thrIdx);
			LogIntRegVal(index, "threshold", "mid = ");
			g_dspThrCnt = THR_DSP_TMO;
		}
		
		static int32_t s_upDnLclickCnt = 0;
		if (BothLclickChk(kPbUp, kPbDn)) {
			if (LoopCntInc(&s_upDnLclickCnt, DLCLICK_WAIT)) {	/* 1. up&dn long click over 2.1s switch q1/q2 */
				g_thrGrp = 1 - g_thrGrp;
				g_dspThrCnt = THR_DSP_TMO;
				wlog("select threshold group %d", g_thrGrp);
			}
		} else {
			s_upDnLclickCnt = 0;

			static int32_t s_upLclickCnt = 0;
			static int32_t s_upLcStep = 0;
			if (LclickChk(kPbUp)) {
				if (CntInc(&s_upLclickCnt, LCLICK_WAIT)) {		/* 2. up long click over 2s add constantly  */
					if (LoopCntInc(&s_upLcStep, LCLICK_STEP)) {
						int32_t index = thrIdxs[g_thrGrp][g_thrIdx];
						RegAdd(index);
						g_dspThrCnt = THR_DSP_TMO;
					}
				}
			} else {
				s_upLclickCnt = 0;
				s_upLcStep = 0;
			}

			static int32_t s_dnLclickCnt = 0;
			static int32_t s_dnLcStep = 0;
			if (LclickChk(kPbDn)) {
				if (CntInc(&s_dnLclickCnt, LCLICK_WAIT)) {		/* 3. dn long click over 2s sub constantly  */
					if (LoopCntInc(&s_dnLcStep, LCLICK_STEP)) {
						int32_t index = thrIdxs[g_thrGrp][g_thrIdx];
						RegSub(index);
						g_dspThrCnt = THR_DSP_TMO;
					}
				}
			} else {
				s_dnLclickCnt = 0;
				s_dnLcStep = 0;
			}
		}
		/* ---------------------------------------------------------------------------------- */
	}
	break;
	/* 4. menu setting ---------------------------------------------------------------------- */
	case kMsMenu: {
		/* 4.1. menu timeout ---------------------------------------------------------------- */
		static int32_t s_menuTmo = 0;
		if (isBtnAct) {
			s_menuTmo = 0;
		}
		if (LoopCntInc(&s_menuTmo, MENU_TMO)) {
			/* cancel setting */
			if (g_menuState == kMenuSet) {
				FLASH_RegReload(g_pCurrMenu->regIdx); // reload from rom
			}
			g_mstate = kMsFunc;
			g_menuState = kMenuSelect;
			g_pCurrMenu = &modeMenu;
		}
		/* 4.2. set click to switch between kMenuSelect/kMenuSet ---------------------------- */
		if (SclickChk(kPbM)) {
			g_menuState = (menu_state_t)(1 - g_menuState);
			wlog("toggle kMenuSelect/kMenuSet -> %s", \
			     g_menuState == kMenuSet ? "kMenuSet" : "kMenuSelect");
		}
		/* 4.3. switch g_mstate between kMsFunc & kMsMenu ----------------------------------- */
		static int32_t s_setLclickCnt = 0;
		if (LclickChk(kPbM)) {
			if (LoopCntInc(&s_setLclickCnt, DLCLICK_WAIT)) { /* set long click over 2.1s(1000 + 1100ms)*/
				g_longClick[kPbM] = INA; /* clear M long clicked flag */
				g_mstate = (mstate_t)(1 - g_mstate);
				g_pCurrMenu = &modeMenu; 		/* reset menu to modeMenu */
				wlog("toggle kMsFunc/kMsMenu -> %s", (g_mstate == kMsFunc) ? "kMsFunc" : "kMsMenu");
			}
		} else {
			s_setLclickCnt = 0;
		}
		/* ---------------------------------------------------------------------------------- */
		switch (g_menuState) {
		/* 4.4. select para ----------------------------------------------------------------- */
		case kMenuSelect: {
			if (SclickChk(kPbUp)) {
				g_pCurrMenu = (menu_t*)g_pCurrMenu->nextMenu;
				wlog("to next menu -> %s", g_pCurrMenu->nameEn.name);
			} else if (SclickChk(kPbDn)) {
				g_pCurrMenu = (menu_t*)g_pCurrMenu->preMenu;
				wlog("to next menu -> %s", g_pCurrMenu->nameEn.name);
			} else if (DclickChk(kPbUp)) {
				g_pCurrMenu = &langMenu;
				wlog("to next menu -> %s", g_pCurrMenu->nameEn.name);
			} else if (DclickChk(kPbDn)) {
				g_pCurrMenu = &modeMenu;
				wlog("to next menu -> %s", g_pCurrMenu->nameEn.name);
			}
		}
		break;
		/* 4.5. set para selected ----------------------------------------------------------- */
		case kMenuSet: {
			/* get reg index */
			uint32_t regIdx = g_pCurrMenu->regIdx;
			char* name = (char*)g_pCurrMenu->nameEn.name;
			/* reg++ */
			if (SclickChk(kPbUp)) {
				RegAdd(regIdx);
				logReg(regIdx, "reg++");
			/* reg-- */
			} else if (SclickChk(kPbDn)) {
				RegSub(regIdx);
				logReg(regIdx, "reg--");
			} else if (DclickChk(kPbUp)) {
				RegSetDef(regIdx);
				LogIntRegVal(regIdx, "Set Def", name);
			} else if (DclickChk(kPbDn)) {
				RegSetMid(regIdx); /* or RegSetMin(regIdx) */
				LogIntRegVal(regIdx, "set Min", name);
			}

			/* case reg type is not enum, then support long click */
			if (g_pCurrMenu->numOfSon == 0) {
				if (LclickChk(kPbUp)) {
					static int32_t s_upLcStep = 0;
					if (LoopCntInc(&s_upLcStep, LCLICK_STEP)) {
						RegAdd(regIdx);
						LogIntRegVal(regIdx, "speed reg++", name);
					}
				} else if (LclickChk(kPbDn)) {
					static int32_t s_dnLcStep = 0;
					if (LoopCntInc(&s_dnLcStep, LCLICK_STEP)) {
						RegSub(regIdx);
						LogIntRegVal(regIdx, "speed reg--", name);
					}
				}
			}
		}
		break;
		}
	}
	break;

	default:
		;
	}
}
/* ------------------------------------------------------------------------------------------ */
/* input task ------------------------------------------------------------------------------- */
void InputTask(void)
{	
	bool isSclick = false;
	bool isDclick = false;
//	bool isLclick = false;
	bool isLrelease = false;

	GPIO_PbScan(kInput);
	switch (GPIO_ReadPbStatus(kInput)) {
	case kLongClick: {
//		isLclick = true;
		wlog("kLongClick %d", kInput);
	}
	break;

	case kLongClickRelease: {
		isLrelease = true;
		wlog("kLongClickRelease %d", kInput);
	}
	break;

	case kSingleClick: {
		isSclick = true;
		wlog("kSingleClick %d", kInput);
	}
	break;
#if D_CLICK
	case kDoubleClick: {
		isDclick = true;
		wlog("kDoubleClick %d", kInput);
	}
	break;
#endif
	default:
		;
	}

	bool isInputAct = (isSclick || isDclick || isLrelease);

	if (isInputAct) {
		isSclick = false;
		isDclick = false;
//		isLclick = false;
		isLrelease = false;

		switch (g_sreg.in) {
		case kInSetOffs: {
#if AUTO_DSP_REV
			g_sreg.dsp = kDpReverse;
#endif
			if (g_sreg.dsp == kDpReverse) {
				if (g_sreg.offs != 0) {
					g_sreg.offs = 0;
					wlog("clear zero");
				} else {
					g_sreg.offs = g_sreg.dist10;
					wlog("set zero");
				}
				SetFlag(kFlagWriteReg);
				g_srOffCnt = 0; /* reset screen off timer */
			}
		}
		break;

		case kInTeach: {
			g_isInputTeach = true;
			g_srOffCnt = 0; /* reset screen off timer */
		}
		break;

		case kInLdOff: {
			if (ChkFlag(kFlagLdOff)) {
				ClrFlag(kFlagLdOff);
				timer_primary_output_config(LD_TMR, ENABLE);
			} else {
				SetFlag(kFlagLdOff);
				timer_primary_output_config(LD_TMR, DISABLE);
			}
		}
		break;
		}
	}
}
/* ------------------------------------------------------------------------------------------ */
/* kMsFunc disp func ------------------------------------------------------------------------ */
static int GetDistStr (int32_t dist, char* str)
{
	const int32_t measD = g_sreg.measDist;
	const int32_t range_2 = g_sreg.range >> 1;

	int32_t reg = dist;

	switch (g_reg[kRegDsp]) {
	case kDpStandard: {	// (measDist - range / 2) ~ (measDist - range / 2)
		reg += measD;
	}
	break;
	case kDpReverse: {	// (-range / 2) ~ (range / 2)
		reg -= g_sreg.offs / 10;
	}
	break;
	case kDpOffs: {		// 0 ~ range
		reg += range_2;
	}
	break;
	}
	memset(str, '\0', 8);
	return S32ToStr(reg, str, 3);
}

static int GetDist10Str (int32_t dist10, char* str)
{
	const int32_t measD10 = g_sreg.measDist * 10;
	const int32_t range10_2 = g_sreg.range * 5;

	int32_t reg = dist10;

	switch (g_reg[kRegDsp]) {
	case kDpStandard: {	// (measDist - range / 2) ~ (measDist - range / 2)
		reg += measD10;
	}
	break;
	case kDpReverse: {	// (-range / 2) ~ (range / 2)
		reg -= g_sreg.offs;
	}
	break;
	case kDpOffs: {		// 0 ~ range
		reg += range10_2;
	}
	break;
	}
	memset(str, '\0', 9);
	return S32ToStr1(reg, str, 4);
}

static void OledPrint (char* str, uint8_t start, uint8_t size, \
                       uint8_t y, uint8_t font, dsp_func_t dspFunc)
{
	uint32_t len = strlen(str);
	uint8_t size_2 = len * font / 2;
	uint8_t x = (size / 2) > size_2 ? size / 2 - size_2 : 0;
	dspFunc(start + x, y, str);
}

static inline void OledPrintStrAtR (char* str)
{
	OledPrint(str, 56, 72, 0, 12, OLED_32Str2Buf);
}

static inline void OledPrintAtLD (char* str)
{
	OledPrint(str, 0, 48, 0, 8, OLED_8x16StrDn2Buf);
}

static inline void OledPrintAtLU (char* str)
{
	OledPrint(str, 0, 48, 2, 8, OLED_8x16StrUp2Buf);
}

static void OledPrintDistAtR (int32_t dist)
{
	char str[9] = {'\0'};
	char* s;
	if (ChkFlag(kFlagOverRange)) {
		s = (char*)overStr;
	} else {
		GetDist10Str(dist, str);
		s = str;
	}
	OledPrint(s, 57, 70, 0, 10, OLED_10x32Str2Buf);
}
/* ------------------------------------------------------------------------------------------ */
/* kMsMenu disp func ------------------------------------------------------------------------ */
static void DispMenu (uint8_t *menu)
{
	/* center aligned display */
	uint8_t len = strlen((char*)menu);
	uint8_t x = 64 - 6 * len;	/* (128 - 12 * size) / 2 */
	OLED_32Str2Buf(x, 0, (char*)menu);
}

static inline void DispMenuVal (char* str)
{
	OledPrint(str, 0, 128, 0, 12, OLED_12x32Str2Buf);
}
/* ------------------------------------------------------------------------------------------ */
/* sample wave funtion ---------------------------------------------------------------------- */
static uint16_t GetMax (uint16_t* p, int len)
{
	uint16_t max = p[0];
	for (int i = 1; i < len; i++) {
		if (max < p[i]) {
			max = p[i];
		}
	}
	return max;
}

static void SampleWave (uint16_t* p)
{
	int wbtm = Float2Int(g_sreg.wbtm);
	int wtop = Float2Int(g_sreg.wtop);
	if (!isnan(g_spara.xs[0]) && !isnan(g_spara.xs[PN - 1])) {
		wbtm = Float2Int(g_spara.xs[PN - 1] - 100); /* -100 & +200 to rang wave a little bit */
		wtop = Float2Int(g_spara.xs[0 ] + 200);
	}

	int cnt;
	uint16_t* pw;
	if (g_waveType == kWaveBig) {
		cnt = 72;
		pw = (uint16_t*)g_wave;
	} else {
		cnt = 48;
		pw = (uint16_t*)g_waveSmall;
	}
	if ((p[wbtm] > BG_MAX) || (p[wtop] > BG_MAX)) { /* over exposure handler */
		for (int i = 0; i < cnt; i++) {
			pw[i] = 0;
		}
	} else {
		int step = (wtop - wbtm) / cnt;
		for (int i = wbtm, j = 0; (i < wtop) && (j < cnt); i += step, j++) {
			pw[j] = GetMax(p + i, getMin(step, wtop - i));
		}
	}
}
/* ------------------------------------------------------------------------------------------ */
/* OledTask --------------------------------------------------------------------------------- */
static void DispRoutine (uint16_t* p)
{
	static name_t s_thrTipsNameEn[] = {
		{"N_THR1", 6},
		{"N_THR2", 6},
		{"P_THR1", 6},
		{"P_THR2", 6},
	};
	static name_t s_funcTipsNameEn[] = {
		{"GEN-2P", 6},
		{"UP-LMT", 6},
		{"DN-LMT", 6},
		{"WIN-1P", 6},
		{"WIN-2P", 6},
		{"WIN-3P", 6},
		{"UP-DN", 5},
		{"OK", 2},
		{"ERR", 3},

		{"M",  1},
		{"M-1", 3},
		{"M-2", 3},
		{"M-3", 3},
		{"UP",  2},
		{"DN",  2},
		{"M-TMO", 5},
		{"DIF_L", 5}
	};
	static name_t s_thrTipsNameCn[] = {
		{{'N', 0xD0, 0xE3, 0xB5, 0xD6, '1', '\0'}, 6},		// N yu zhi 1
		{{'N', 0xD0, 0xE3, 0xB5, 0xD6, '2', '\0'}, 6},		// N yu zhi 2
		{{'P', 0xD0, 0xE3, 0xB5, 0xD6, '1', '\0'}, 6},		// P yu zhi 1
		{{'P', 0xD0, 0xE3, 0xB5, 0xD6, '2', '\0'}, 6},		// P yu zhi 2
	};
	static name_t s_funcTipsNameCn[] = {
		{{0xA8, 0xCD, 0xA3, 0xB3, '2', 'P', '\0'}, 6},		// tong chang 2P
		{{0xA8, 0xCD, 0xA3, 0xB3, 'U', 'P', '\0'}, 6},		// tong chang shang 0xC9CF
		{{0xA8, 0xCD, 0xA3, 0xB3, 'D', 'N', '\0'}, 6},		// tong chang xia   0xCFC2
		{{0xB0, 0xB4, 0xDA, 0xBF, '1', 'P', '\0'}, 6}, 		// chuang kou 1P
		{{0xB0, 0xB4, 0xDA, 0xBF, '2', 'P', '\0'}, 6}, 		// chuang kou 2P
		{{0xB0, 0xB4, 0xDA, 0xBF, '3', 'P', '\0'}, 6}, 		// cchang kou 3P
		{{0xCF, 0xC9, 0xC2, 0xCF, 0xDE, 0xCF, '\0'}, 6}, 	// shang xia xian
		{{0xEA, 0xCD, 0xC9, 0xB3, '\0'}, 4},	       		// wan cheng
		{{0xF6, 0xB3, 0xED, 0xB4, '\0'}, 4},	      		// chu cuo

		{"M",   1},
		{"M-1", 3},
		{"M-2", 3},
		{"M-3", 3},
		{0xCF, 0xC9, '\0', 2},
		{0xC2, 0xCF, '\0', 2},
		{{'M', '-', 0xAC, 0xB3, 0xB1, 0xCA, '\0'}, 6},	 	// M- chao shi
		{{0xD8, 0xBB, 0xEE, 0xB2, 0xA1, 0xD0, '\0'}, 6},	// hui cha xiao
	};

	name_t *p_thrName  = g_reg[kRegLanguage] == kEnglish ? \
	                     s_thrTipsNameEn : s_thrTipsNameCn;

	name_t *p_funcName = g_reg[kRegLanguage] == kEnglish ? \
	                     s_funcTipsNameEn : s_funcTipsNameCn;

	RunningLife((int32_t*)&g_dspFuncCnt);
	RunningLife((int32_t*)&g_dspThrCnt);

	/* reflash screen every 100ms */
	static int32_t s_reflashCnt = 0;
	if (!LoopCntInc(&s_reflashCnt, RE_FLASH_TMO)) {
		return;
	}

	switch (g_mstate) {
	case kMsFunc: {
		OLED_BufClr();
		char str[9] = {'\0'};
		/* 0. twinkle control */
		static int32_t s_twinkleTick = 0;
		if (g_btnSetCnt == 0) {
			s_twinkleTick = 0;
		} else {
			s_twinkleTick++;
		}
		bool isTwkOn = (s_twinkleTick / 5 % 2 == 0); /* 500ms period */

		/* 1. display bar */
		if ((g_sreg.offs != 0) && (g_sreg.dsp == kDpReverse)) {
			OLED_Bar2Buf();
		} else {
			OLED_Dash2Buf();
		}
		/* 2. get threshold */
		int32_t index = thrIdxs[g_thrGrp][g_thrIdx];
		int32_t thr = *(int32_t*)(g_reg + index);
		GetDistStr(thr, str);
		/* 3. display threshold, distance & wave */
		int32_t dist = g_sreg.dist10;
		switch (g_waveType) {
		/* 3.1. small wave display */
		case kWaveSmall: {
			/* 3.1.1. threshold */
			if (isTwkOn) {
				OledPrintAtLD(str);
			}
			/* 3.1.2. dist */
			if (g_dspFuncCnt > 0) {
				char* func = (char*)p_funcName[g_funcTipsIdx].name;
				OledPrintStrAtR(func);
			} else if (g_dspThrCnt > 0) {
				char* name = (char*)p_thrName[(g_thrGrp << 1) + g_thrIdx].name;
				OledPrintStrAtR(name);
			} else {
				OledPrintDistAtR(dist);
			}
			/* 3.1.3. wave small */
			SampleWave(p);
			OLED_WaveSmall2Buf((uint16_t*)g_waveSmall, g_waveScal[kWaveSmall]);
		}
		break;
		/* 3.1. big wave display */
		case kWaveBig: {
			/* 3.1.1. threshold */
			if (isTwkOn) {
				OledPrintAtLD(str);
			}
			/* 3.1.2. dist */
			if (ChkFlag(kFlagOverRange)) {
				OledPrintAtLU((char*)overStr);
			} else {
				GetDistStr((dist + 5) / 10, str); /* (x + 5) / 10 means round after div by 10 */
				OledPrintAtLU(str);
			}
			/* 3.1.3. wave big */
			if (g_dspFuncCnt > 0) {
				char* func = (char*)p_funcName[g_funcTipsIdx].name;
				OledPrintStrAtR(func);
			} else if (g_dspThrCnt > 0) {
				char* name = (char*)p_thrName[(g_thrGrp << 1) + g_thrIdx].name;
				OledPrintStrAtR(name);
			} else {
				SampleWave(p);
				OLED_Wave2Buf((uint16_t*)g_wave, g_waveScal[kWaveBig]);
			}
		}
		break;
		/* 3.1. none wave display */
		case kWaveNone: {
			/* 3.1.1. threshold */
			if (isTwkOn) {
				g_thrIdx == 0 ? OledPrintAtLD(str) : OledPrintAtLU(str);
			}
			/* 3.1.2. dist */
			if (g_dspFuncCnt > 0) {
				char* func = (char*)p_funcName[g_funcTipsIdx].name;
				OledPrintStrAtR(func);
			} else if (g_dspThrCnt > 0) {
				char* name = (char*)p_thrName[(g_thrGrp << 1) + g_thrIdx].name;
				OledPrintStrAtR(name);
			} else {
				OledPrintDistAtR(dist);
			}
			/* 3.1.3. threshold other */
			index = thrIdxs[g_thrGrp][1 - g_thrIdx];
			thr = *(int32_t*)(g_reg + index);
			GetDistStr(thr, str);
			g_thrIdx != 0 ? OledPrintAtLD(str) : OledPrintAtLU(str);
		}
		break;
		}
		OLED_LoadBuf();
	}
	break;

	case kMsMenu: {
		OLED_BufClr();
		switch (g_menuState) {
		case kMenuSelect: {
			/* display menu name */
			uint8_t *name = (uint8_t*)(g_reg[kRegLanguage] == kEnglish ? \
			                           g_pCurrMenu->nameEn.name : g_pCurrMenu->nameCn.name);
			DispMenu(name);
		}
		break;

		case kMenuSet: {
			/* display item name */
			uint32_t regIdx = g_pCurrMenu->regIdx;
			if (g_pCurrMenu->numOfSon > 0) {
				name_t *son = g_reg[kRegLanguage] == kEnglish ? \
				              &g_pCurrMenu->sonNameEn[g_reg[regIdx]] : \
				              &g_pCurrMenu->sonNameCn[g_reg[regIdx]];
				uint8_t *name = (uint8_t*)son->name;
				DispMenu(name);
				/* or display item value */
			} else {
				int index = g_pCurrMenu->regIdx;
				char str[8] = {'\0'};
				FLASH_GetIntRegStr(index, str);
				DispMenuVal(str);
			}
			OLED_Frame2Buf();
		}
		break;
		}
		OLED_LoadBuf();
	}
	break;

	case kMsTransient: {
		/* restore g_mstate after 500ms */
		static int32_t s_timeout = 0;
		if (LoopCntInc(&s_timeout, 5)) {
			g_mstate = g_mstateLast;
		}
		/* trans disp state meachine */
		if (s_timeout == 1) {
			switch (g_tispIndex) {
			case kTdLock: {
				OLED_FullBmp(lock);
			}
			break;

			case kTdUnlock: {
				OLED_FullBmp(unlock);
			}
			break;

			default:
				;
			}
		}
	}
	break;
	}
}

static bool ScreenOffChk (void)
{
	static bool s_isScreenOff = false;

	if (ChkFlag(kFlagLock) && (g_sreg.screen == kSrTmoOff)) {
		if (CntInc((int32_t*)&g_srOffCnt, SCR_OFF_TMO)) {
			if (!s_isScreenOff) {
				s_isScreenOff = true;
				OLED_Fill(0x00); /* screen off */
			}
		}
	} else {
		g_srOffCnt = 0;
		s_isScreenOff = false;
	}

	return s_isScreenOff;
}

void OledTask (uint16_t* p)
{
	if (!ScreenOffChk()) {
		DispRoutine(p);
	}
}
/* ------------------------------------------------------------------------------------------ */
/* oled busy check function ----------------------------------------------------------------- */
inline bool OledChkBsy (void)
{
	/*
		g_menuState != kMenuSet means para setting over,
		g_dspThrCnt == 0 means threshold setting over
	*/
	return (g_menuState != kMenuSet) && (g_dspThrCnt == 0);
}
/* ------------------------------------------------------------------------------------------ */
/* member function -------------------------------------------------------------------------- */
void ResetWaveType (void)
{
	g_waveType = (g_sreg.out == k2PTeach) ? kWaveNone : kWaveSmall;
}

int32_t getThre (int group, int idx)
{
	int32_t thr = *(int32_t*)(g_reg + thrIdxs[group][idx]);
	return thr;
}

int32_t getCurrThre (int group)
{
	int32_t thr = *(int32_t*)(g_reg + thrIdxs[group][g_thrIdx]);
	return thr;
}
/* ------------------------------------------------------------------------------------------ */
