import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

fname_mklFFT_OPT = "./MKL_FFT_Optimized/workspace/runtime_O3"

with open(fname_mklFFT_OPT, 'r') as fmklFFT_OPT:
    mklFFT_OPT = np.genfromtxt(fmklFFT_OPT, dtype=np.float)

fname_mklFFT = "./MKL_FFT/workspace/runtime_O3"

with open(fname_mklFFT, 'r') as fmklFFT:
    mklFFT = np.genfromtxt(fmklFFT, dtype=np.float)

fname_ADI_PolyBench = "./ADI_PolyBench/workspace/runtime_O3"

with open(fname_ADI_PolyBench, 'r') as fADI_PolyBench:
    ADI_PolyBench = np.genfromtxt(fADI_PolyBench, dtype=np.float)

fname_ADI_PolyBench_Optimized = "./ADI_PolyBench_Optimized/workspace/runtime_O3"

with open(fname_ADI_PolyBench_Optimized, 'r') as fADI_PolyBench_Optimized:
    ADI_PolyBench_Optimized = np.genfromtxt(fADI_PolyBench_Optimized, dtype=np.float)


fname_HimenoBMT = "./HimenoBMT/workspace/runtime_O3"

with open(fname_HimenoBMT, 'r') as fHimenoBMT:
    HimenoBMT = np.genfromtxt(fHimenoBMT, dtype=np.float)

fname_HimenoBMT_Optimized = "./HimenoBMT_Optimized/workspace/runtime_O3"

with open(fname_HimenoBMT_Optimized, 'r') as fHimenoBMT_Optimized:
    HimenoBMT_Optimized = np.genfromtxt(fHimenoBMT_Optimized, dtype=np.float)


fname_Kripke_representative_loop = "./Kripke_representative_loop/workspace/runtime_O3"

with open(fname_Kripke_representative_loop, 'r') as fKripke_representative_loop:
    Kripke_representative_loop = np.genfromtxt(fKripke_representative_loop, dtype=np.float)

fname_Kripke_representative_loop_Optimized = "./Kripke_representative_loop_Optimized/workspace/runtime_O3"

with open(fname_Kripke_representative_loop_Optimized, 'r') as fKripke_representative_loop_Optimized:
    Kripke_representative_loop_Optimized = np.genfromtxt(fKripke_representative_loop_Optimized, dtype=np.float)

fname_NW_Rodinia = "./NW_Rodinia/workspace/runtime_O3"

with open(fname_NW_Rodinia, 'r') as fNW_Rodinia:
    NW_Rodinia = np.genfromtxt(fNW_Rodinia, dtype=np.float)

fname_NW_Rodinia_Optimized = "./NW_Rodinia_Optimized/workspace/runtime_O3"

with open(fname_NW_Rodinia_Optimized, 'r') as fNW_Rodinia_Optimized:
    NW_Rodinia_Optimized = np.genfromtxt(fNW_Rodinia_Optimized, dtype=np.float)


fname_Tiny_DNN_representative_loop = "./Tiny_DNN_representative_loop/workspace/runtime_O3"

with open(fname_Tiny_DNN_representative_loop, 'r') as fTiny_DNN_representative_loop:
    Tiny_DNN_representative_loop = np.genfromtxt(fTiny_DNN_representative_loop, dtype=np.float)

fname_Tiny_DNN_representative_loop_Optimized = "./Tiny_DNN_representative_loop_Optimized/workspace/runtime_O3"

with open(fname_Tiny_DNN_representative_loop_Optimized, 'r') as fTiny_DNN_representative_loop_Optimized:
    Tiny_DNN_representative_loop_Optimized = np.genfromtxt(fTiny_DNN_representative_loop_Optimized, dtype=np.float)




x_axis = ['RCD-distance']
#RCD-distance,Poly-ADI-Original,Poly-ADI-PADDED,Needleman-Wunsch-Original,Needleman-Wunsch-PADDED,MKL-FFT-Original,MKL-FFT-PADDED,Tiny-DNN-Original,Tiny-DNN-PADDED,Kripke-Original,Kripke-PADDED,HimenoBMT-Original,HimenoBMT-PADDED
y1_axis = ['Poly-ADI-Original']
y1_1_axis = ['Poly-ADI-PADDED']
y2_axis = ['Needleman-Wunsch-Original']
y2_1_axis = ['Needleman-Wunsch-PADDED']
y3_axis = ['MKL-FFT-Original']
y3_1_axis = ['MKL-FFT-PADDED']
y4_axis = ['Tiny-DNN-Original']
y4_1_axis = ['Tiny-DNN-PADDED']
y5_axis = ['Kripke-Original']
y5_1_axis = ['Kripke-PADDED']
y6_axis = ['HimenoBMT-Original']
y6_1_axis = ['HimenoBMT-PADDED']

y1 = ADI_PolyBench
y2 = NW_Rodinia
y3 = mklFFT
y4 = Tiny_DNN_representative_loop
y5 = Kripke_representative_loop
y6 = HimenoBMT
y1_1 = ADI_PolyBench_Optimized
y2_1 = NW_Rodinia_Optimized
y3_1 = mklFFT_OPT
y4_1 = Tiny_DNN_representative_loop_Optimized
y5_1 = Kripke_representative_loop_Optimized
y6_1 = HimenoBMT_Optimized


with open("speedupStat.txt", "a") as myfile:
    myfile.write("\nApplication\tSpeedup")
#
speedupy1=(y1/y1_1)
with open("speedupStat.txt", "a") as myfile:
    myfile.write("\nPoly-ADI\t"+"{:.2f}".format(speedupy1)+"x")


#
speedupy2=(y2/y2_1)
with open("speedupStat.txt", "a") as myfile:
    myfile.write("\nNW_Rodinia\t"+"{:.2f}".format(speedupy2)+"x")

#
speedupy3=y3/y3_1
with open("speedupStat.txt", "a") as myfile:
    myfile.write("\nmklFFT\t"+"{:.2f}".format(speedupy3)+"x")

#
speedupy4=y4/y4_1
with open("speedupStat.txt", "a") as myfile:
    myfile.write("\nTiny_DNN\t"+"{:.2f}".format(speedupy4)+"x")


#
speedupy5=y5/y5_1
with open("speedupStat.txt", "a") as myfile:
    myfile.write("\nKripke\t"+"{:.2f}".format(speedupy5)+"x")

#
speedupy6=y6/y6_1
with open("speedupStat.txt", "a") as myfile:
    myfile.write("\nHimenoBMT\t"+"{:.2f}".format(speedupy6)+"x")

