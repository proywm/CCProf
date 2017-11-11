Intel(R) Math Kernel Library Version 2017.0.4 Product Build 20170811 for Intel(R) 64 architecture applications
Example basic_dp_complex_dft_1d
Forward and backward double-precision complex in-place 1D FFTs
Configuration parameters:
 DFTI_PRECISION      = DFTI_DOUBLE
 DFTI_FORWARD_DOMAIN = DFTI_COMPLEX
 DFTI_DIMENSION      = 1
 DFTI_LENGTHS        = {7}
Create DFTI descriptor
Commit DFTI descriptor
Allocate input array
Initialize input for forward transform
Compute forward transform
Verify the result of forward FFT
 Verify the result, errthr = 3.12e-15
 Verified, maximum error was 8.92e-17
Initialize input for backward transform
Compute backward transform
Verify the result of backward FFT
 Verify the result, errthr = 3.12e-15
 Verified, maximum error was 8.92e-17
Release the DFTI descriptor
Free data array
TEST PASSED
