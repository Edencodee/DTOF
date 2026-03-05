#ifndef __GAUSS_NEWTON_H
#define __GAUSS_NEWTON_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stddef.h>
#include "matrix.h"
#include "user_flash.h"

#define COEFFI_SIZE      4
#define AVAIL_COEFF      3

	enum {
		kxc = 0,
		kw,
		kA,
		ky0
	};

	typedef struct {
		float b[COEFFI_SIZE];
		uint16_t *xs;
		uint16_t *ys;
	} GaussNewton_Typedef;

	extern GaussNewton_Typedef gt;

	void GAUSS_NEWTON_setSource(uint16_t* x, uint16_t* y);
	void GAUSS_NEWTON_solveGauss(int center, uint16_t times);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
