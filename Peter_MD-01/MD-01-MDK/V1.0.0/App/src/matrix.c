#include "matrix.h"
#include <math.h>

int Matrix_Identity(Matrix* matrix)
{
	if ((matrix->row != matrix->col)) {
		return 0;
	}
	
	for (int row = 0; row < matrix->row; row++) {
		for (int col = 0; col < matrix->col; col++) {
			if (row == col) {
				matrix->ele[row][col] = 1.0f;
			} else {
				matrix->ele[row][col] = 0;
			}
		}
	}

	return 1;
}

// dst = src1 + src2
int MATRIX_Add(Matrix* src1, Matrix* src2, Matrix* dst)
{
	if (((src1->row != dst->row) || (src2->row != dst->row) || \
	    (src1->col != dst->col) || (src2->col != dst->col))) {
		return 0;
	}

	for (int row = 0; row < src1->row; row++) {
		for (int col = 0; col < src1->col; col++) {
			dst->ele[row][col] = src1->ele[row][col] + src2->ele[row][col];
		}
	}

	return 1;
}

// dst = src1 - src2
int MATRIX_Sub(Matrix* src1, Matrix* src2, Matrix* dst)
{
	if (((src1->row != dst->row) || (src2->row != dst->row) || \
	    (src1->col != dst->col) || (src2->col != dst->col))) {
		return 0;
	}

	for (int row = 0; row < src1->row; row++) {
		for (int col = 0; col < src1->col; col++) {
			dst->ele[row][col] = src1->ele[row][col] - src2->ele[row][col];
		}
	}

	return 1;
}

// dst = src1 * src2
int MATRIX_Multiply(Matrix* src1, Matrix* src2, Matrix* dst)
{
	if ((src1->col != src2->row) || (src1->row != dst->row) || \
	   (src2->col != dst->col)) {
		return 0;
	}

	for (int row = 0; row < dst->row; row++) {
		for (int col = 0; col < dst->col; col++) {
			float temp = 0.0f;
			for (int i = 0; i < src1->col; i++) {
				temp += src1->ele[row][i] * src2->ele[i][col];
			}
			dst->ele[row][col] = temp;
		}
	}

	return 1;
}

// dst = src'
int MATRIX_Transpose(Matrix* src, Matrix* dst)
{
	if ((src->row != dst->col) || (src->col != dst->row)) {
		return 0;
	}

	for (int row = 0; row < dst->row; row++) {
		for (int col = 0; col < dst->col; col++) {
			dst->ele[row][col] = src->ele[col][row];
		}
	}

	return 1;
}

static void swapRow(Matrix *m, int row1, int row2)
{
	float element;
	for (int i = 0; i < m->col; i++) {
		element = m->ele[row1][i];
		m->ele[row1][i] = m->ele[row2][i];
		m->ele[row2][i] = element;
	}
}

static int Gauss_Jordon(Matrix* src, Matrix* dst)
{
	// gauss
	for (int i = 0; i < src->row - 1; i++) {
		// 1.swap to make m[i][i] != 0
		if (src->ele[i][i] == 0) {
			for (int j = i + 1; j < src->row; j++) {
				if (src->ele[j][i] != 0) {
					// TBD: swap(src, i, j, col)
					swapRow(src, i, j);
					swapRow(dst, i, j);
					break;
				}
			}
		}
		if (src->ele[i][i] == 0) {
		/*float pivot = src->ele[i][i];
		if (fabsf(pivot) < 1e-6f) {*/
			return 0;
		}
		// 2.m[j][i] == 0 where j > i
		for (int j = i + 1; j < src->row; j++) {
			float pivot = src->ele[i][i];
			float multiplier = src->ele[j][i] / pivot;
			src->ele[j][i] = 0;
			for (int k = i + 1; k < src->col; k++) {
				src->ele[j][k] -= src->ele[i][k] * multiplier;
			}
			for (int k = 0; k < dst->col; k++) {
				dst->ele[j][k] -= dst->ele[i][k] * multiplier;
			}
		}
	}

	// jordan
	for (int i = src->row - 1; i > 0; i--) {
		// m[j][i] == 0 where j < i
		/*float pivot = src->ele[i][i];
		if (fabsf(pivot) < 1e-6f) {
			return 0;
		}*/
		for (int j = i - 1; j >= 0; j--) {
			float pivot = src->ele[i][i];
			if (pivot == 0) {
				return 0;
			}
			float multiplier = src->ele[j][i] / pivot;
			src->ele[j][i] = 0;
			for (int k = i - 1; k > 0; k--) {
				src->ele[j][k] -= src->ele[i][k] * multiplier;
			}
			for (int k = 0; k < dst->col; k++) {
				dst->ele[j][k] -= dst->ele[i][k] * multiplier;
			}
		}
	}

	// [I A^-1]
	for (int i = 0; i < src->row; i++) {
		float pivot = src->ele[i][i];
		/*if (fabsf(pivot) < 1e-6f) {
			return 0;
		}*/
		for (int k = 0; k < dst->col; k++) {
			dst->ele[i][k] /= pivot;
		}
	}

	return 1;
}

// dst = src^(-1)
int MATRIX_Inverse(Matrix* src, Matrix* dst)
{
	if ((src->row != src->col) || (src->row != dst->row) || \
	   (src->col != dst->col)) {
		return 0;
	}

	if (Matrix_Identity(dst) != 1) {
		return 0;
	}
	if (Gauss_Jordon(src, dst) != 1) {
		return 0;
	}

	return 1;
}
