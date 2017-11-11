Intel(R) Math Kernel Library Version 2017.0.4 Product Build 20170811 for Intel(R) 64 architecture applications
Example basic_dp_complex_dft_2d
Forward and backward double-precision complex in-place 2D FFTs
Configuration parameters:
 DFTI_PRECISION      = DFTI_DOUBLE
 DFTI_FORWARD_DOMAIN = DFTI_COMPLEX
 DFTI_DIMENSION      = 2
 DFTI_LENGTHS        = {4096, 4096}
Create DFTI descriptor
Commit DFTI descriptor
Allocate input array
Initialize input for forward transform
Compute forward transform
Verify the result of forward FFT
 Verify the result, errthr = 2.66e-14
 Verified, maximum error was 2.22e-16
Initialize input for backward transform
Compute backward transform
Verify the result of backward FFT
 Verify the result, errthr = 2.66e-14
 Verified, maximum error was 2.22e-16
Free DFTI descriptor
Free data array
TEST PASSED
