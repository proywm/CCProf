cd reproduce_case_studies_of_cgo2018_paper

ReproduceBaseDIR=$PWD

cd ADI_PolyBench
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR

cd HimenoBMT
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR                       

cd Kripke_representative_loop          
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd MKL_FFT_Optimized    
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd Tiny_DNN_representative_loop_Optimized
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd ADI_PolyBench_Optimized
sh ccProf_run_and_cdf.sh                  
cd $ReproduceBaseDIR                       

cd HimenoBMT_Optimized  
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd Kripke_representative_loop_Optimized  
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd NW_Rodinia            
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd MKL_FFT                              
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd NW_Rodinia_Optimized 
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR    
                   
cd Tiny_DNN_representative_loop
sh ccProf_run_and_cdf.sh
cd $ReproduceBaseDIR

python plotCase.py

cp CCProfPredictionOnCaseStudies_Figure9.pdf $CCPROFDIR/../

##################################################################################################################
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

################################################################################################################

cp *Stat.txt $CCPROFDIR/../
