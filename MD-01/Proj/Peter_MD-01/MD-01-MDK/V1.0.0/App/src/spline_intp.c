#include "main.h"

typedef struct {
	int row;
	int col;
	ele_t ele[PN][PN];
} mat2_t;

static void swapRow2 (mat2_t *m, int row1, int row2)
{
	ele_t temp;

	for (int i = 0; i < m->col; i++) {
		temp = m->ele[row1][i];
		m->ele[row1][i] = m->ele[row2][i];
		m->ele[row2][i] = temp;
	}
}

static int Gauss_Jordon2 (mat2_t* src, mat2_t* dst)
{
	/* 1. gauss */
	for (int i = 0; i < src->row - 1; i++) {
		/* 1.1. swap to make m[i][i] != 0 */
		if (src->ele[i][i] == 0) {
			for (int j = i + 1; j < src->row; j++) {
				if (src->ele[j][i] != 0) {
					// TBD: swap(src, i, j, col)
					swapRow2(src, i, j);
					swapRow2(dst, i, j);
					break;
				}
			}
		}
		ele_t piv = src->ele[i][i];
		if (fabsf(piv) < 1e-6f) {
			return 0;
		}
		/* 1.2. m[j][i] == 0 where j > i */
		for (int j = i + 1; j < src->row; j++) {
			ele_t mul = src->ele[j][i] / piv;
			src->ele[j][i] = 0;
			for (int k = i + 1; k < src->col; k++) {
				src->ele[j][k] -= src->ele[i][k] * mul;
			}
			for (int k = 0; k < dst->col; k++) {
				dst->ele[j][k] -= dst->ele[i][k] * mul;
			}
		}
	}

	/* 2. jordan */
	for (int i = src->row - 1; i > 0; i--) {
		ele_t piv = src->ele[i][i];
		if (fabsf(piv) < 1e-6f) {
			return 0;
		}
		/* 2.1. m[j][i] == 0 where j < i */
		for (int j = i - 1; j >= 0; j--) {
			ele_t mul = src->ele[j][i] / piv;
			src->ele[j][i] = 0;
			for (int k = i - 1; k > 0; k--) {
				src->ele[j][k] -= src->ele[i][k] * mul;
			}
			for (int k = 0; k < dst->col; k++) {
				dst->ele[j][k] -= dst->ele[i][k] * mul;
			}
		}
	}

	/* 3. [I A^-1] */
	for (int i = 0; i < src->row; i++) {
		ele_t piv = src->ele[i][i];
		for (int k = 0; k < dst->col; k++) {
			dst->ele[i][k] /= piv;
		}
	}

	return 1;
}

void SplineInterp (ele_t* y, ele_t* a, ele_t* b, ele_t* c, ele_t* d) /* only for x[i+1]-x[i] = 1 */
{
	mat2_t src = {PN, PN, {{0.0f}}};
	mat2_t dst = {PN,  1, {{0.0f}}};

	src.ele[0][0] = 1;
	for (int i = 0; i < PN - 2; i++) {
		src.ele[i + 1][i] = 1; 		/* h[i], where h[i] = x[i+1]-x[i] */
		src.ele[i + 1][i + 1] = 4;	/* 2(h[i] + h[i+1])               */
		src.ele[i + 1][i + 2] = 1;	/* h[i+1]                         */
	}
	src.ele[PN - 1][PN - 1] = 1;

	ele_t dy[PN - 1];
	dy[0] = y[1] - y[0];

	dst.ele[0][0] = 0;
	for (int i = 0; i < PN - 2; i++) {
		dy[i + 1] = y[i + 2] - y[i + 1];
		dst.ele[i + 1][0] = dy[i + 1] - dy[i];	/* (y[i+2]-y[i+1])/h[i+1] - (y[i+1]-y[i])/h[i] */
		dst.ele[i + 1][0] *= 6.0f;
	}
	dst.ele[PN - 1][0] = 0;

	Gauss_Jordon2(&src, &dst);

	ele_t m[PN];
	for (int i = 0; i < PN; i++) {
		m[i] = dst.ele[i][0];
	}
	memcpy(a, y, 4 * (PN - 1));		/* a[i] = y[i] */
	for (int i = 0; i < PN - 1; i++) {
		float m_2 = m[i] / 2.0f;
		float dm_6 = (m[i + 1] - m[i]) / 6.0f;
//		a[i] = y[i];
		b[i] = dy[i] - m_2 - dm_6;	/* b[i] = (y[i+1]-y[i])/hi - hi*m[i]/2 - hi*(m[i+1]-m[i])/6 */
		c[i] = m_2;		/* c[i] = m[i]/2 */
		d[i] = dm_6;	/* d[i] = (m[i+1]-m[i])/(6*hi) */
	}
}
