cd $CCPROFDIR/..
mkdir CCPROF_result
cd CCPROF_result
RESULTDIR=$PWD
cd $CCPROFDIR/..
cd reproduce_case_studies_of_cgo2018_paper
python plotCase_ADI.py        
python plotCase_HimenoBMT.py  
python plotCase_Kripke.py     
python plotCase_Tiny_DNN.py
python plotCase_NW.py
pyton plotCase_MKLFFT.py     
mv *.pdf  $RESULTDIR/
