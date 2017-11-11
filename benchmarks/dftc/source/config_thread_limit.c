/*******************************************************************************
* Copyright 2012-2016 Intel Corporation All Rights Reserved.
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*******************************************************************************/

/*
! Content:
! An example of using DFTI_THREAD_LIMIT configuration parameter.
! The parameter specifies maximum number of OpenMP threads FFT can use.
!
! Values:
!   0 (default) = use number of threads specified by
!                 mkl_[domain_]set_num_threads()
!   Any positive integer N = use not more than N threads
!
!****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_dfti.h"
#include "mkl_service.h"
#if defined(_OPENMP)
#include <omp.h>
#endif

#define REAL double
typedef struct { REAL real,imag; } COMPLEX;

static void init(COMPLEX *x, int N1, int N2, int N3, int H1, int H2, int H3);
static int verify(COMPLEX *x, int N1, int N2, int N3, int H1, int H2, int H3);

/* Define the format to printf MKL_LONG values */
#if !defined(MKL_ILP64)
#define LI "%li"
#else
#define LI "%lli"
#endif


int run_dft
(
 int tid,                       /* Id of this thread */
 int tlimit,                    /* Thread limit */
 int N1, int N2, int N3,        /* Sizes of 3D transform */
 int H1, int H2, int H3         /* Arbitrary harmonic used to verify FFT */
)
{
    /* Execution status */
    MKL_LONG status = 0;

    /* Pointer to input/output data */
    COMPLEX *x = 0;

    DFTI_DESCRIPTOR_HANDLE hand = 0;

    printf("Thread %i: 3D in-place FFT on %i threads\n", tid, tlimit);

    printf("Thread %i: Create DFTI descriptor for %ix%ix%i FFT\n", tid, N1,N2,N3);
    {
        MKL_LONG N[3]; N[0] = N1; N[1] = N2; N[2] = N3;
        status = DftiCreateDescriptor(&hand,
                                      sizeof(REAL)==sizeof(float)
                                      ? DFTI_SINGLE : DFTI_DOUBLE,
                                      DFTI_COMPLEX, 3, N);
        if (0 != status) goto failed;
    }

    printf("Thread %i: Set thread limit %i\n", tid, tlimit);
    status = DftiSetValue(hand, DFTI_THREAD_LIMIT, tlimit);
    if (0 != status) goto failed;


    /* If tlimit > 1 check if we linked with sequential MKL */
    if (tlimit > 1)
    {
        /* Get thread limit of uncommitted descriptor */
        MKL_LONG tl;

        status = DftiGetValue(hand, DFTI_THREAD_LIMIT, &tl);
        if (0 != status) goto failed;

        printf("Thread %i: uncommitted descriptor thread limit %i %s\n",
               tid, (int)tl, tl==1 ? "(sequential MKL)" : "");
    }

    printf("Thread %i: commit descriptor\n", tid);
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    /* Get thread limit of committed descriptor */
    {
        MKL_LONG tl;
        status = DftiGetValue(hand, DFTI_THREAD_LIMIT, &tl);
        if (0 != status) goto failed;

        printf("Thread %i: committed descriptor thread limit %i\n", tid, (int)tl);
    }

    printf("Thread %i: Allocate input/output array\n", tid);
    x = (COMPLEX*)mkl_malloc(N1*N2*N3 * sizeof(COMPLEX), 64);
    if (0 == x) goto failed;

    printf("Thread %i: Initialize input\n", tid);
    init(x, N1, N2, N3, H1, H2, H3);

    printf("Thread %i: Compute forward transform\n", tid);
    status = DftiComputeForward(hand, x);
    if (0 != status) goto failed;

    printf("Thread %i: Verify the result\n", tid);
    status = verify(x, N1, N2, N3, H1, H2, H3);
    if (0 != status) goto failed;

 cleanup:

    printf("Thread %i: Free DFTI descriptor\n", tid);
    DftiFreeDescriptor(&hand);

    printf("Thread %i: Free data array\n", tid);
    mkl_free(x);

    printf("Thread %i: Subtest %s\n", tid, 0==status ? "Passed" : "Failed");
    return status;

 failed:
    printf("Thread %i: ERROR, status = "LI"\n", tid, status);
    status = 1;
    goto cleanup;
}

int main()
{
    /* Number of parallel user threads */
#if defined(_OPENMP)
    int NUT = 2;
#endif

    int failed = 0;
    char version[DFTI_VERSION_LENGTH];

    /* Enable nested parallel OpenMP sections (maybe oversubscribed) */
#if defined(_OPENMP)
    omp_set_nested(1);
    omp_set_dynamic(0);
#endif

    /* Enable threading of MKL called from OpenMP parallel sections */
    MKL_Set_Dynamic(0);

    DftiGetValue(0, DFTI_VERSION, version);
    printf("%s\n", version);

    printf("Example config_thread_limit\n");

#if defined(_OPENMP)
    printf("Run parallel FFTs on %i parallel threads\n",NUT);
#pragma omp parallel num_threads(NUT)
#else
    printf("Run parallel FFT on a single thread\n");
#endif
    {
        /* Two threads running DFT on different number of threads */
        int err;
#if defined(_OPENMP)
        int me = omp_get_thread_num();
        int team = omp_get_num_threads();
#else
        int me = 0;
        int team = 1;
#endif
        if (me == 0)
            printf("Thread %i: parallel team is %i threads\n",me,team);

        if (me)
        {
            err = run_dft(me, 2, 100,200,300, -1,-2,-3);
        }
        else
        {
            err = run_dft(me, 3, 300,100,200, -1,-2,-3);
        }
        if (err)
        {
            failed = err;
        }
    }

    printf("TEST %s\n",failed ? "FAILED" : "PASSED");
    return failed;
}

/* Compute (K*L)%M accurately */
static double moda(int K, int L, int M)
{
    return (double)(((long long)K * L) % M);
}

/* Initialize array with harmonic {H1, H2, H3} */
static void init(COMPLEX *x, int N1, int N2, int N3, int H1, int H2, int H3)
{
    double TWOPI = 6.2831853071795864769, phase;
    int n1, n2, n3, index;

    /* Generalized strides for row-major addressing of x */
    int S1 = N2*N3, S2 = N3, S3 = 1;

    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            for (n3 = 0; n3 < N3; n3++)
            {
                phase =  moda(n1,H1,N1) / N1;
                phase += moda(n2,H2,N2) / N2;
                phase += moda(n3,H3,N3) / N3;
                index = n1*S1 + n2*S2 + n3*S3;
                x[index].real = cos( TWOPI * phase ) / (N1*N2*N3);
                x[index].imag = sin( TWOPI * phase ) / (N1*N2*N3);
            }
        }
    }
}

/* Verify that x(n1,n2,n3) are unit peaks at H1,H2,H3 */
static int verify(COMPLEX *x, int N1, int N2, int N3, int H1, int H2, int H3)
{
    double err, errthr, maxerr;
    int n1, n2, n3, index;

    /* Generalized strides for row-major addressing of x */
    int S1 = N2*N3, S2 = N3, S3 = 1;

    /*
     * Note, this simple error bound doesn't take into account error of
     * input data
     */
    errthr = 5.0 * log( (double)N1*N2*N3 ) / log(2.0)
        * (sizeof(REAL)==sizeof(float) ? FLT_EPSILON : DBL_EPSILON);
    printf(" Verify the result, errthr = %.3lg\n", errthr);

    maxerr = 0;
    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            for (n3 = 0; n3 < N3; n3++)
            {
                double re_exp = 0.0, im_exp = 0.0, re_got, im_got;

                if ((n1-H1)%N1==0 && (n2-H2)%N2==0 && (n3-H3)%N3==0)
                {
                    re_exp = 1;
                }

                index = n1*S1 + n2*S2 + n3*S3;
                re_got = x[index].real;
                im_got = x[index].imag;
                err  = fabs(re_got - re_exp) + fabs(im_got - im_exp);
                if (err > maxerr) maxerr = err;
                if (!(err <= errthr))
                {
                    printf(" x[%i][%i][%i]: ",n1,n2,n3);
                    printf(" expected (%.17lg,%.17lg), ",re_exp,im_exp);
                    printf(" got (%.17lg,%.17lg), ",re_got,im_got);
                    printf(" err %.3lg\n", err);
                    printf(" Verification FAILED\n");
                    return 1;
                }
            }
        }
    }
    printf(" Verified, maximum error was %.3lg\n", maxerr);
    return 0;
}
