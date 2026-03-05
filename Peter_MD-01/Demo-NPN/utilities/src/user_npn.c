#include "user_npn.h"

const uint32_t NPN_PORT[OUT_N] = {NPN1_PORT};
const uint32_t NPN_PIN[OUT_N] = {NPN1_PIN};
const rcu_periph_enum NPN_CLK[OUT_N] = {NPN1_CLK};

const uint32_t PNP_PORT[OUT_N] = {PNP1_PORT};
const uint32_t PNP_PIN[OUT_N] = {PNP1_PIN};
const rcu_periph_enum PNP_CLK[OUT_N] = {PNP1_CLK};

void GPIO_UserInit(void)
{
	/* configure NPN/PNP GPIO port*/
	for (int i = 0; i < OUT_N; i++) {
		rcu_periph_clock_enable(NPN_CLK[i]);
		gpio_mode_set(NPN_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, NPN_PIN[i]);
		GPIO_BC(NPN_PORT[i]) = NPN_PIN[i];

		rcu_periph_clock_enable(PNP_CLK[i]);
		gpio_mode_set(PNP_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PNP_PIN[i]);
		GPIO_BC(PNP_PORT[i]) = PNP_PIN[i];
	}
}

/* output set/clr function ----------------------------------------------------------------- */
inline void NpnClr (ch_t ch)
{
	GPIO_BC(NPN_PORT[ch]) = NPN_PIN[ch];
}

inline void NpnSet (ch_t ch)
{
	GPIO_BOP(NPN_PORT[ch]) = NPN_PIN[ch];
}

inline void PnpClr (ch_t ch)
{
	GPIO_BC(PNP_PORT[ch]) = PNP_PIN[ch];
}

inline void PnpSet (ch_t ch)
{
	GPIO_BOP(PNP_PORT[ch]) = PNP_PIN[ch];
}
