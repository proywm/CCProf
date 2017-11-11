/* gemm.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "gemm.h"


/* Array initialization. */
static
void init_array(int ni, int nj, int nk,
		int padA_i, int padA_k,
		int padB_k, int padB_j,
		int padC_i, int padC_j,
		DATA_TYPE *alpha,
		DATA_TYPE *beta,
		DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni+padC_i,nj+padC_j),
		DATA_TYPE POLYBENCH_2D(A,NI,NK,ni+padA_i,nk+padA_k),
		DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk+padB_k,nj+padB_k))
{
  int i, j;

  *alpha = 1.5;
  *beta = 1.2;
  for (i = 0; i < ni + padC_i; i++)
    for (j = 0; j < nj + padC_j; j++)
      C[i][j] = (DATA_TYPE) (i*j % ni) / ni;
  for (i = 0; i < ni + padA_i; i++)
    for (j = 0; j < nk + padA_k; j++)
      A[i][j] = (DATA_TYPE) (i*(j+1) % nk) / nk;
  for (i = 0; i < nk + padB_k; i++)
    for (j = 0; j < nj + padB_j; j++)
      B[i][j] = (DATA_TYPE) (i*(j+2) % nj) / nj;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int ni, int nj,
		 DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni,nj))
{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("C");
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
	if ((i * ni + j) % 20 == 0) fprintf (POLYBENCH_DUMP_TARGET, "\n");
	fprintf (POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, C[i][j]);
    }
  POLYBENCH_DUMP_END("C");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_gemm(int ni, int nj, int nk,
		 int padA_i, int padA_k,
		 int padB_k, int padB_j,
		 int padC_i, int padC_j,
		 DATA_TYPE alpha,
		 DATA_TYPE beta,
		 DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni+padC_i,nj+padC_j),
		 DATA_TYPE POLYBENCH_2D(A,NI,NK,ni+padA_i,nk+padA_k),
		 DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk+padB_k,nj+padB_k))
{
  int i, j, k;

//BLAS PARAMS
//TRANSA = 'N'
//TRANSB = 'N'
// => Form C := alpha*A*B + beta*C,
//A is NIxNK
//B is NKxNJ
//C is NIxNJ
//#pragma scop
  for (i = 0; i < _PB_NI; i++) {
    for (j = 0; j < _PB_NJ; j++)
	C[i][j] *= beta;
    for (k = 0; k < _PB_NK; k++) {
      for (j = 0; j < _PB_NJ; j++)
	C[i][j] += alpha * A[i][k] * B[k][j];
    }
  }
//#pragma endscop

}


int main(int argc, char** argv)
{
  if (argc != 10)
    {
      fprintf (stderr, "Usage: %s <NI> <NJ> <NK> <PadA-I> <PadA-K> <PadB-K> <PadB-J> <PadC-I> <PadC-J>\n", argv[0]);
      exit (1);
    }
  /* Retrieve problem size. */
  int ni = atoi (argv[1]);
  int nj = atoi (argv[2]);
  int nk = atoi (argv[3]);

  int padA_i = atoi (argv[4]);
  int padA_k = atoi (argv[5]);
  int padB_k = atoi (argv[6]);
  int padB_j = atoi (argv[7]);
  int padC_i = atoi (argv[8]);
  int padC_j = atoi (argv[9]);

  /* Variable declaration/allocation. */
  DATA_TYPE alpha;
  DATA_TYPE beta;
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NI,NJ,ni + padC_i, nj + padC_j);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NK,ni + padA_i, nk + padA_k);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NK,NJ,nk + padB_k, nj + padB_j);

  /* Initialize array(s). */
  init_array (ni, nj, nk,
	      padA_i, padA_k, padB_k, padB_j, padC_i, padC_j,
	      &alpha, &beta,
	      POLYBENCH_ARRAY(C),
	      POLYBENCH_ARRAY(A),
	      POLYBENCH_ARRAY(B));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_gemm (ni, nj, nk,
	       padA_i, padA_k, padB_k, padB_j, padC_i, padC_j,
	       alpha, beta,
	       POLYBENCH_ARRAY(C),
	       POLYBENCH_ARRAY(A),
	       POLYBENCH_ARRAY(B));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(ni+padC_i, nj+padC_j,  POLYBENCH_ARRAY(C)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(C);
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
