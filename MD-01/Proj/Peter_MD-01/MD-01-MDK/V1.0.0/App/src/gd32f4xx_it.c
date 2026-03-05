#include "gd32f4xx_it.h"
#include "main.h"

__IO uint32_t g_extiIrq = 0;
extern uint32_t g_res[];

/*!
    \brief    this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief    this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief    this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief    this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief    this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief    this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief    this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief    this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}
#pragma arm section code = "RAMCODE"
/*!
    \brief    this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
	SysTickCallBack();
}

void Compensate(uint32_t* res, uint32_t* res1, uint32_t* res2)
{
    /* Copilot Coding=============================================== */
    /* Minimal compensation: take first stop result as measurement.
       Replace with proper calibration/compensation if available. */
    res[0] = res1[0];
    /* Copilot Coding End=============================================== */
}

/*!
    \brief    this function handles EXTI1_IRQHandler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI1_IRQHandler(void)
{
	if (RESET != exti_interrupt_flag_get(TDC_INTN_LINE)) {
        exti_interrupt_flag_clear(TDC_INTN_LINE);
		
		for (int i = 0; i < STOP_N; i++) {
			TDC_Res1[i] = TDC_ReadRes(0xB0 + i);
		}
		
		for (int i = 0; i < STOP_N; i++) {
			TDC_Res2[i] = TDC_ReadRes(0xC0 + i);
		}
		
		uint32_t res[STOP_N];
		for (int i = 0; i < STOP_N; i++) {
			Compensate(res, TDC_Res1, TDC_Res2);
		}
		
		memcpy(g_res + g_extiIrq * STOP_N, res, 4 * STOP_N);
		
		if (++g_extiIrq >= 1000) {
			g_extiIrq = 0;
			HandleHistogram(g_res);
		}
    }
}

/*!
    \brief    this function handles TIMER3 update exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER3_IRQHandler(void)
{	
	if (SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP)) {
		/* clear TIMER interrupt flag */
		timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
		Timer3UpdateCallBack();
	}
}

/*!
    \brief      this function handles USART2 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART2_IRQHandler(void)
{
	if(SET == usart_interrupt_flag_get(USART2, USART_INT_FLAG_RT)) {
        /* clear IDLE flag */
		usart_interrupt_flag_clear(USART2, USART_INT_FLAG_RT);
		
		/* toggle buffer */
		uint16_t len = UART_ToggleDmaBuf();
		
        /* call back function */
		RecvTimeoutCallBack(g_inaUartBuf, len);
    }
}
#pragma arm section
