cd reproduce_case_studies_of_cgo2018_paper
ReproduceBaseDIR=$PWD

cd ..
mkdir CCPROF_result
cd CCPROF_result
RESULTDIR=$PWD

###################################################### Generate cache stat ##########################################################
cd $ReproduceBaseDIR

cd ADI_PolyBench
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd HimenoBMT
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd Kripke_representative_loop
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd MKL_FFT_Optimized
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd Tiny_DNN_representative_loop_Optimized
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd ADI_PolyBench_Optimized
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd HimenoBMT_Optimized
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd Kripke_representative_loop_Optimized
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd NW_Rodinia
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd MKL_FFT
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd NW_Rodinia_Optimized
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

cd Tiny_DNN_representative_loop
sh ccProf_cache_stat.sh
cd $ReproduceBaseDIR

############################################# Evaluation ###################################################
#rm $RESULTDIR/CCProfPerformanceMetrics_table2.txt
python getStat_cache.py

cat *Stat.txt >> $RESULTDIR/CCProfPerformanceMetrics_table2.txt
rm *Stat.txt

