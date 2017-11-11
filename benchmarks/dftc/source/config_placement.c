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
! An example of using MKL DFTI configuration parameter DFTI_PLACEMENT.
! The parameter defines if the result overwrites the input data or not.
!
! Values:
!   DFTI_INPLACE (default) - result overwrites input data
!   DFTI_NOT_INPLACE       - result is placed in a separate array
!
! Note: When storage data types of forward and backward domains are
!       the same, the configuration parameters for the layout of input are
!       also used for the layout of output (e.g. output strides are
!       ignored). Otherwise, both input and output layout shall be defined
!       (for example real transform with conjugate even storage
!       set to DFTI_COMPLEX_COMPLEX).
!
! This example computes an in-place and an out-of-place 2D real transform.
!
!****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_service.h"
#include "mkl_dfti.h"

static void init_r(double *x, MKL_LONG *N, MKL_LONG *S, MKL_LONG *H);
static int verify_c(MKL_Complex16 *x, MKL_LONG *N, MKL_LONG *S, MKL_LONG *H);

/* Define the format to printf MKL_LONG values */
#if !defined(MKL_ILP64)
#define LI "%li"
#else
#define LI "%lli"
#endif

int main(void)
{
    /* Sizes of 2D transform */
    MKL_LONG N[2] = { 128, 256 };

    /* Arbitrary harmonic used to verify FFT */
    MKL_LONG H[2] = { -1, 2 };

    /* Execution status */
    MKL_LONG status = 0;

    /* Pointers to input and output data */
    double *r = 0;
    MKL_Complex16 *c = 0;

    /* Strides define data layout in forward and backward domains */
    MKL_LONG rs[3];
    MKL_LONG cs[3];

    DFTI_DESCRIPTOR_HANDLE hand = 0;

    char version[DFTI_VERSION_LENGTH];


    DftiGetValue(0, DFTI_VERSION, version);
    printf("%s\n", version);
    printf("Example config_placement\n");
    printf("In-place and out-of-place 2D real FFT\n");
    printf(" Configuration parameters:\n");
    printf(" DFTI_PRECISION      = DFTI_DOUBLE\n");
    printf(" DFTI_FORWARD_DOMAIN = DFTI_REAL\n");
    printf(" DFTI_DIMENSION      = 2\n");
    printf(" DFTI_LENGTHS        = { "LI", "LI" }\n", N[0], N[1]);

    printf("======= In-place 2D real FFT =======\n");

    printf("Allocate array for input/output data\n");
    r = mkl_malloc(sizeof(double) * N[0]*(N[1]/2+1)*2, 64);
    if (!r) goto failed;


    printf("Create DFTI descriptor\n");
    status = DftiCreateDescriptor(&hand, DFTI_DOUBLE, DFTI_REAL, 2, N);
    if (0 != status) goto failed;

    /*
     * In-place is default configuration, no need to set it.
     *
     * status = DftiSetValue(hand, DFTI_PLACEMENT, DFTI_INPLACE);
     * if (0 != status) goto failed;
     */

    printf("Set CCE storage\n");
    status = DftiSetValue(hand, DFTI_CONJUGATE_EVEN_STORAGE,
                          DFTI_COMPLEX_COMPLEX);
    if (0 != status) goto failed;

    rs[2] = 1;              cs[2] = 1;
    rs[1] = (N[1]/2+1)*2;   cs[1] = (N[1]/2+1);
    rs[0] = 0;              cs[0] = 0;

    printf("Set input  strides = "LI", "LI", "LI"\n", rs[0], rs[1], rs[2]);
    status = DftiSetValue(hand, DFTI_INPUT_STRIDES, rs);
    if (0 != status) goto failed;

    printf("Set output strides = "LI", "LI", "LI"\n", cs[0], cs[1], cs[2]);
    status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, cs);
    if (0 != status) goto failed;

    printf("Commit descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Inititalize input for 2D real in-place transform\n");
    init_r(r, N, rs, H);

    printf("Compute real-to-complex in-place transform\n");
    status = DftiComputeForward(hand, r);
    if (0 != status) goto failed;

    printf("Verify 2D real in-place transform\n");
    status = verify_c((MKL_Complex16*)r, N, cs, H);
    if (0 != status) goto failed;


    printf("======= Reconfigure for out-of-place 2D real FFT =======\n");

    printf("Allocate array for output data\n");
    c = mkl_malloc(sizeof(MKL_Complex16) * N[0]*(N[1]/2+1), 64);
    if (!c) goto failed;

    printf("Set out-of-place configuration\n");
    status = DftiSetValue(hand, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
    if (0 != status) goto failed;


    rs[2] = 1;              cs[2] = 1;
    rs[1] = N[1];           cs[1] = (N[1]/2+1);
    rs[0] = 0;              cs[0] = 0;

    printf("Set input  strides = "LI", "LI", "LI"\n", rs[0], rs[1], rs[2]);
    status = DftiSetValue(hand, DFTI_INPUT_STRIDES, rs);
    if (0 != status) goto failed;

    printf("Set output strides = "LI", "LI", "LI"\n", cs[0], cs[1], cs[2]);
    status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, cs);
    if (0 != status) goto failed;

    printf("Commit descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Inititalize input for 2D real out-of-place transform\n");
    init_r(r, N, rs, H);

    printf("Compute real-to-complex out-of-place transform\n");
    status = DftiComputeForward(hand, r, c);
    if (0 != status) goto failed;

    printf("Verify 2D real in-place transform\n");
    status = verify_c(c, N, cs, H);
    if (0 != status) goto failed;

 cleanup:

    printf("Free DFTI descriptor\n");
    DftiFreeDescriptor(&hand);

    printf("Free data arrays\n");
    mkl_free(r);
    mkl_free(c);

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

/* Initialize array x(N) to produce unit peaks at x(H) and x(N-H) */
static void init_r(double *x, MKL_LONG *N, MKL_LONG *S, MKL_LONG *H)
{
    double TWOPI = 6.2831853071795864769, phase, factor;
    MKL_LONG n1, n2, N1, N2, S1, S2, H1, H2, index;

    N1 = N[0];  S1 = S[1];  H1 = H[0];
    N2 = N[1];  S2 = S[2];  H2 = H[1];

    factor = (2*(N1-H1)%N1==0 && 2*(N2-H2)%N2==0) ? 1.0 : 2.0;
    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2; n2++)
        {
            phase  = moda(n1,H1,N1) / N1;
            phase += moda(n2,H2,N2) / N2;
            index = n1*S1 + n2*S2;
            x[index] = factor * cos( TWOPI * phase ) / (N1*N2);
        }
    }
}

/* Verify that x has unit peak at H */
static int verify_c(MKL_Complex16 *x, MKL_LONG *N, MKL_LONG *S, MKL_LONG *H)
{
    double err, errthr, maxerr;
    MKL_LONG n1, n2, N1, N2, S1, S2, H1, H2, index;

    N1 = N[0];  S1 = S[1];  H1 = H[0];
    N2 = N[1];  S2 = S[2];  H2 = H[1];

    /*
     * Note, this simple error bound doesn't take into account error of
     * input data
     */
    errthr = 2.5 * log( (double)N1*N2 ) / log(2.0) * DBL_EPSILON;
    printf(" Check if err is below errthr %.3lg\n", errthr);

    maxerr = 0;
    for (n1 = 0; n1 < N1; n1++)
    {
        for (n2 = 0; n2 < N2/2+1; n2++)
        {
            double re_exp = 0.0, im_exp = 0.0, re_got, im_got;

            if ((( n1-H1)%N1==0 && ( n2-H2)%N2==0) ||
                ((-n1-H1)%N1==0 && (-n2-H2)%N2==0))
            {
                re_exp = 1;
            }

            index = n1*S1 + n2*S2;
            re_got = x[index].real;
            im_got = x[index].imag;
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
    printf(" Verified,  maximum error was %.3lg\n", maxerr);
    return 0;
}
