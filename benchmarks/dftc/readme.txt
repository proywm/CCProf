!===============================================================================
! Copyright 2003-2016 Intel Corporation All Rights Reserved.
!
! The source code,  information  and material  ("Material") contained  herein is
! owned by Intel Corporation or its  suppliers or licensors,  and  title to such
! Material remains with Intel  Corporation or its  suppliers or  licensors.  The
! Material  contains  proprietary  information  of  Intel or  its suppliers  and
! licensors.  The Material is protected by  worldwide copyright  laws and treaty
! provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
! modified, published,  uploaded, posted, transmitted,  distributed or disclosed
! in any way without Intel's prior express written permission.  No license under
! any patent,  copyright or other  intellectual property rights  in the Material
! is granted to  or  conferred  upon  you,  either   expressly,  by implication,
! inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
! property rights must be express and approved by Intel in writing.
!
! Unless otherwise agreed by Intel in writing,  you may not remove or alter this
! notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
! suppliers or licensors in any way.
!===============================================================================

mkl/examples/dftc directory contains examples of using Discrete Fourier
Transform Interface (DFTI) for C/C++ to compute various FFT problems.

Each example is a self-contained C program.

Examples with prefix 'basic_' demonstrate simple ways to compute complex and
real Fast Fourier transforms. The programs may be used as quick start
templates. Files containing '_dp_'/'_sp_' in their names use double and single
precision, respectively.

Examples with prefix 'config_' demonstrate usage of DFTI configuration
parameters.

For every FFT computed in the example an initialization and verification
function is provided. Initialization function shows how the input data is
indexed and what input will produce a unit peak in the result. Verification
function checks if the unit peak is produced by the computation.

Every example uses dynamically allocated arrays for the data.  Sizes of the
transforms and parameters for verification are selected randomly.

Refer to "Fast Fourier Transforms" chapter in MKL Reference Manual for detailed
description of the functions used in these examples.

Your feedback on the examples is welcome at Intel MKL Forum site:
http://software.intel.com/en-us/forums/intel-math-kernel-library


(set-fill-column 79)
