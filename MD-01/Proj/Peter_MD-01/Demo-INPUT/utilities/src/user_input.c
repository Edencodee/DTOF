#include "user_input.h"
#include "main.h"

void GPIO_UserInit (void)
{
	rcu_periph_clock_enable(INPUT_CLK);
	gpio_mode_set(INPUT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, INPUT_PIN);
}
