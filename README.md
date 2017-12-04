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
* Intel Skylake (verified)
* Intel Broadwell (verified)
* Intel Haswell (verified)
    
### Software Depebdencies  ###
* cmake version 2.8.10 or later
* gcc (GCC) 4.8.5 or later
* intel MKL
* Linux kernel 4.8.0 or later
* git client
* python2.7, pandas (0.19.2), numpy (1.12.0), pickleshare (0.7.4), plotly (2.2.1), sklearn, matplotlib (2.0.0)

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

### Assumption ###
We assume that you donot need sudo permission to access native PAPI events(i.e., L1-DCACHE-LOADS, L1-DCACHE-LOAD-MISSES, LLC_MISSES and LLC_REFERENCES). 

You can test this by running following command:

`$CCPROFDIR/PAPI/PAPI/src/libpfm4/perf_examples/task -e L1-DCACHE-LOADS -e L1-DCACHE-LOAD-MISSES -e LLC_MISSES -e LLC_REFERENCES ls`

If it returns with "Permission denied", you need to modify the cachestat scripts with sudo permission. 

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

### Individual Experiments ###
You may prefer to run individual test case or evaluate a new application using CCProf. This section demonstrates how to customize scripts and run an application with CCProf.

#### Setting up for new application ####
To run a new application, navigate to reproduce_case_studies_of_cgo2018_paper directory, create an application directory and copy scripts from an example test case directory:

```
$ cd reproduce_case_studies_of_cgo2018_paper
$ mkdir App
$ cp ../ADI_PolyBench/*.sh .
```

#### configuration ####
Go through the scripts and replace with appropriate path and parameters for the new application.

* set path of the directory of the target application in BENCHMARK_RELATIVE_LOCATION 
* set the path of the binary of the target application in BENCHMARK_BINARY*
* set appropriate parameters of the target application

To set sampling period, write to SampleRateThreshold as shown in the script:

`$ echo "1212" > SampleRateThreshold`

#### Run ####
Once configured, run ccProf_run_and_analyze.sh for general CCProf analysis report
`$sh ccProf_run_and_analyze.sh`

CCProf will profile the target application and preprocess the generated files for post-mortem analysis and run analysis to identify cache conflict per loop. For ADI benchmark, running this script gives following output:

```
CCPROF PREDICTS >>> ***     CONFLICT MISS     *** in LOOP(line: 102 :ip 0x400f6c) ***      AND     *** loop contribution is ***    HIGH    ***  94.26 percent

CCPROF PREDICTS >>> ***  NO CONFLICT MISS     *** in loop(line: 108 :ip 0x401051). Loop's contribution to total L1 miss: 3.13 percent

CCPROF PREDICTS >>> ***  NO CONFLICT MISS     *** in loop(line: 117 :ip 0x401282). Loop's contribution to total L1 miss: 0.86 percent

CCPROF PREDICTS >>> ***  NO CONFLICT MISS     *** in loop(line: 122 :ip 0x401365). Loop's contribution to total L1 miss: 1.74 percent
```

You can identify the file and line number from the IP address:

`$addr2line -e APP_BINARY 0xIP`

If you wish to generate CDF of RCD of each loop, navigate to workspace directory within target App directory and run:

`$python ccProfCDFdata.py`

This will generate the CDF_of_Loop_at_[N] files where N is the loop identifier.

To get perfomrance statistics, navigate to the taget App directory and run ccProf_runtime_cache_stat.sh to get performance data(i.e., runtime, cache miss etc.)

`$sh ccProf_runtime_cache_stat.sh`
 
This script will generate runtime_ccProf, runtime_without_ccProf, runtime_O3 and cacheStat_L1L_L1M_L2M_LLCM files in the workspace.



### Authors ###

Probir Roy, College of William and Mary

Shuaiwen Leon Song, Pacific Northwest National Laboratory

Sriram Krishnamoorthy, Pacific Northwest National Laboratory

Xu Liu, College of William and Mary

### Paper ###
[CGO'18] "Lightweight Detection of Cache Conflicts"
