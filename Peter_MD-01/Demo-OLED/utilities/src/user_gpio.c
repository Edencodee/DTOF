#include "user_gpio.h"

/* IO array -------------------------------------------------------------------------------- */
const uint32_t OLED_PORT[OLED_N] = {OLED_RES_PORT, OLED_CS_PORT, OLED_DC_PORT, OLED_VEN_PORT};
const uint32_t OLED_PIN[OLED_N] = {OLED_RES_PIN, OLED_CS_PIN, OLED_DC_PIN, OLED_VEN_PIN};
const rcu_periph_enum OLED_CLK[OLED_N] = {OLED_RES_CLK, OLED_CS_CLK, OLED_DC_CLK, OLED_VEN_CLK};

/* ----------------------------------------------------------------------------------------- */
void GPIO_UserInit(void)
{
	/* configure OLED GPIO port */
	for (int i = 0; i < OLED_N; i++) {
		rcu_periph_clock_enable(OLED_CLK[i]);
		gpio_mode_set(OLED_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OLED_PIN[i]);
		GPIO_BC(OLED_PORT[i]) = OLED_PIN[i];
	}	
}

/* oled ctrl set/clr function -------------------------------------------------------------- */
inline void OledClr (oled_ctrl_t ctrl)
{
	GPIO_BC(OLED_PORT[ctrl]) = OLED_PIN[ctrl];
}

inline void OledSet (oled_ctrl_t ctrl)
{
	GPIO_BOP(OLED_PORT[ctrl]) = OLED_PIN[ctrl];
}
/* ----------------------------------------------------------------------------------------- */
