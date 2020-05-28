/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 * Created by Michael Minogue. A20388449
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{/*Checklist:
  No more than 12 local variables +
  No funny bit tricks (longs,arrays, etc ) +
  No warnings during compilation +
  No modifying array A +
  No recursion +
  */
  //Current local variable count: 6
  int i, j;
	int row;
	int column;
	int dia = 0;
	int storage = 0;

	if (N == 32 && M == 32) {  //Case for 32x32
		for (column = 0; column < N; column += 8) {
			for (row = 0; row < N; row += 8) {
				for (i = row; i < row + 8; i++) {
					for (j = column; j < column + 8; j++) {
						if (i != j) {
							B[j][i] = A[i][j];
						} else {

							storage = A[i][j];
							dia = i;
						}
					}
					if (row == column) {
						B[dia][dia] = storage;
					}
				}
			}
		}
	} else if (N == 64 && M == 64) { //Case for 64x64
		for (column = 0; column < N; column += 4) {
			for (row = 0; row < M; row += 4) {
        //I attempted to add elements manually to figure out
        //the abnormally high error rate. I was unsuccessful but kept my work here.
        				B[row+3][column] = A[column][row+3];
        				B[row+3][column+1] = A[column+1][row+3];
        				B[row+3][column+2] = A[column+2][row+3];
        				B[row+2][column] = A[column][row+2];
        				B[row+2][column+1] = A[column+1][row+2];
        				B[row+2][column+2] = A[column+2][row+2];
        				B[row+1][column] = A[column][row+1];
        				B[row+1][column+1] = A[column+1][row+1];
        				B[row+1][column+2] = A[column+2][row+1];
        				B[row][column] = A[column][row];
        				B[row][column+1] = A[column+1][row];
        				B[row][column+2] = A[column+2][row];
        				B[row][column+3] = A[column+3][row];
        				B[row+1][column+3] = A[column+3][row+1];
        				B[row+2][column+3] = A[column+3][row+2];
        				B[row+3][column+3] = A[column+3][row+3];
			}
		}
	}
	else { //61x67 case
		for (column = 0; column < M; column += 16) {
			for (row = 0; row < N; row += 16) {
				for (i = row; (i < row + 16) && (i < N); i++) {
					for (j = column; (j < column + 16) && (j < M); j++) {
						if (i != j) {
							B[j][i] = A[i][j];
						} else {
							storage = A[i][j];
							dia = i;
						}
					}
					if (row == column) {
						B[dia][dia] = storage;
					}
				}
	 		}
		}
	}
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 *    Note for Michael: Do not edit. Comes correct.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
