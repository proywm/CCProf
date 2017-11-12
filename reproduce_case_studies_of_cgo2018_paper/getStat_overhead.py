import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

fname_mklFFT_noCCPROF = "./MKL_FFT/workspace/runtime_without_ccProf"

with open(fname_mklFFT_noCCPROF, 'r') as fmklFFT_noCCPROF:
    mklFFT_noCCPROF = np.genfromtxt(fmklFFT_noCCPROF, dtype=np.float)

fname_mklFFT = "./MKL_FFT/workspace/runtime_ccProf"

with open(fname_mklFFT, 'r') as fmklFFT:
    mklFFT = np.genfromtxt(fmklFFT, dtype=np.float)

fname_ADI_PolyBench = "./ADI_PolyBench/workspace/runtime_ccProf"

with open(fname_ADI_PolyBench, 'r') as fADI_PolyBench:
    ADI_PolyBench = np.genfromtxt(fADI_PolyBench, dtype=np.float)

fname_ADI_PolyBench_noCCPROF = "./ADI_PolyBench/workspace/runtime_without_ccProf"

with open(fname_ADI_PolyBench_noCCPROF, 'r') as fADI_PolyBench_noCCPROF:
    ADI_PolyBench_noCCPROF = np.genfromtxt(fADI_PolyBench_noCCPROF, dtype=np.float)


fname_HimenoBMT = "./HimenoBMT/workspace/runtime_ccProf"

with open(fname_HimenoBMT, 'r') as fHimenoBMT:
    HimenoBMT = np.genfromtxt(fHimenoBMT, dtype=np.float)

fname_HimenoBMT_noCCPROF = "./HimenoBMT/workspace/runtime_without_ccProf"

with open(fname_HimenoBMT_noCCPROF, 'r') as fHimenoBMT_noCCPROF:
    HimenoBMT_noCCPROF = np.genfromtxt(fHimenoBMT_noCCPROF, dtype=np.float)


fname_Kripke_representative_loop = "./Kripke_representative_loop/workspace/runtime_ccProf"

with open(fname_Kripke_representative_loop, 'r') as fKripke_representative_loop:
    Kripke_representative_loop = np.genfromtxt(fKripke_representative_loop, dtype=np.float)

fname_Kripke_representative_loop_noCCPROF = "./Kripke_representative_loop/workspace/runtime_without_ccProf"

with open(fname_Kripke_representative_loop_noCCPROF, 'r') as fKripke_representative_loop_noCCPROF:
    Kripke_representative_loop_noCCPROF = np.genfromtxt(fKripke_representative_loop_noCCPROF, dtype=np.float)

fname_NW_Rodinia = "./NW_Rodinia/workspace/runtime_ccProf"

with open(fname_NW_Rodinia, 'r') as fNW_Rodinia:
    NW_Rodinia = np.genfromtxt(fNW_Rodinia, dtype=np.float)

fname_NW_Rodinia_noCCPROF = "./NW_Rodinia/workspace/runtime_without_ccProf"

with open(fname_NW_Rodinia_noCCPROF, 'r') as fNW_Rodinia_noCCPROF:
    NW_Rodinia_noCCPROF = np.genfromtxt(fNW_Rodinia_noCCPROF, dtype=np.float)


fname_Tiny_DNN_representative_loop = "./Tiny_DNN_representative_loop/workspace/runtime_ccProf"

with open(fname_Tiny_DNN_representative_loop, 'r') as fTiny_DNN_representative_loop:
    Tiny_DNN_representative_loop = np.genfromtxt(fTiny_DNN_representative_loop, dtype=np.float)

fname_Tiny_DNN_representative_loop_noCCPROF = "./Tiny_DNN_representative_loop/workspace/runtime_without_ccProf"

with open(fname_Tiny_DNN_representative_loop_noCCPROF, 'r') as fTiny_DNN_representative_loop_noCCPROF:
    Tiny_DNN_representative_loop_noCCPROF = np.genfromtxt(fTiny_DNN_representative_loop_noCCPROF, dtype=np.float)




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
y1_1 = ADI_PolyBench_noCCPROF
y2_1 = NW_Rodinia_noCCPROF
y3_1 = mklFFT_noCCPROF
y4_1 = Tiny_DNN_representative_loop_noCCPROF
y5_1 = Kripke_representative_loop_noCCPROF
y6_1 = HimenoBMT_noCCPROF

with open("overheadStat.txt", "w") as myfile:
    myfile.write("\n####################################### CCProf Overhead (compiler optimization level : -O0) #####################################################\n")

with open("overheadStat.txt", "a") as myfile:
    myfile.write("\nApplication\tCCProf-Overhead")
#
overheady1=(y1/y1_1)
with open("overheadStat.txt", "a") as myfile:
    myfile.write("\nPoly-ADI\t"+"{:.2f}".format(overheady1)+"x")


#
overheady2=(y2/y2_1)
with open("overheadStat.txt", "a") as myfile:
    myfile.write("\nNW_Rodinia\t"+"{:.2f}".format(overheady2)+"x")

#
overheady3=y3/y3_1
with open("overheadStat.txt", "a") as myfile:
    myfile.write("\nmklFFT\t"+"{:.2f}".format(overheady3)+"x")

#
overheady4=y4/y4_1
with open("overheadStat.txt", "a") as myfile:
    myfile.write("\nTiny_DNN\t"+"{:.2f}".format(overheady4)+"x")


#
overheady5=y5/y5_1
with open("overheadStat.txt", "a") as myfile:
    myfile.write("\nKripke\t"+"{:.2f}".format(overheady5)+"x")

#
overheady6=y6/y6_1
with open("overheadStat.txt", "a") as myfile:
    myfile.write("\nHimenoBMT\t"+"{:.2f}".format(overheady6)+"x")


with open("overheadStat.txt", "a") as myfile:
    myfile.write("\n##################################################################################################################\n")
