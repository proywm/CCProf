# CCProf: Lightweight Detection of Cache Conflicts

This artifact contains the source code of CCProf along
with six benchmarks demonstrating cache conflict behavior. Additionally, we provide the CCProf guided transformed source code of those benchmarks. This artifact
comes with scripts to build CCProf and detect cache
conflict on any application. Finally, we provide scripts
to reproduce the performance results presented in the
CGO2018 paper: Lightweight Detection of Cache Conflicts. Successful building and running of the scripts will
reproduce figure 9 and performance statistics(i.e., speed
up, overhead, cache miss reduction) of table 2.

### Hardware Prerequisite ###
Any of the following microarchitectures:
* Intel Broadwell (verified)
* Intel Haswell (verified)
    
### Software Depebdencies  ###
* cmake version 2.8.10 or later
* gcc (GCC) 4.8.5 or later
* intel MKL
* Linux kernel 4.8.0 or later
* python2.7
* pip
* git client

python, pip and git can be installed on Ubuntu via:

`$ sudo apt-get install python python-pip git`

Software dependencies must be resolved before proceeding to the next step.

### Build  ###
You can install CCProf via
`$ sh build.sh`

After successful installation,the script will update the ~/.bashrc file with CCPROFDIR
environment variable pointing to CCProf’s installation
directory. It will also set HpcToolKit’s local installation
directory in PATH environment variable. Load the environment variables before proceeding to the next step to
run the CCProf via

`$ source ~/.bashrc`

### Running experimental workflows (reproducing result) ###
To reproduce the results of the paper, run

`$ sh reproduce_result.sh`

This will run all the experiments on the six benchmarks, perform post-processing, run
evaluation and finally generate CCPROF_result/*.pdf, CCPROF_result/CCProfPerformanceMetrics table2.txt
and CCPROF_result/*result files.

### Validation of results ###
CCProf’s conflict miss analysis results are stored in
CCPROF_result/*result files. Compare the CCProf’s
conflict analysis on original and transformed code. For
instance, to compare CCProf’s evaluation report on
ADI, compare CCPROF_result/ADI PolyBench result
and CCPROF_result/ADI PolyBench Optimized result
and observe loop level cache conflict prediction. Performance metrics (i.e. speed up, overhead and cache
miss reduction) are summarized in CCProfPerformanceMetrics_table2.txt. These performance metrics
are presented in table 2 of the paper. The generated
CCPROF_result/*.pdf files plot the figure 9 of the paper.

We donot expect absolute values to match with the paper, but we expect a similar trend of CCProf guidance and performance metrics as presented in paper. 

As instruction pointer(ip) varies over compilations and
ambiguity of line numbers(e.g., MKLFFT), it may require to manually set the appropriate loop files to generate CCPROF_result/*.pdf files.

### Authors ###

Probir Roy, College of William and Mary

Shuaiwen Leon Song, Pacific Northwest National Laboratory

Sriram Krishnamoorthy, Pacific Northwest National Laboratory

Xu Liu, College of William and Mary

### Paper ###
[CGO'18] "Lightweight Detection of Cache Conflicts"
