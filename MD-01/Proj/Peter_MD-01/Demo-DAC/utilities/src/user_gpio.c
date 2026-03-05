#include "user_gpio.h"
#include "main.h"

/* analog selecte function ----------------------------------------------------------------- */
void GPIO_AnalogSel(uint16_t analog) 
{
	(analog == 0) ? (GPIO_BC (ANALOG_PORT) = ANALOG_PIN) : \
				    (GPIO_BOP(ANALOG_PORT) = ANALOG_PIN);
}

/* ----------------------------------------------------------------------------------------- */
void GPIO_UserInit (void)
{
	/* configure ANALOG GPIO port */
	rcu_periph_clock_enable(ANALOG_CLK);
	gpio_mode_set(ANALOG_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ANALOG_PIN);
	gpio_output_options_set(ANALOG_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, ANALOG_PIN);
//	GPIO_BOP(ANALOG_PORT) = ANALOG_PIN;
}
