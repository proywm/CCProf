RESULTDIR=$CCPROFDIR/CCPROF_result

echo -e "==================== Before building  =========\n\n"
echo -e "Make sure hardware and software dependencis are met. Read the ReadMe file for dependencies\n"

echo -e "==================== How to build =========\n\n"
echo -e "1. To build: sh build.sh\n"
echo -e "2. Set environment variable: source ~/.bashrc\n"

echo -e "==================== How to run =========\n\n"
echo -e "1. To reproduce results: sh reproduce_result.sh \n"
echo -e "This will create and store all results in $RESULTDIR\n"

echo -e "==================== How to validate result =========\n\n"
echo -e "1. If Results are generated; Navigate to " $RESULTDIR "\n"
echo -e "2. For Evaluation metrices, read file " $RESULTDIR/CCProfPerformanceMetrics_table2.txt "\n"
echo -e "3. For figure 9 of the paper, navigate to " $RESULTDIR "/*.pdf" "\n"
echo -e "4. To verify results of CCProf analysis navigate to " $RESULTDIR " and *result files(e.a, compare  ADI_PolyBench_result and ADI_PolyBench_Optimized_result) \n" 

echo -e "==================== Troubleshooting  =========\n\n"
echo -e "1. No Figure 9(*.pdf files): It is possible due to ambiguity of generated line numbers. In that case, it is required to manually set the generated files in plot based on the $RESULTDIR/*result files. Please refer to github directory for more details\n"
