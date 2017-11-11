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
! Example of printing DFTI descriptor's configuration
!
!****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mkl_dfti.h"

static void dump_descriptor(DFTI_DESCRIPTOR_HANDLE hand);

/* Define the format to printf MKL_LONG values */
#if !defined(MKL_ILP64)
#define LI "%li"
#else
#define LI "%lli"
#endif

int main(void)
{
    /* Sizes of a 4D FFT */
    MKL_LONG N[4] = { 10,20,30,40 };

    /* Execution status */
    MKL_LONG status = 0;

    DFTI_DESCRIPTOR_HANDLE hand = 0;

    printf("Create a DFTI descriptor\n");
    status = DftiCreateDescriptor(&hand, DFTI_DOUBLE, DFTI_COMPLEX, 4, N);
    if (0 != status) goto failed;

    printf("Dump the descriptor\n");
    dump_descriptor(hand);

 cleanup:

    printf("Free DFTI descriptor\n");
    DftiFreeDescriptor(&hand);

    printf("TEST %s\n",0==status ? "PASSED" : "FAILED");
    return status;

 failed:
    printf(" ERROR, status = "LI"\n", status);
    status = 1;
    goto cleanup;
}

/* Maximum supported rank of multidimensional FFTs */
#define MAX_RANK 7

static void dump_descriptor(DFTI_DESCRIPTOR_HANDLE hand)
{
    /* Execution status */
    MKL_LONG status = 0;

    char version[DFTI_VERSION_LENGTH];

    enum DFTI_CONFIG_VALUE placement, precision, domain, storage, packfmt, wspace, ordering, cmtstatus;
    MKL_LONG rank, lengths[MAX_RANK];
    double fwd_scale, bwd_scale;
    MKL_LONG nut, is[1+MAX_RANK], os[1+MAX_RANK], ntr, idist, odist, tlimit;

    DftiGetValue(0, DFTI_VERSION, version);
    printf("%s\n", version);

    printf("  PRECISION = "); fflush(0);
    status = DftiGetValue(hand, DFTI_PRECISION, &precision);
    if (0 != status) goto failed;
    if      (precision == DFTI_SINGLE) printf("DFTI_SINGLE\n");
    else if (precision == DFTI_DOUBLE) printf("DFTI_DOUBLE\n");
    else { printf("unknown (%i)\n", precision); goto failed; }

    printf("  FORWARD_DOMAIN = "); fflush(0);
    status = DftiGetValue(hand, DFTI_FORWARD_DOMAIN, &domain);
    if (0 != status) goto failed;
    if      (domain == DFTI_COMPLEX) printf("DFTI_COMPLEX\n");
    else if (domain == DFTI_REAL)    printf("DFTI_REAL\n");
    else { printf("unknown (%i)\n", domain); goto failed; }

    printf("  DIMENSION = "); fflush(0);
    status = DftiGetValue(hand, DFTI_DIMENSION, &rank);
    if (0 != status) goto failed;
    printf(LI"\n", rank);

    printf("  LENGTHS = "); fflush(0);
    status = DftiGetValue(hand, DFTI_LENGTHS, lengths);
    if (0 != status) goto failed;
    {
        int r = 0;
        printf(LI,lengths[0]);
        for (r = 1; r < rank; ++r) printf(", "LI, lengths[r]);
        printf("\n");
    }

    printf("  PLACEMENT = "); fflush(0);
    status = DftiGetValue(hand, DFTI_PLACEMENT, &placement);
    if (0 != status) goto failed;
    if      (placement == DFTI_NOT_INPLACE) printf("DFTI_NOT_INPLACE\n");
    else if (placement == DFTI_INPLACE)     printf("DFTI_INPLACE\n");
    else { printf("unknown (%i)\n", placement); goto failed; }

    printf("  F/B SCALES = "); fflush(0);
    if (precision == DFTI_DOUBLE)
    {
        status = DftiGetValue(hand, DFTI_FORWARD_SCALE, &fwd_scale);
        if (0 != status) goto failed;
        status = DftiGetValue(hand, DFTI_BACKWARD_SCALE, &bwd_scale);
        if (0 != status) goto failed;
    }
    else
    {
        float fs, bs;
        status = DftiGetValue(hand, DFTI_FORWARD_SCALE, &fs);
        if (0 != status) goto failed;
        status = DftiGetValue(hand, DFTI_BACKWARD_SCALE, &bs);
        if (0 != status) goto failed;
        fwd_scale = (double)fs;
        bwd_scale = (double)bs;
    }
    printf(" %lg, %lg\n", fwd_scale, bwd_scale );

    printf("  NO OF USER THREADS = "); fflush(0);
    status = DftiGetValue(hand, DFTI_NUMBER_OF_USER_THREADS, &nut);
    if (0 != status) goto failed;
    printf(LI"\n", nut);

    printf("  INPUT  STRIDES = "); fflush(0);
    status = DftiGetValue(hand, DFTI_INPUT_STRIDES, is);
    if (0 != status) goto failed;
    {
        int r = 0;
        printf(LI,is[0]);
        for (r = 1; r <= rank; ++r) printf(", "LI, is[r]);
        printf("\n");
    }

    printf("  OUTPUT STRIDES = "); fflush(0);
    status = DftiGetValue(hand, DFTI_OUTPUT_STRIDES, os);
    if (0 != status) goto failed;
    {
        int r = 0;
        printf(LI,os[0]);
        for (r = 1; r <= rank; ++r) printf(", "LI, os[r]);
        printf("\n");
    }

    printf("  NO OF TRANSFORMS = "); fflush(0);
    status = DftiGetValue(hand, DFTI_NUMBER_OF_TRANSFORMS, &ntr);
    if (0 != status) goto failed;
    printf(LI"\n", ntr);

    printf("  I/O DISTANCES = "); fflush(0);
    status = DftiGetValue(hand, DFTI_INPUT_DISTANCE, &idist);
    if (0 != status) goto failed;
    status = DftiGetValue(hand, DFTI_OUTPUT_DISTANCE, &odist);
    if (0 != status) goto failed;
    printf(LI", "LI"\n", idist, odist);

    if (domain == DFTI_COMPLEX)
    {
        printf("  COMPLEX STORAGE = "); fflush(0);
        status = DftiGetValue(hand, DFTI_COMPLEX_STORAGE, &storage);
        if (0 != status) goto failed;
        if      (storage == DFTI_COMPLEX_COMPLEX) printf("DFTI_COMPLEX_COMPLEX\n");
        else if (storage == DFTI_REAL_REAL)       printf("DFTI_REAL_REAL\n");
        else { printf("wrong (%i)\n", storage); goto failed; }
    }
    else
    {
        printf("  CONJUGATE EVEN STORAGE = "); fflush(0);
        status = DftiGetValue(hand, DFTI_CONJUGATE_EVEN_STORAGE, &storage);
        if (0 != status) goto failed;
        if      (storage == DFTI_COMPLEX_COMPLEX) printf("DFTI_COMPLEX_COMPLEX\n");
        else if (storage == DFTI_COMPLEX_REAL)    printf("DFTI_COMPLEX_REAL\n");
        else { printf("wrong (%i)\n", storage); goto failed; }
        if (storage == DFTI_COMPLEX_REAL)
        {
            printf("     PACKED FORMAT = "); fflush(0);
            status = DftiGetValue(hand, DFTI_PACKED_FORMAT, &packfmt);
            if (0 != status) goto failed;
            if      (packfmt == DFTI_CCS_FORMAT) printf("DFTI_CCS_FORMAT\n");
            else if (packfmt == DFTI_PACK_FORMAT) printf("DFTI_PACK_FORMAT\n");
            else if (packfmt == DFTI_PERM_FORMAT) printf("DFTI_PERM_FORMAT\n");
            else { printf("wrong (%i)\n", packfmt); goto failed; }
        }
    }

    printf("  WORKSPACE = "); fflush(0);
    status = DftiGetValue(hand, DFTI_WORKSPACE, &wspace);
    if (0 != status) goto failed;
    if      (wspace == DFTI_ALLOW) printf("DFTI_ALLOW\n");
    else if (wspace == DFTI_AVOID) printf("DFTI_AVOID\n");
    else if (wspace == DFTI_NONE)  printf("DFTI_NONE\n");
    else { printf("wrong (%i)\n", wspace); goto failed; }

    printf("  ORDERING = "); fflush(0);
    status = DftiGetValue(hand, DFTI_ORDERING, &ordering);
    if (0 != status) goto failed;
    if      (ordering == DFTI_ORDERED)            printf("DFTI_ORDERED\n");
    else if (ordering == DFTI_BACKWARD_SCRAMBLED) printf("DFTI_BACKWARD_SCRAMBLED\n");
    else { printf("wrong (%i)\n", ordering); goto failed; }

    printf("  COMMIT STATUS = "); fflush(0);
    status = DftiGetValue(hand, DFTI_COMMIT_STATUS, &cmtstatus);
    if (0 != status) goto failed;
    if      (cmtstatus == DFTI_COMMITTED) printf("DFTI_COMMITTED\n");
    else if (cmtstatus == DFTI_UNCOMMITTED) printf("DFTI_UNCOMMITTED\n");
    else { printf("wrong (%i)\n", cmtstatus); goto failed; }

    printf("  THREAD LIMIT = "); fflush(0);
    status = DftiGetValue(hand, DFTI_THREAD_LIMIT, &tlimit);
    if (0 != status) goto failed;
    printf(LI"\n", tlimit); fflush(0);

    return;

 failed:
    printf("Error, status = "LI"\n", status);
    exit(1);
}
