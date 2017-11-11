/home/proy/r/install/bin/Rscript $*
parentdir=${PWD##*/}
echo $parentdir
out=PlotOverSet_L1_"${parentdir}".pdf
mv PlotOverSet_L1.pdf "${out}"
