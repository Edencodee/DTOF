#include "gauss_newton.h"
#include <math.h>
#include "wtdint.h"

#define PI          3.141593f
#define SQRT_PI_2   1.253314f

GaussNewton_Typedef gt = {{0.0f}, NULL, NULL};

static float g_witem = 0.0f;
static float g_awitem = 0.0f;
static float g_dxs[MATRIX_SIZE] = {0.0f};
static float g_eitems[MATRIX_SIZE] = {0.0f};
static float g_fit[MATRIX_SIZE] = {0.0f};

void GAUSS_NEWTON_setSource (uint16_t* x, uint16_t* y)
{
	gt.xs = x;
	gt.ys = y;
}

static uint16_t getAverage (uint16_t* p, uint16_t size)
{
	int sum = 0;
	for (int i = 0; i < size; i++) {
		sum += p[i];
	}
	return sum / size;
}

// Gauss: y0+A/(w*sqrt(pi/2))*exp(-2*((x-xc)/w)^2)
static void Gauss_InitPara (int center)
{
	/*
		[Derived Parameters]
		sigma = w/2; // sigma.
		FWHM = sqrt(2*ln(2)) * w; // Full Width at Half Maximum.
		Height = A/(w*sqrt(PI/2)); // Height of the Peak.

		[FORMULA]
		y=y0 + (A/(w*sqrt(PI/2)))*exp(-2*((x-xc)/w)^2)

	    [Parameters Initialization]
	    float h;
	    xc = peak_pos(x_y_curve, &w, &y0, &A, &h);
	    A = (h>=0) ? abs(A):-abs(A);
	    w *= 0.85;
	*/
	
	/* 1. init y0 */
	int mid = center; // MATRIX_SIZE / 2;
	int btmAve = getAverage(gt.ys, 2);
	int topAve = getAverage(gt.ys + MATRIX_SIZE - 3, 2);
	int y0 = btmAve > topAve ? topAve : btmAve;
	gt.b[ky0] = y0;
	/* 2. init xc */
	gt.b[kxc] = gt.xs[mid];
	/* 3. init A */
	int A = 0;
#if CONTINUE_Y
	for (int i = 0; i < MATRIX_SIZE; i++) {
		A += gt.ys[i] - y0;
	}
	gt.b[kA] = A * g_sreg.interval;
#else
	for (int i = 1; i < MATRIX_SIZE; i++) {
		A += ((gt.ys[i] - y0) + (gt.ys[i - 1] - y0)) * (gt.xs[i] - gt.xs[i - 1]) / 2;
	}
	gt.b[kA] = A;
#endif
	/* 4. init w */
	int half = (gt.ys[mid] + y0) / 2;
	int btm = 0;
	int top = 0;
	for (int i = 0; i < mid; i++) {
		if (gt.ys[i] > half) {
			btm = i;
			break;
		}
	}
	for (int i = mid; i < MATRIX_SIZE; i++) {
		if (gt.ys[i] < half) {
			top = i;
			break;
		}
	}
	gt.b[kw] = (gt.xs[top] - gt.xs[btm]) * 1.0955f; // fwhm/sqrt(2log(2))*0.85
}

static void Gauss (float *g_fit)
{
	/*
	[FORMULA]
		y=y0 + (A/(w*sqrt(PI/2)))*exp(-2*((x-xc)/w)^2)
	*/
	g_witem = gt.b[kw] * SQRT_PI_2; // w*sqrt(pi/2)
	g_awitem = gt.b[kA] / g_witem; // A/w*sqrt(pi/2)
	for (int i = 0; i < MATRIX_SIZE; i++) {
		float dx = gt.xs[i] - gt.b[kxc];
		g_dxs[i] = dx;
		float eitem = expf(-2 * powf((dx / gt.b[kw]), 2)); // exp(-2*((x-xc)/w)^2)
		g_eitems[i] = eitem; // backup for derivative
		g_fit[i] = gt.b[ky0] + g_awitem * eitem; // y0+A/(w*sqrt(pi/2))*exp(-2*((x-xc)/w)^2)
	}
}

static float Gauss_Derivative (float dx, int index, float eitem)
{
	float res = 0;
	static float s_aeww = 0.0f;
	static float s_ew = 0.0f;
	static float s_dxc = 0.0f;
	static float s_xcw = 0.0f;

	switch(index) {
	case kxc: {
		s_ew = eitem / g_witem;  // exp(-2*((x-xc)/w)^2)/(w*sqrt(pi/2))
		s_aeww = gt.b[kA] * s_ew / gt.b[kw];   // A*exp(-2*((x-xc)/w)^2)/(w^2*sqrt(pi/2))
		s_xcw = dx / gt.b[kw];	// (x-xc)/w
		res = 4 * s_xcw * s_aeww;   // 4A(x-xc)exp(-2*((x-xc)/w)^2)/(w^3*sqrt(pi/2))
		s_dxc = res;
	} break;
	case kw: {	// 4A(x-xc)^2*exp(-2*((x-xc)/w)^2)/(w^4*sqrt(pi/2)) - A*exp(-2*((x-xc)/w)^2)/(w^2*sqrt(pi/2))
		res = s_dxc * s_xcw - s_aeww;
	} break;
	case kA: {
		res = s_ew;  // exp(-2*((x-xc)/w)^2)/(w*sqrt(pi/2))
	} break;
	case ky0: {
		res = 1;
	} break;
	}

	return res;
}

static void JacobinMatrix (Matrix *jac, float (*d_func)(float, int, float))
{
	for (int i = 0; i < MATRIX_SIZE; i++) {
		for (int j = 0; j < AVAIL_COEFF; j++) {
			jac->ele[i][j] = d_func(g_dxs[i], j, g_eitems[i]);
		}
	}
}

static bool GAUSS_NEWTON_solve (void (*func)(float*), float (*d_func)(float, int, float), int times)
{	
	static Matrix jac    = {MATRIX_SIZE, AVAIL_COEFF, {{1.0f}}};  	// jac[m, n]
	static Matrix jac_t  = {AVAIL_COEFF, MATRIX_SIZE, {{1.0f}}};  	// jac_t[n, m]
	static Matrix hes    = {AVAIL_COEFF, AVAIL_COEFF, {{1.0f}}};  	// (jac_t*jac)[n, n]
	static Matrix inv    = {AVAIL_COEFF, AVAIL_COEFF, {{1.0f}}};  	// ((jac_t*jac)^-1)[n, n]
	static Matrix inv_jt = {AVAIL_COEFF, MATRIX_SIZE, {{1.0f}}};	// ((jac_t*jac)^-1*jac_t)[n, m]
	static Matrix r      = {MATRIX_SIZE, 1, {{1.0f}}};				// r[m, 1]
	static Matrix shift  = {AVAIL_COEFF, 1, {{1.0f}}};				// ((jac_t*jac)^-1*jac_t*r)[n, 1]

	for (int j = 0; j < times; j++) {
		func(g_fit);

		JacobinMatrix(&jac, d_func);                        		// generate jac

		if (MATRIX_Transpose(&jac, &jac_t) != 1) {           		// jac_t = transpose jac
			return false;
		}
		if (MATRIX_Multiply(&jac_t, &jac, &hes) != 1) {      		// hes = jac_t*jac
			return false;
		}

		if (MATRIX_Inverse(&hes, &inv) != 1) {               		// inv = hes^-1 = (jac_t*jac)^-1
			return false;
		}

		if (MATRIX_Multiply(&inv, &jac_t, &inv_jt) != 1) {   		// inv_jt = inv*jac_t = (jac_t*jac)^-1*jac_t
			return false;
		}

		for (int i = 0; i < MATRIX_SIZE; i++) {
			r.ele[i][0] = gt.ys[i] - g_fit[i];              		// ri = yi - f(xi, b)
		}

		if (MATRIX_Multiply(&inv_jt, &r, &shift) != 1) {    		// shift = (jac_t*jac)^-1*jac_t*r
			return false;
		}

		for (int i = 0; i < AVAIL_COEFF; i++) {             		// iteration

			gt.b[i] += shift.ele[i][0];
		}
	}
	return true;
}

void GAUSS_NEWTON_solveGauss (int center, uint16_t times)
{
	Gauss_InitPara(center);
	GAUSS_NEWTON_solve(Gauss, Gauss_Derivative, times);
}
