#include <string.h>
#include <stdio.h>
#include "user_timer.h"
#include "systick.h"

#define PWM_1M 239

static void TIMER_CfgGpio (void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	/*Configure PB3(TIMER1 CH1) as alternate function*/
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_3);
}

#if 1
static void TIMER_Config(void)
{
	timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

	/* 1. timer1 1MHz LD pulse ---------------------------------------------------- */
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);// Timer1 clk 240M

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = PWM_1M;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* CH1 configuration in PWM mode */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocintpara);
 
    /* CH1 configuration in PWM mode1,duty cycle 0.4% */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 1);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */  
    timer_auto_reload_shadow_enable(TIMER1);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
	
	/* 2. timer3 200us timing ----------------------------------------------------- */
	rcu_periph_clock_enable(RCU_TIMER3);
//    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

	timer_deinit(TIMER3);
	/* TIMER1 configuration, 200us period */
	timer_initpara.prescaler         = 29;	// 29 -> 240 / 30 = 8M
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 1600; // 200us -> 200 * 8
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER3, &timer_initpara);
	
	/* enable update interrupt */
	nvic_irq_enable(TIMER3_IRQn, 0, 0);
	timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
	timer_interrupt_enable(TIMER3, TIMER_INT_FLAG_UP);

	/* auto-reload preload enable */
	timer_auto_reload_shadow_enable(TIMER3);
	timer_enable(TIMER3);
	/* ---------------------------------------------------------------------------- */
}
#endif

void TIMER_UserInit()
{
	TIMER_CfgGpio();
	TIMER_Config();
}
