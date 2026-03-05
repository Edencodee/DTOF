#ifndef __MATRIX_H
#define __MATRIX_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

	typedef struct {
		int row;
		int col;
		float ele[MATRIX_SIZE][MATRIX_SIZE];
	} Matrix;

	/* return value: 1 succeed, 0 error */
	int Matrix_Identity(Matrix* matrix);
	int MATRIX_Add(Matrix* src1, Matrix* src2, Matrix* dst);
	int MATRIX_Sub(Matrix* src1, Matrix* src2, Matrix* dst);
	int MATRIX_Multiply(Matrix* src1, Matrix* src2, Matrix* dst);
	int MATRIX_Transpose(Matrix* src, Matrix* dst);
	int MATRIX_Determinant(Matrix* matrix, float *det);
	int MATRIX_Cofactor(Matrix* src, float* res, int row, int col);
	int MATRIX_Adjoint(Matrix* src, Matrix* dst);
	int MATRIX_Inverse(Matrix* src, Matrix* dst);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
