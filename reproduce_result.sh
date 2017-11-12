cd reproduce_case_studies_of_cgo2018_paper
ReproduceBaseDIR=$PWD

cd ..
mkdir CCPROF_result
cd CCPROF_result
RESULTDIR=$PWD

##########################################################  CCProf RCD calculation and prediction ###############

cd $ReproduceBaseDIR
cd ADI_PolyBench
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/ADI_PolyBench_result

cd $ReproduceBaseDIR

cd HimenoBMT
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/HimenoBMT_result
cd $ReproduceBaseDIR                       

cd Kripke_representative_loop          
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/Kripke_representative_loop_result
cd $ReproduceBaseDIR    
                   
cd MKL_FFT_Optimized    
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/MKL_FFT_Optimized_result
cd $ReproduceBaseDIR    
                   
cd Tiny_DNN_representative_loop_Optimized
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/Tiny_DNN_representative_loop_Optimized_result
cd $ReproduceBaseDIR    
                   
cd ADI_PolyBench_Optimized
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/ADI_PolyBench_Optimized_result
cd $ReproduceBaseDIR                       

cd HimenoBMT_Optimized  
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/HimenoBMT_Optimized_result
cd $ReproduceBaseDIR    
                   
cd Kripke_representative_loop_Optimized  
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/Kripke_representative_loop_Optimized_result
cd $ReproduceBaseDIR    
                   
cd NW_Rodinia            
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/NW_Rodinia_result
cd $ReproduceBaseDIR    
                   
cd MKL_FFT                              
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/MKL_FFT_result
cd $ReproduceBaseDIR    
                   
cd NW_Rodinia_Optimized 
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/NW_Rodinia_Optimized_result
cd $ReproduceBaseDIR    
                   
cd Tiny_DNN_representative_loop
sh ccProf_run_and_cdf.sh
cd workspace
python ccProfAnalysis.py > $RESULTDIR/Tiny_DNN_representative_loop_result
cd $ReproduceBaseDIR

###################################################### Generate Performance Metrices ##########################################################
cd $ReproduceBaseDIR

cd ADI_PolyBench
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd HimenoBMT
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd Kripke_representative_loop
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd MKL_FFT_Optimized
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd Tiny_DNN_representative_loop_Optimized
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd ADI_PolyBench_Optimized
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd HimenoBMT_Optimized
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd Kripke_representative_loop_Optimized
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd NW_Rodinia
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd MKL_FFT
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd NW_Rodinia_Optimized
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

cd Tiny_DNN_representative_loop
sh ccProf_runtime_cache_stat.sh
cd $ReproduceBaseDIR

############################################# Evaluation ###################################################

python getStat_overhead.py
python getStat_speedup.py
python getStat_cache.py



cat *Stat.txt $RESULTDIR/CCProfPerformanceMetrics_table2.txt
rm *Stat.txt

################################################# plotting ##################################################
cd $CCPROFDIR
sh plot.sh

echo "Results have been generated. Navigate to " + $RESULTDIR
