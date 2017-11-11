/* gemm.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#ifdef BENCH_AVX2
#include "immintrin.h"
#endif

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
		 int TtI, int TtJ, int TtK,
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
#ifdef ceild
# undef ceild
#endif
#ifdef floord
# undef floord
#endif
#ifdef max
# undef max
#endif
#ifdef min
# undef min
#endif
#define ceild(x,y) (((x) > 0)? (1 + ((x) - 1)/(y)): ((x) / (y)))
#define floord(x,y) (((x) > 0)? ((x)/(y)): 1 + (((x) -1)/ (y)))
#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))


  register int lbv, ubv, lb, ub, lb1, ub1, lb2, ub2;
  register int c1, c2, c3;
  register int c1t, c2t, c3t;
#pragma scop
if ((_PB_NI >= 1) && (_PB_NJ >= 1)) {
  for (c1t = 0; c1t <= (_PB_NI + -1); c1t += TtI)
    for (c2t = 0; c2t <= (_PB_NJ + -1); c2t += TtJ)
      for (c1 = c1t; c1 <= min((_PB_NI + -1),c1t + TtI); c1++)
	for (c2 = c2t; c2 <= min((_PB_NJ + -1), c2t + TtJ); c2++)
	  C[c1][c2] *= beta;
  }
  if (_PB_NK >= 1) {
#ifdef BENCH_AVX2
    __m256d alpha_v = _mm256_broadcast_sd (&(alpha));
#endif    
    for (c1t = 0; c1t <= (_PB_NI + -1); c1t += TtI)
      for (c2t = 0; c2t <= (_PB_NJ + -1); c2t += TtJ)
	for (c3t = 0; c3t <= (_PB_NK + -1); c3t += TtK)
	  for (c1 = c1t; c1 <= min((_PB_NI + -1),c1t + TtI); c1++)
	    for (c3 = c3t; c3 <= min((_PB_NK + -1), c3t + TtK); c3++)
	      {
#ifdef BENCH_AVX2
  __m256d prod = _mm256_mul_pd (_mm256_broadcast_sd (&(A[c1][c3])), alpha_v);
  for (c2 = c2t; c2 <= min((_PB_NJ + -1), c2t + TtJ) - 16; c2 += 16)
    {
  _mm256_storeu_pd(&(C[c1][c2]),
    _mm256_fmadd_pd(prod,
    _mm256_loadu_pd(&(B[c3][c2])),
    _mm256_loadu_pd(&(C[c1][c2]))));

  _mm256_storeu_pd(&(C[c1][c2+4]),
    _mm256_fmadd_pd(prod,
    _mm256_loadu_pd(&(B[c3][c2+4])),
    _mm256_loadu_pd(&(C[c1][c2+4]))));

  _mm256_storeu_pd(&(C[c1][c2+8]),
    _mm256_fmadd_pd(prod,
    _mm256_loadu_pd(&(B[c3][c2+8])),
    _mm256_loadu_pd(&(C[c1][c2+8]))));

  _mm256_storeu_pd(&(C[c1][c2+12]),
    _mm256_fmadd_pd(prod,
    _mm256_loadu_pd(&(B[c3][c2+12])),
    _mm256_loadu_pd(&(C[c1][c2+12]))));

}
  for (; c2 <= min((_PB_NJ + -1), c2t + TtJ); c2++)
    C[c1][c2] += alpha * A[c1][c3] * B[c3][c2];

#else  
  //// CODE BELOW: scalar code, but good perf.
  DATA_TYPE local_tmp = alpha * A[c1][c3];
  int loop_bound = min((_PB_NJ + -1), c2t + TtJ) - 4;
  for (c2 = c2t; c2 <= loop_bound; c2 += 1)
    C[c1][c2] += local_tmp * B[c3][c2];
  for (; c2 <= min((_PB_NJ + -1), c2t + TtJ); c2++)
    C[c1][c2] += local_tmp * B[c3][c2];

  /// Code below: 3x slower with GCC 4.9.
  /* for (c2 = c2t; c2 <= min((_PB_NJ + -1), c2t + TtJ); c2++) */
  /*   C[c1][c2] += alpha * A[c1][c3] * B[c3][c2]; */
#endif  
  }  }
#pragma endscop


}


int main(int argc, char** argv)
{
  if (argc != 13)
    {
      fprintf (stderr, "Usage: %s <NI> <NJ> <NK> <PadA-I> <PadA-K> <PadB-K> <PadB-J> <PadC-I> <PadC-J> <TtI> <TtJ> <Ttk>\n", argv[0]);
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

  int TtI = atoi (argv[10]);
  int TtJ = atoi (argv[11]);
  int TtK = atoi (argv[12]);

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
	       TtI, TtJ, TtK,
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

