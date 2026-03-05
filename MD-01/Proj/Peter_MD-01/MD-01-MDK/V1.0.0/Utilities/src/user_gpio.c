#include "user_gpio.h"
#include "main.h"
#include <string.h>

typedef enum {
	kBsIdle = 0,
	kBsDithering1,
	kBsWait1,
	kBsRelease1,
	kBsDithering2,
	kBsWait2,
	kBsWait3,
} button_state_t;

/* key variable */
__IO key_press_state_t g_pressState[PB_N + 1] = {kNoneClick, kNoneClick, kNoneClick, kNoneClick};

/* IO array -------------------------------------------------------------------------------- */
const uint32_t PB_PORT[PB_N] = {PB0_PORT, PB1_PORT, PB2_PORT, PB3_PORT};
const uint32_t PB_PIN[PB_N] = {PB0_PIN, PB1_PIN, PB2_PIN, PB3_PIN};
const rcu_periph_enum PB_CLK[PB_N] = {PB0_CLK, PB1_CLK, PB2_CLK, PB3_CLK};

const uint32_t LED_PORT[LED_N] = {BLED_PORT, RLED_PORT};
const uint32_t LED_PIN[LED_N] = {BLED_PIN, RLED_PIN};
const rcu_periph_enum LED_CLK[LED_N] = {BLED_CLK, RLED_CLK};

const uint32_t OLED_PORT[OLED_N] = {OLED_RES_PORT, OLED_CS_PORT, OLED_DC_PORT, OLED_VEN_PORT};
const uint32_t OLED_PIN[OLED_N] = {OLED_RES_PIN, OLED_CS_PIN, OLED_DC_PIN, OLED_VEN_PIN};
const rcu_periph_enum OLED_CLK[OLED_N] = {OLED_RES_CLK, OLED_CS_CLK, OLED_DC_CLK, OLED_VEN_CLK};

const uint32_t NPN_PORT[OUT_N] = {NPN1_PORT};
const uint32_t NPN_PIN[OUT_N] = {NPN1_PIN};
const rcu_periph_enum NPN_CLK[OUT_N] = {NPN1_CLK};

const uint32_t PNP_PORT[OUT_N] = {PNP1_PORT};
const uint32_t PNP_PIN[OUT_N] = {PNP1_PIN};
const rcu_periph_enum PNP_CLK[OUT_N] = {PNP1_CLK};

const uint32_t FUN_PORT[OUT_N] = {FUN1_PORT};
const uint32_t FUN_PIN[OUT_N] = {FUN1_PIN};
const rcu_periph_enum FUN_CLK[OUT_N] = {FUN1_CLK};

const uint32_t FUP_PORT[OUT_N] = {FUP1_PORT};
const uint32_t FUP_PIN[OUT_N] = {FUP1_PIN};
const rcu_periph_enum FUP_CLK[OUT_N] = {FUP1_CLK};
/* ----------------------------------------------------------------------------------------- */
#pragma arm section code = "RAMCODE"
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
/* ----------------------------------------------------------------------------------------- */
#pragma arm section
/* led set/clr function -------------------------------------------------------------------- */
inline void LedClr (led_t led)
{
	GPIO_BC(LED_PORT[led]) = LED_PIN[led];
}

inline void LedSet (led_t led)
{
	GPIO_BOP(LED_PORT[led]) = LED_PIN[led];
}
/* ----------------------------------------------------------------------------------------- */
#pragma arm section code = "RAMCODE"
/* protect get function -------------------------------------------------------------------- */
inline FlagStatus GetNpn (ch_t ch)
{
	return gpio_input_bit_get(FUN_PORT[ch], FUN_PIN[ch]);
}

inline FlagStatus GetPnp (ch_t ch)
{
	return gpio_input_bit_get(FUP_PORT[ch], FUP_PIN[ch]);
}
/* ----------------------------------------------------------------------------------------- */
#pragma arm section
/* ----------------------------------------------------------------------------------------- */

/* switch get function --------------------------------------------------------------------- */
inline FlagStatus GetPb (pb_t pb)
{
	return gpio_input_bit_get(PB_PORT[pb], PB_PIN[pb]);
}
/* ----------------------------------------------------------------------------------------- */

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

/* analog selecte function ----------------------------------------------------------------- */
void GPIO_AnalogSel(uint16_t analog) 
{
	(analog == 0) ? (GPIO_BC (ANALOG_PORT) = ANALOG_PIN) : \
				    (GPIO_BOP(ANALOG_PORT) = ANALOG_PIN);
}

/* ----------------------------------------------------------------------------------------- */
void GPIO_UserInit (void)
{
	/* configure SW GPIO port */
	for (int i = 0; i < PB_N; i++) {
		rcu_periph_clock_enable(PB_CLK[i]);
		gpio_mode_set(PB_PORT[i], GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, PB_PIN[i]);
	}

	/* configure LED GPIO port */
	for (int i = 0; i < LED_N; i++) {
		rcu_periph_clock_enable(LED_CLK[i]);
		gpio_mode_set(LED_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN[i]);
		gpio_output_options_set(LED_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_PIN[i]);
		GPIO_BC(LED_PORT[i]) = LED_PIN[i];
	}

	/* configure OLED GPIO port */
	for (int i = 0; i < OLED_N; i++) {
		rcu_periph_clock_enable(OLED_CLK[i]);
		gpio_mode_set(OLED_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OLED_PIN[i]);
		gpio_output_options_set(OLED_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, OLED_PIN[i]);
		GPIO_BC(OLED_PORT[i]) = OLED_PIN[i];
	}	

	/* configure NPN/PNP GPIO port*/
	for (int i = 0; i < OUT_N; i++) {
		rcu_periph_clock_enable(NPN_CLK[i]);
		gpio_mode_set(NPN_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, NPN_PIN[i]);
		gpio_output_options_set(NPN_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, NPN_PIN[i]);
		GPIO_BC(NPN_PORT[i]) = NPN_PIN[i];

		rcu_periph_clock_enable(PNP_CLK[i]);
		gpio_mode_set(PNP_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PNP_PIN[i]);
		gpio_output_options_set(PNP_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PNP_PIN[i]);
		GPIO_BC(PNP_PORT[i]) = PNP_PIN[i];
	}

	/* configure FUN/FUP GPIO port*/
	for (int i = 0; i < OUT_N; i++) {
		rcu_periph_clock_enable(FUN_CLK[i]);
		gpio_mode_set(FUN_PORT[i], GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, FUN_PIN[i]);

		rcu_periph_clock_enable(FUP_CLK[i]);
		gpio_mode_set(FUP_PORT[i], GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, FUP_PIN[i]);
	}

	rcu_periph_clock_enable(INPUT_CLK);
	gpio_mode_set(INPUT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, INPUT_PIN);

	/* configure ANALOG GPIO port */
	rcu_periph_clock_enable(ANALOG_CLK);
	gpio_mode_set(ANALOG_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ANALOG_PIN);
	gpio_output_options_set(ANALOG_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, ANALOG_PIN);
//	GPIO_BOP(ANALOG_PORT) = ANALOG_PIN;
}

inline bool GPIO_isInputActive (void)
{
	return (SET == gpio_input_bit_get(INPUT_PORT, INPUT_PIN));
}

/* return 1 when key pressed */
static inline bool GPIO_isPb2Gnd (uint8_t index)
{
	return (RESET == GetPb((pb_t)index));
}

static inline bool GPIO_isKeyActive (uint8_t index) 
{	
	if (index > 2) {
		return GPIO_isInputActive();
	}

	return GPIO_isPb2Gnd(index);
}

/* call every 1ms */
void GPIO_PbScan (uint8_t index)
{	
	static button_state_t s_keyState[PB_N + 1] = {kBsIdle};
	static uint16_t s_key_ticks[PB_N + 1] = {0};

	switch (s_keyState[index]) {
	case kBsIdle: {
		if (GPIO_isKeyActive(index)) {
			s_keyState[index] = kBsDithering1; /* press down switch to case 1 */
		}
	} break;

	case kBsDithering1: {
		if (GPIO_isKeyActive(index)) {
			/* dithering elimination */
			s_keyState[index] = kBsWait1;
			s_key_ticks[index] = 0;
		} else {
			s_keyState[index] = kBsIdle;
		}
	} break;

	case kBsWait1: {
		if (!GPIO_isKeyActive(index)) {
			/* release switch to case 3 */
			s_keyState[index] = kBsRelease1;
			s_key_ticks[index] = 0;
		} else if (s_key_ticks[index] > BS_LONG_TICKS) {
			/* long press switch to case 6 */
			g_pressState[index] = kLongClick;
			s_keyState[index] = kBsWait3;
		} else {
			s_key_ticks[index]++;
		}
	} break;

	case kBsRelease1: {
		if (GPIO_isKeyActive(index)) { /* press down again switch to case 4 */
			s_keyState[index] = kBsDithering2;
		} else if (s_key_ticks[index] > BS_SHORT_TMO) {
			/* shot press timeout indicating single pressed */
			g_pressState[index] = kSingleClick;
			s_keyState[index] = kBsIdle;
		} else {
			s_key_ticks[index]++;
		}
	} break;

	case kBsDithering2: {
		if (GPIO_isKeyActive(index)) { /* dithering elimination */
			/* set double pressed */
			g_pressState[index] = kDoubleClick;
			s_keyState[index] = kBsWait2;
		} else {
			s_keyState[index] = kBsRelease1;
		}
	} break;

	case kBsWait2: {
		if (!GPIO_isKeyActive(index)) {
			s_keyState[index] = kBsIdle;
		}
	} break;

	case kBsWait3: {
		if (!GPIO_isKeyActive(index)) {
			/* wait for release then set long pressed */
			g_pressState[index] = kLongClickRelease;
			s_keyState[index] = kBsIdle;
		}
	} break;
	}
}

key_press_state_t GPIO_ReadPbStatus (uint8_t index)
{
	key_press_state_t res = g_pressState[index];
	g_pressState[index] = kNoneClick;
	return res;
}
