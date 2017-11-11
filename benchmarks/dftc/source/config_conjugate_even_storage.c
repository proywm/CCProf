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
! An example of using MKL DFTI configuration parameter
! DFTI_CONJUGATE_EVEN_STORAGE. The parameter defines layout of complex data in
! the backward domain of real-to-complex FFT
! (when DFTI_FORWARD_DOMAIN==DFTI_REAL).
!
! Values:
! DFTI_COMPLEX_REAL (default for 1d and 2d transforms, not recommended)
!     represent the complex data by real and imaginary parts packed in a real
!     array as defined by DFTI_PACKED_FORMAT configuration parameter.
!     This example shows usage of this default setting.
!
! DFTI_COMPLEX_COMPLEX (recommented, default for 3d and higher rank FFTs)
!     represent the complex data by complex elements. For the recommended use
!     of DFTI_CONJUGATE_EVEN_STORAGE see examples of real DFT.
!
! Note: DFTI_COMPLEX_COMPLEX is recommended value for
!       DFTI_CONJUGATE_EVEN_STORAGE configuration.
!
!****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_service.h"
#include "mkl_dfti.h"

typedef MKL_Complex16 (*packed_t)(double *x,MKL_LONG N1,MKL_LONG N2,
                                  MKL_LONG n1,MKL_LONG n2);
static MKL_Complex16 ccs(double *x,MKL_LONG N1,MKL_LONG N2,
                         MKL_LONG n1,MKL_LONG n2);
static MKL_Complex16 pack(double *x,MKL_LONG N1,MKL_LONG N2,
                          MKL_LONG n1,MKL_LONG n2);
static MKL_Complex16 perm(double *x,MKL_LONG N1,MKL_LONG N2,
                          MKL_LONG n1,MKL_LONG n2);

static void init(double *x, MKL_LONG N1, MKL_LONG N2, MKL_LONG S1, MKL_LONG S2,
                 MKL_LONG H1, MKL_LONG H2);
static int verify(packed_t p, double *x, MKL_LONG N1, MKL_LONG N2,
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
    MKL_LONG N[2] = { 16, 32 };

    /* Arbitrary harmonic to verify FFT */
    MKL_LONG H[] = { 1, -1 };

    /* Execution status */
    MKL_LONG status = 0;

    /* Pointer to input/output data */
    double *x;

    /* Strides define the data layout for forward and backward domains */
    MKL_LONG strides[3];

    DFTI_DESCRIPTOR_HANDLE hand = 0;

    char version[DFTI_VERSION_LENGTH];

    DftiGetValue(0, DFTI_VERSION, version); /* get DFTI version */
    printf("%s\n", version);
    printf("Example config_conjugate_even_storage\n");
    printf("Real-to-complex in-place 2D FFT\n");
    printf("Configuration parameters:\n");
    printf(" DFTI_PRECISION                = DFTI_DOUBLE\n");
    printf(" DFTI_FORWARD_DOMAIN           = DFTI_REAL\n");
    printf(" DFTI_DIMENSION                = 2\n");
    printf(" DFTI_LENGTHS                  = { "LI", "LI" }\n", N[0], N[1]);
    printf(" DFTI_CONJUGATE_EVEN_STORAGE   = DFTI_COMPLEX_REAL\n");

    printf("Allocate array x of size (N1+2)*(N2+2),"
           " sufficient space for all packed formats\n");
    x = mkl_malloc(sizeof(double) * (N[0]+2) * (N[1]+2), 64);
    if (0 == x) goto failed;

    printf("===== Configure descriptor for CCS format =====\n");

    printf("Create DFTI descriptor\n");
    status = DftiCreateDescriptor(&hand, DFTI_DOUBLE, DFTI_REAL, 2, N);
    if (0 != status) goto failed;

    /* This may be skipped because COMPLEX_REAL is default for 2D real FFT */
    printf("Set configuration: real elements in conjugate-even domain\n");
    status = DftiSetValue(hand,DFTI_CONJUGATE_EVEN_STORAGE,DFTI_COMPLEX_REAL);
    if (0 != status) goto failed;

    /* This may be skipped because CCS is default for 2D real FFT */
    printf("Set DFTI_PACKED_FORMAT = DFTI_CCS_FORMAT\n");
    status = DftiSetValue(hand, DFTI_PACKED_FORMAT, DFTI_CCS_FORMAT);
    if (0 != status) goto failed;

    strides[2] = 1;
    strides[1] = N[1]+2;
    strides[0] = 0;

    printf("Set input/output strides = { "LI", "LI", "LI" }\n",
           strides[0], strides[1], strides[2]);
    status = DftiSetValue(hand, DFTI_INPUT_STRIDES, strides);
    if (0 != status) goto failed;

    /*
     * Output strides are ignored for real-to-complex in-place FFT
     * with DFTI_COMPLEX_REAL storage.
     *
     * status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, strides);
     * if (0 != status) goto failed;
     */

    printf("Commit DFTI descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Initialize input\n");
    init(x, N[0], N[1], strides[1], strides[2], H[0], H[1]);

    printf("Compute forward transform\n");
    status = DftiComputeForward(hand, x);
    if (0 != status) goto failed;

    printf("Verify the result in CCS format\n");
    status = verify(ccs, x, N[0], N[1], H[0], H[1]);
    if (0 != status) goto failed;

    printf("===== Configure descriptor for PACK format =====\n");

    printf("Set DFTI_PACKED_FORMAT = DFTI_PACK_FORMAT\n");
    status = DftiSetValue(hand, DFTI_PACKED_FORMAT, DFTI_PACK_FORMAT);
    if (0 != status) goto failed;

    strides[2] = 1;
    strides[1] = N[1];
    strides[0] = 0;

    printf("Set input/output strides = { "LI", "LI", "LI" }\n",
           strides[0], strides[1], strides[2]);
    status = DftiSetValue(hand, DFTI_INPUT_STRIDES, strides);
    if (0 != status) goto failed;

    /*
     * Output strides are ignored for real-to-complex in-place FFT
     * with DFTI_COMPLEX_REAL storage.
     *
     * status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, strides);
     * if (0 != status) goto failed;
     */

    printf("Commit DFTI descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Initialize input\n");
    init(x, N[0], N[1], strides[1], strides[2], H[0], H[1]);

    printf("Compute forward transform\n");
    status = DftiComputeForward(hand, x);
    if (0 != status) goto failed;

    printf("Verify the result in PACK format\n");
    status = verify(pack, x, N[0], N[1], H[0], H[1]);
    if (0 != status) goto failed;

    printf("===== Configure descriptor for PERM format =====\n");

    printf("Set DFTI_PACKED_FORMAT = DFTI_PERM_FORMAT\n");
    status = DftiSetValue(hand, DFTI_PACKED_FORMAT, DFTI_PERM_FORMAT);
    if (0 != status) goto failed;

    strides[2] = 1;
    strides[1] = N[1];
    strides[0] = 0;

    printf("Set input/output strides = { "LI", "LI", "LI" }\n",
           strides[0], strides[1], strides[2]);
    status = DftiSetValue(hand, DFTI_INPUT_STRIDES, strides);
    if (0 != status) goto failed;

    /*
     * Output strides are ignored for real-to-complex in-place FFT
     * with DFTI_COMPLEX_REAL storage.
     *
     * status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, strides);
     * if (0 != status) goto failed;
     */

    printf("Commit DFTI descriptor\n");
    status = DftiCommitDescriptor(hand);
    if (0 != status) goto failed;

    printf("Initialize input\n");
    init(x, N[0], N[1], strides[1], strides[2], H[0], H[1]);

    printf("Compute forward transform\n");
    status = DftiComputeForward(hand, x);
    if (0 != status) goto failed;

    printf("Verify the result in PERM format\n");
    status = verify(perm, x, N[0], N[1], H[0], H[1]);
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
static double moda(MKL_LONG K, MKL_LONG L, MKL_LONG M)
{
    return (double)(((long long)K * L) % M);
}

/* Initialize array x(N) to produce unit peaks at x(H) and x(N-H) */
static void init(double *x, MKL_LONG N1, MKL_LONG N2, MKL_LONG S1, MKL_LONG S2,
                 MKL_LONG H1, MKL_LONG H2)
{
    double TWOPI = 6.2831853071795864769, phase, factor;
    MKL_LONG n1, n2, index;

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
static int verify(packed_t packed,double *x,MKL_LONG N1,MKL_LONG N2,
                  MKL_LONG H1,MKL_LONG H2)
{
    double err, errthr, maxerr;
    MKL_LONG n1, n2;

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
            double re_exp = 0, im_exp = 0, re_got, im_got;
            MKL_Complex16 x_got;

            if ((( n1-H1)%N1==0 && ( n2-H2)%N2==0) ||
                ((-n1-H1)%N1==0 && (-n2-H2)%N2==0))
            {
                re_exp = 1.0;
            }

            x_got = packed(x, N1, N2, n1, n2);
            re_got = x_got.real;
            im_got = x_got.imag;

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

/*
 * Fetch x(n1,n2) from the result of N1-by-N2 real FFT CCS-packed in x(N1+2,N2+2).
 * Note: x should be embedded in a matrix at least x(N1+2,N2+2).
 * Assume n1=0:N1-1, n2=0:N2-1.
 */
static MKL_Complex16 ccs(double *y,MKL_LONG N1,MKL_LONG N2,
                         MKL_LONG n1,MKL_LONG n2)
{
    MKL_Complex16 res;
    if (n1==0)
    {
        if (n2 <= N2/2)
        {
            res.real =  y[2*n2+0];
            res.imag =  y[2*n2+1];
        }
        else
        {
            res.real =  y[2*(N2-n2)+0];
            res.imag = -y[2*(N2-n2)+1];
        }
    }
    else if (n2==0)
    {
        if (n1 <= N1/2)
        {
            res.real =  y[(2*n1+0)*(N2+2)];
            res.imag =  y[(2*n1+1)*(N2+2)];
        }
        else
        {
            res.real =  y[(2*(N1-n1)+0)*(N2+2)];
            res.imag = -y[(2*(N1-n1)+1)*(N2+2)];
        }
    }
    else if (n2 == N2-n2)
    {
        if (n1 <= N1/2)
        {
            res.real =  y[(2*n1+0)*(N2+2) + 2*(N2/2)];
            res.imag =  y[(2*n1+1)*(N2+2) + 2*(N2/2)];
        }
        else
        {
            res.real =  y[(2*(N1-n1)+0)*(N2+2) + 2*(N2/2)];
            res.imag = -y[(2*(N1-n1)+1)*(N2+2) + 2*(N2/2)];
        }
    }
    else if (n2 <= N2/2)
    {
        res.real =  y[n1*(N2+2)+2*n2+0];
        res.imag =  y[n1*(N2+2)+2*n2+1];
    }
    else
    {
        res.real =  y[(N1-n1)*(N2+2)+2*(N2-n2)+0];
        res.imag = -y[(N1-n1)*(N2+2)+2*(N2-n2)+1];
    }
    return res;
}

/*
 * Fetch x(n1,n2) from the result of N1-by-N2 real FFT PACK-packed in x(N1,N2).
 * Assume n1=0:N1-1, n2=0:N2-1.
 */
static MKL_Complex16 pack(double *y,MKL_LONG N1,MKL_LONG N2,
                          MKL_LONG n1,MKL_LONG n2)
{
    MKL_Complex16 res;
    if (n1==0)
    {
        if (n2 == 0)
        {
            res.real =  y[0];
            res.imag =  0;
        }
        else if (n2 == N2-n2)
        {
            res.real =  y[2*n2-1];
            res.imag =  0;
        }
        else if (n2 <= N2/2)
        {
            res.real =  y[2*n2-1];
            res.imag =  y[2*n2-0];
        }
        else
        {
            res.real =  y[2*(N2-n2)-1];
            res.imag = -y[2*(N2-n2)-0];
        }
    }
    else if (n2==0)
    {
        if (n1 == N1-n1)
        {
            res.real =  y[(N1-1)*N2];
            res.imag =  0;
        }
        else if (n1 <= N1/2)
        {
            res.real =  y[(2*n1-1)*N2];
            res.imag =  y[(2*n1-0)*N2];
        }
        else
        {
            res.real =  y[(2*(N1-n1)-1)*N2];
            res.imag = -y[(2*(N1-n1)-0)*N2];
        }
    }
    else if (n2 == N2-n2)
    {
        if (n1 == N1-n1)
        {
            res.real =  y[N1*N2 - 1];
            res.imag =  0;
        }
        else if (n1 <= N1/2)
        {
            res.real =  y[(2*n1 - 1)*N2 + N2-1];
            res.imag =  y[(2*n1 - 0)*N2 + N2-1];
        }
        else
        {
            res.real =  y[(2*(N1-n1) - 1)*N2 + N2-1];
            res.imag = -y[(2*(N1-n1) - 0)*N2 + N2-1];
        }
    }
    else if (n2 <= N2/2)
    {
        res.real =  y[n1*N2+2*n2-1];
        res.imag =  y[n1*N2+2*n2-0];
    }
    else
    {
        res.real =  y[(N1-n1)*N2+2*(N2-n2) - 1];
        res.imag = -y[(N1-n1)*N2+2*(N2-n2) - 0];
    }
    return res;
}

/*
 * Fetch x(n1,n2) from the result of N1-by-N2 real FFT PERM-packed in x(N1,N2).
 * Assume n1=0:N1-1, n2=0:N2-1.
 */
static MKL_Complex16 perm(double *y,MKL_LONG N1,MKL_LONG N2,
                          MKL_LONG n1,MKL_LONG n2)
{
    MKL_Complex16 res;
    if (n1==0)
    {
        if (n2 == 0)
        {
            res.real =  y[0];
            res.imag =  0;
        }
        else if (n2 == N2-n2)
        {
            res.real =  y[1];
            res.imag =  0;
        }
        else if (n2 <= N2/2)
        {
            res.real =  y[2*n2+0 - N2%2];
            res.imag =  y[2*n2+1 - N2%2];
        }
        else
        {
            res.real =  y[2*(N2-n2)+0 - N2%2];
            res.imag = -y[2*(N2-n2)+1 - N2%2];
        }
    }
    else if (n2==0)
    {
        if (n1 == N1-n1)
        {
            res.real =  y[N2];
            res.imag =  0;
        }
        else if (n1 <= N1/2)
        {
            res.real =  y[(2*n1+0 - N1%2)*N2];
            res.imag =  y[(2*n1+1 - N1%2)*N2];
        }
        else
        {
            res.real =  y[(2*(N1-n1)+0 - N1%2)*N2];
            res.imag = -y[(2*(N1-n1)+1 - N1%2)*N2];
        }
    }
    else if (n2 == N2-n2)
    {
        if (n1 == N1-n1)
        {
            res.real =  y[N2 + 1];
            res.imag =  0;
        }
        else if (n1 <= N1/2)
        {
            res.real =  y[(2*n1+0 - N1%2)*N2 + 1];
            res.imag =  y[(2*n1+1 - N1%2)*N2 + 1];
        }
        else
        {
            res.real =  y[(2*(N1-n1)+0 - N1%2)*N2 + 1];
            res.imag = -y[(2*(N1-n1)+1 - N1%2)*N2 + 1];
        }
    }
    else if (n2 <= N2/2)
    {
        res.real =  y[n1*N2+2*n2+0 - N2%2];
        res.imag =  y[n1*N2+2*n2+1 - N2%2];
    }
    else
    {
        res.real =  y[(N1-n1)*N2+2*(N2-n2)+0 - N2%2];
        res.imag = -y[(N1-n1)*N2+2*(N2-n2)+1 - N2%2];
    }
    return res;
}
