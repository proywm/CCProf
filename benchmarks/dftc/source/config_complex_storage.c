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
! An example of using MKL DFTI configuration parameter DFTI_COMPLEX_STORAGE.
! The parameter is used to select how complex data is laid out in memory
! (DFTI_FORWARD_DOMAIN = DFTI_COMPLEX is required).
!
! Values:
! DFTI_COMPLEX_COMPLEX (default) - use array of complex data
! DFTI_REAL_REAL                 - use two arrays of real data (split complex)
!
!****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_service.h"
#include "mkl_dfti.h"

static void init(double *x_re, double *x_im, MKL_LONG N1, MKL_LONG N2,
                 MKL_LONG H1, MKL_LONG H2);
static int verify(double *x_re, double *x_im, MKL_LONG N1, MKL_LONG N2,
                  MKL_LONG H1, MKL_LONG H2);

/* Define the format to printf MKL_LONG values */
#if !defined(MKL_ILP64)
#define LI "%li"
#else
#define LI "%lli"
#endif


int main(void)
{
    /* Sizes of 2D transform */
    MKL_LONG N[] = { 7, 13 };

    /* Arbitrary harmonic to verify FFT */
    MKL_LONG H[] = { 1, -1 };

    /* Execution status */
    MKL_LONG status = 0;

    /* Pointers to input/output data */
    double *x_re = 0, *x_im = 0;

    DFTI_DESCRIPTOR_HANDLE hand = 0;

    char version[DFTI_VERSION_LENGTH];


    DftiGetValue(0, DFTI_VERSION, version);
    printf("%s\n", version);
    printf("Example config_complex_storage\n");
    printf("Forward and backward split complex in-place 2D transform\n");
    printf("Configuration parameters:\n");
    printf(" DFTI_PRECISION       = DFTI_DOUBLE\n");
    printf(" DFTI_FORWARD_DOMAIN  = DFTI_COMPLEX\n");
    printf(" DFTI_DIMENSION       = 2\n");
    printf(" DFTI_LENGTHS         = { "LI", "LI" }\n", N[0], N[1]);
    printf(" DFTI_COMPLEX_STORAGE = DFTI_REAL_REAL\n");

    printf("Create DFTI descriptor\n");
    status = DftiCreateDescriptor(&hand, DFTI_DOUBLE, DFTI_COMPLEX, 2, N);
    if (0 != status) goto failed;

    printf("Set configuration: split-complex\n");
    status = DftiSetValue(hand, DFTI_COMPLEX_STORAGE, DFTI_REAL_REAL);
    if (0 != status) goto failed;

    printf("Commit the descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Allocate arrays\n");
    x_re = (double*)mkl_malloc(N[0]*N[1]*sizeof(double), 64);
    x_im = (double*)mkl_malloc(N[0]*N[1]*sizeof(double), 64);
    if (0 == x_re || 0 == x_im) goto failed;

    printf("Initialize input data\n");
    init(x_re, x_im, N[0], N[1], H[0], H[1]);

    printf("Compute forward transform\n");
    status = DftiComputeForward(hand, x_re, x_im);
    if (0 != status) goto failed;

    printf("Verify the result\n");
    status = verify(x_re, x_im, N[0], N[1], H[0], H[1]);
    if (0 != status) goto failed;

    printf("Initialize input for backward transform\n");
    init(x_re, x_im, N[0], N[1], -H[0], -H[1]);

    printf("Compute backward transform\n");
    status = DftiComputeBackward(hand, x_re, x_im);
    if (0 != status) goto failed;

    printf("Verify the result of backward transform\n");
    status = verify(x_re, x_im, N[0], N[1], H[0], H[1]);
    if (0 != status) goto failed;

 cleanup:

    printf("Free DFTI descriptor\n");
    DftiFreeDescriptor(&hand);

    printf("Free data arrays\n");
    mkl_free(x_re);
    mkl_free(x_im);

    printf("TEST %s\n",0==status ? "PASSED" : "FAILED");
    return status;

 failed:
    printf(" ERROR, status = "LI"\n", status);
    status = 1;
    goto cleanup;
}

/* Compute (K*L)%M accurately */
static double moda(MKL_LONG K, MKL_LONG L, MKL_LONG M)
{
    return (double)(((long long)K * L) % M);
}

/* Initialize array x(N) to produce unit peaks at x(H) */
static void init(double *x_re, double *x_im, MKL_LONG N1, MKL_LONG N2,
                 MKL_LONG H1, MKL_LONG H2)
{
    double TWOPI = 6.2831853071795864769, phase;
    MKL_LONG n1, n2, S1, S2, index;

    /* Generalized strides for row-major addressing of x */
    S2 = 1;
    S1 = N2;

    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            phase  = moda(n1,H1,N1) / N1;
            phase += moda(n2,H2,N2) / N2;
            index = n1*S1 + n2*S2;
            x_re[index] = cos( TWOPI * phase ) / (N1*N2);
            x_im[index] = sin( TWOPI * phase ) / (N1*N2);
        }
    }
}

/* Verify that x has unit peak at H */
static int verify(double *x_re, double *x_im, MKL_LONG N1, MKL_LONG N2,
                  MKL_LONG H1, MKL_LONG H2)
{
    double err, errthr, maxerr;
    MKL_LONG n1, n2, index;

    /* Generalized strides for row-major addressing of x */
    MKL_LONG S1 = N2, S2 = 1;

    /*
     * Note, this simple error bound doesn't take into account error of
     * input data
     */
    errthr = 5.0 * log( (double)N1*N2 ) / log(2.0) * DBL_EPSILON;
    printf(" Verify the result, errthr = %.3lg\n", errthr);

    maxerr = 0;
    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            double re_exp = 0.0, im_exp = 0.0, re_got, im_got;

            if ((n1-H1)%N1==0 && (n2-H2)%N2==0)
            {
                re_exp = 1;
            }

            index = n1*S1 + n2*S2;
            re_got = x_re[index];
            im_got = x_im[index];
            err  = fabs(re_got - re_exp) + fabs(im_got - im_exp);
            if (err > maxerr) maxerr = err;
            if (!(err < errthr))
            {
                printf(" x["LI"]["LI"]: ",n1,n2);
                printf(" expected (%.17lg,%.17lg), ",re_exp,im_exp);
                printf(" got (%.17lg,%.17lg), ",re_got,im_got);
                printf(" err %.3lg\n", err);
                printf(" Verification FAILED\n");
                return 1;
            }
        }
    }
    printf(" Verified, maximum error was %.3lg\n", maxerr);
    return 0;
}
