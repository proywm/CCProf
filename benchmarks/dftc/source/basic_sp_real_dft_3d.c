/*******************************************************************************
* Copyright 2011-2016 Intel Corporation All Rights Reserved.
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
! A simple example of single-precision real-to-complex in-place 3D
! FFT using MKL DFTI
!
!****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_service.h"
#include "mkl_dfti.h"

/* Define the format to printf MKL_LONG values */
#if !defined(MKL_ILP64)
#define LI "%li"
#else
#define LI "%lli"
#endif

static void init_r(float *x, int N1, int N2, int N3, int H1, int H2, int H3);
static void init_c(MKL_Complex8 *x, int N1, int N2, int N3, int H1, int H2,
                   int H3);
static int verify_r(float *x, int N1, int N2, int N3, int H1, int H2, int H3);
static int verify_c(MKL_Complex8 *x, int N1, int N2, int N3, int H1, int H2,
                    int H3);

/* Define the format to printf MKL_LONG values */
#if !defined(MKL_ILP64)
#define LI "%li"
#else
#define LI "%lli"
#endif

int main(void)
{
    /* Size of 3D transform */
    int N1 = 4, N2 = 5, N3 = 13;

    /* Arbitrary harmonic used to verify FFT */
    int H1 = -1, H2 = 2, H3 = 4;

    /* Execution status */
    MKL_LONG status = 0;

    /* Pointer to input/output data */
    float *x = 0;

    /* Strides describe data layout in real and conjugate-even domain */
    MKL_LONG rs[4], cs[4];

    DFTI_DESCRIPTOR_HANDLE hand = 0;

    char version[DFTI_VERSION_LENGTH];

    DftiGetValue(0, DFTI_VERSION, version);
    printf("%s\n", version);

    printf("Example basic_sp_real_dft_3d\n");
    printf("Forward-Backward single-precision in-place 3D real FFT\n");
    printf("Configuration parameters:\n");
    printf(" DFTI_PRECISION                = DFTI_SINGLE\n");
    printf(" DFTI_FORWARD_DOMAIN           = DFTI_REAL\n");
    printf(" DFTI_DIMENSION                = 3\n");
    printf(" DFTI_LENGTHS                  = {%d, %d, %d}\n", N1, N2, N3);
    printf(" DFTI_PLACEMENT                = DFTI_INPLACE\n");
    printf(" DFTI_CONJUGATE_EVEN_STORAGE   = DFTI_COMPLEX_COMPLEX\n");


    printf("Create DFTI descriptor\n");
    {
        MKL_LONG N[3]; N[0] = N1; N[1] = N2; N[2] = N3;
        status = DftiCreateDescriptor(&hand, DFTI_SINGLE, DFTI_REAL, 3, N);
        if (0 != status) goto failed;
    }

    /* This is not needed, default setting */
    /* status = DftiSetValue(hand, DFTI_PLACEMENT, DFTI_INPLACE); */
    /* if (0 != status) goto failed; */


    printf("Set configuration: CCE storage\n");
    status = DftiSetValue(hand, DFTI_CONJUGATE_EVEN_STORAGE,
                          DFTI_COMPLEX_COMPLEX);
    if (0 != status) goto failed;

    /* Compute strides */
    rs[3] = 1;             cs[3] = 1;
    rs[2] = (N3/2+1)*2;    cs[2] = (N3/2+1);
    rs[1] = N2*(N3/2+1)*2; cs[1] = N2*(N3/2+1);
    rs[0] = 0;             cs[0] = 0;

    printf("Set input  strides = { "LI", "LI", "LI", "LI" }\n",
           rs[0],rs[1],rs[2],rs[3]);
    status = DftiSetValue(hand, DFTI_INPUT_STRIDES, rs);
    if (0 != status) goto failed;

    printf("Set output strides = { "LI", "LI", "LI", "LI" }\n",
           cs[0],cs[1],cs[2],cs[3]);
    status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, cs);
    if (0 != status) goto failed;

    printf("Commit the descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Allocate data array\n");
    x = (float *)mkl_malloc(N1*N2*(N3/2+1)*2*sizeof(float), 64);
    if (0 == x) goto failed;

    printf("Initialize data for r2c transform\n");
    init_r(x, N1, N2, N3, H1, H2, H3);

    printf("Compute real-to-complex in-place transform\n");
    status = DftiComputeForward(hand, x);
    if (0 != status) goto failed;

    printf("Verify the result\n");
    status = verify_c((MKL_Complex8*)x, N1, N2, N3, H1, H2, H3);
    if (0 != status) goto failed;

    printf("Change strides to compute backward transform\n");
    status = DftiSetValue(hand, DFTI_INPUT_STRIDES, cs);
    if (0 != status) goto failed;
    status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, rs);
    if (0 != status) goto failed;

    printf("Commit the descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Initialize data for c2r transform\n");
    init_c((MKL_Complex8*)x, N1, N2, N3, H1, H2, H3);

    printf("Compute backward transform\n");
    status = DftiComputeBackward(hand, x);
    if (0 != status) goto failed;

    printf("Verify the result\n");
    status = verify_r(x, N1, N2, N3, H1, H2, H3);
    if (0 != status) goto failed;

 cleanup:

    printf("Free DFTI descriptor\n");
    DftiFreeDescriptor(&hand);

    printf("Free data array\n");
    mkl_free(x);

    printf("TEST %s\n",0==status ? "PASSED" : "FAILED");
    return status;

 failed:
    printf(" ERROR, status = "LI"\n", status);
    status = 1;
    goto cleanup;
}

/* Compute (K*L)%M accurately */
static float moda(int K, int L, int M)
{
    return (float)(((long long)K * L) % M);
}

/* Initialize array x(N) to produce unit peaks at x(H) and x(N-H) */
static void init_r(float *x, int N1, int N2, int N3, int H1, int H2, int H3)
{
    float TWOPI = 6.2831853071795864769f, phase, factor;
    int n1, n2, n3, S1, S2, S3, index;

    /* Generalized strides for row-major addressing of x */
    S3 = 1;
    S2 = (N3/2+1)*2;
    S1 = N2*(N3/2+1)*2;

    factor = (2*(N1-H1)%N1==0 && 2*(N2-H2)%N2==0 && 2*(N3-H3)%N3==0) ? 1.0f : 2.0f;
    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            for (n3 = 0; n3 < N3; n3++)
            {
                phase  = moda(n1,H1,N1) / N1;
                phase += moda(n2,H2,N2) / N2;
                phase += moda(n3,H3,N3) / N3;
                index = n1*S1 + n2*S2 + n3*S3;
                x[index] = factor * cosf( TWOPI * phase ) / (N1*N2*N3);
            }
        }
    }
}

/* Verify that x has unit peak at H */
static int verify_c(MKL_Complex8 *x,int N1,int N2,int N3,int H1,int H2,int H3)
{
    float err, errthr, maxerr;
    int n1, n2, n3, S1, S2, S3, index;

    /* Generalized strides for row-major addressing of x */
    S3 = 1;
    S2 = N3/2+1;
    S1 = N2*(N3/2+1);

    /*
     * Note, this simple error bound doesn't take into account error of
     * input data
     */
    errthr = 2.5f * logf( (float)N1*N2*N3 ) / logf(2.0f) * FLT_EPSILON;
    printf(" Check if err is below errthr %.3lg\n", errthr);

    maxerr = 0;
    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            for (n3 = 0; n3 < N3/2+1; n3++)
            {
                float re_exp = 0.0f, im_exp = 0.0f, re_got, im_got;

                if ((( n1-H1)%N1==0 && ( n2-H2)%N2==0 && ( n3-H3)%N3==0) ||
                    ((-n1-H1)%N1==0 && (-n2-H2)%N2==0 && (-n3-H3)%N3==0))
                {
                    re_exp = 1;
                }

                index = n1*S1 + n2*S2 + n3*S3;
                re_got = x[index].real;
                im_got = x[index].imag;
                err  = fabsf(re_got - re_exp) + fabsf(im_got - im_exp);
                if (err > maxerr) maxerr = err;
                if (!(err < errthr))
                {
                    printf(" x[%i][%i][%i]: ",n1,n2,n3);
                    printf(" expected (%.7g,%.7g), ",re_exp,im_exp);
                    printf(" got (%.7g,%.7g), ",re_got,im_got);
                    printf(" err %.3lg\n", err);
                    printf(" Verification FAILED\n");
                    return 1;
                }
            }
        }
    }
    printf(" Verified,  maximum error was %.3lg\n", maxerr);
    return 0;
}

/* Initialize array x(N) to produce unit peak at x(H) */
static void init_c(MKL_Complex8 *x,int N1,int N2,int N3,int H1,int H2,int H3)
{
    float TWOPI = 6.2831853071795864769f, phase;
    int n1, n2, n3, S1, S2, S3, index;

    /* Generalized strides for row-major addressing of x */
    S3 = 1;
    S2 = N3/2+1;
    S1 = N2*(N3/2+1);

    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            for (n3 = 0; n3 < N3/2+1; n3++)
            {
                phase  = moda(n1,H1,N1) / N1;
                phase += moda(n2,H2,N2) / N2;
                phase += moda(n3,H3,N3) / N3;
                index = n1*S1 + n2*S2 + n3*S3;
                x[index].real =  cosf( TWOPI * phase ) / (N1*N2*N3);
                x[index].imag = -sinf( TWOPI * phase ) / (N1*N2*N3);
            }
        }
    }
}

/* Verify that x has unit peak at H */
static int verify_r(float *x, int N1, int N2, int N3, int H1, int H2, int H3)
{
    float err, errthr, maxerr;
    int n1, n2, n3, S1, S2, S3, index;

    /* Generalized strides for row-major addressing of x */
    S3 = 1;
    S2 = (N3/2+1)*2;
    S1 = N2*(N3/2+1)*2;

    /*
     * Note, this simple error bound doesn't take into account error of
     * input data
     */
    errthr = 2.5f * logf( (float)N1*N2*N3 ) / logf(2.0f) * FLT_EPSILON;
    printf(" Check if err is below errthr %.3lg\n", errthr);

    maxerr = 0;
    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            for (n3 = 0; n3 < N3; n3++)
            {
                float re_exp = 0.0f, re_got;

                if ((n1-H1)%N1==0 && (n2-H2)%N2==0 && (n3-H3)%N3==0)
                {
                    re_exp = 1;
                }

                index = n1*S1 + n2*S2 + n3*S3;
                re_got = x[index];
                err  = fabsf(re_got - re_exp);
                if (err > maxerr) maxerr = err;
                if (!(err < errthr))
                {
                    printf(" x[%i][%i][%i]: ",n1,n2,n3);
                    printf(" expected %.7g, ",re_exp);
                    printf(" got %.7g, ",re_got);
                    printf(" err %.3lg\n", err);
                    printf(" Verification FAILED\n");
                    return 1;
                }
            }
        }
    }
    printf(" Verified,  maximum error was %.3lg\n", maxerr);
    return 0;
}
