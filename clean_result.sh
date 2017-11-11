cd reproduce_case_studies_of_cgo2018_paper

ReproduceBaseDIR=$PWD

cd ADI_PolyBench
rm -rf workspace*
cd $ReproduceBaseDIR

cd HimenoBMT
rm -rf workspace*
cd $ReproduceBaseDIR                       

cd Kripke_representative_loop          
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd MKL_FFT_Optimized    
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd Tiny_DNN_representative_loop_Optimized
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd ADI_PolyBench_Optimized
rm -rf workspace*                  
cd $ReproduceBaseDIR                       

cd HimenoBMT_Optimized  
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd Kripke_representative_loop_Optimized  
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd NW_Rodinia            
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd MKL_FFT                              
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd NW_Rodinia_Optimized 
rm -rf workspace*
cd $ReproduceBaseDIR    
                   
cd Tiny_DNN_representative_loop
rm -rf workspace*
cd $ReproduceBaseDIR

rm *Stat.txt
rm ../*Stat.txt
