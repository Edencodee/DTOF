#ifndef __WTDINT_H
#define __WTDINT_H

#ifdef cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define getMax(a,b) ((a)>(b))?(a):(b)
#define getMin(a,b) ((a)<(b))?(a):(b)

/* log ------------------------------------------------------------------ */
extern volatile bool g_isLogEn;

#define wlog(format, ...) \
	do { \
		if(g_isLogEn) { \
			printf("%s(%d):", __func__, __LINE__); \
			printf(format, ##__VA_ARGS__); \
			printf("\r\n"); \
		} \
	} while (0)
	
#define info(format, ...) \
	do { \
		if(g_isLogEn) { \
			printf("  "); \
			printf(format, ##__VA_ARGS__); \
			printf("\r\n"); \
		} \
	} while (0)
	
#define rtt_log(format, ...) \
	do { \
		SEGGER_RTT_printf(0, "%s(%d):", __func__, __LINE__); \
		SEGGER_RTT_printf(0, format, ##__VA_ARGS__); \
		SEGGER_RTT_printf(0, "\r\n"); \
	} while (0)
/* ---------------------------------------------------------------------- */

	// Little-Endian
	typedef enum {
		kLsb = 0,
		kMsb
	} endian_t;

	typedef union {
		uint16_t word;
		uint8_t buf[2];
	} u16_t;
	
	typedef union {
		uint32_t word;
		uint8_t buf[4];
		uint16_t buf16[2];
	} u32_t;
	
	typedef union {
		int32_t word;
		uint8_t buf[4];
		uint16_t buf16[2];
	} s32_t;
	
	typedef union {
		float valf;
		uint8_t buf[4];
		uint16_t buf16[2];
	} fp32_t;

#ifdef cplusplus
}
#endif
	
#endif
