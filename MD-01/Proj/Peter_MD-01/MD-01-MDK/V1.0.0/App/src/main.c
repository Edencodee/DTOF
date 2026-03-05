#include "main.h"

#define APP_VERSION		"1.0.0"

static void BSP_Init (void);
static void Wdgt_UserInit (void);

extern __IO bool g_isSampleDone;
extern uint32_t* g_inaAdcBuf;

int main (void)
{
	/* reset ivt address */
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, APP_ADDR - 0x8000000ul);	
    BSP_Init();
    while(1) {
		//串口标志位检测
		//测量任务
		//看门狗喂狗
		//短路检测
		//按键事件
    }
}

/* BSP function -------------------------------------------- */
static void Wdgt_UserInit (void)
{
	/* confiure FWDGT counter clock: 40KHz(IRC40K) / 64k = 0.625 Hz	*/
	fwdgt_config(2 * 500, FWDGT_PSC_DIV64);
	fwdgt_enable();
}

static void BSP_Init (void)
{
	/* 1. global interrupt and systick */
	__enable_irq();
	systick_config();
	/* 2. load reg from flash*/
	FLASH_RegLoadChk();
	FLASH_ParaLoadChk();
	FLASH_CoefLoadChk();
	/* 3. init uart */
	UART_UserInit(USER_COM1, g_sreg.bps);
	UART_UserDMAInit();
	wlog("Run MD-01 App Version %s", APP_VERSION); /* version print */
	/* 4. init spi & dac */
	SPI_UserInit();
	/* 5. init gpio */
	GPIO_UserInit();
	GPIO_AnalogSel(g_sreg.analog);
	/* 6. init oled */
	OLED_Init();
//	OLED_FullBmp(bojke);
	delay_1ms(1000);
	OLED_Fill(0x00);
	g_reg[kRegFlag] |= (1 << kFlagInit);
	/* 7. app init */
	AppInit();
	/* 8. init timer */
	TIMER_UserInit();
	/* 9. watch dog init */
	Wdgt_UserInit();
}
/* --------------------------------------------------------- */

/* retarget printf function to the USART ------------------- */
int fputc (int ch, FILE *f) 
{
	usart_data_transmit(USER_COM1, (uint8_t)ch);
	while(RESET == usart_flag_get(USER_COM1, USART_FLAG_TBE));

	return ch;
}
/* --------------------------------------------------------- */
