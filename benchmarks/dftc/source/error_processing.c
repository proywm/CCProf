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
! An example of error processing when using DFTI functions.
!
!****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_dfti.h"

/* Define the format to printf MKL_LONG values */
#if !defined(MKL_ILP64)
#define LI "%li"
#else
#define LI "%lli"
#endif

static void report_error(MKL_LONG status);

int main(void)
{
    /* Execution status */
    MKL_LONG status = 0;

    DFTI_DESCRIPTOR_HANDLE hand = 0;

    printf("Example error_processing\n");

    printf("Try to create a DFTI descriptor with misplaced arguments\n");
    status = DftiCreateDescriptor(&hand, DFTI_COMPLEX, DFTI_SINGLE, 1, 1024);
    if (0 != status) report_error(status);

    if (0 == status)
    {
        printf("TEST FAILED\n");
        return 1;
    }
    else
    {
        printf("TEST PASSED\n");
        return 0;
    }
}

static void report_error(MKL_LONG status)
{
    printf(" Nonzero status = "LI"\n", status);
    printf(" Check if the status indicates of error\n");
    if ( ! DftiErrorClass( status, DFTI_NO_ERROR))
    {
        printf("   Error: %s\n", DftiErrorMessage(status));
    }
    else
    {
        printf("   Not an error: %s\n", DftiErrorMessage(status));
    }
}
