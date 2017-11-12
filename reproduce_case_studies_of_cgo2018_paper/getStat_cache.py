import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

fname_mklFFT_OPT = "./MKL_FFT_Optimized/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_mklFFT_OPT, 'r') as fmklFFT_OPT:
    mklFFT_OPT = np.genfromtxt(fmklFFT_OPT, dtype=np.float)

fname_mklFFT = "./MKL_FFT/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_mklFFT, 'r') as fmklFFT:
    mklFFT = np.genfromtxt(fmklFFT, dtype=np.float)

fname_ADI_PolyBench = "./ADI_PolyBench/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_ADI_PolyBench, 'r') as fADI_PolyBench:
    ADI_PolyBench = np.genfromtxt(fADI_PolyBench, dtype=np.float)

fname_ADI_PolyBench_Optimized = "./ADI_PolyBench_Optimized/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_ADI_PolyBench_Optimized, 'r') as fADI_PolyBench_Optimized:
    ADI_PolyBench_Optimized = np.genfromtxt(fADI_PolyBench_Optimized, dtype=np.float)


fname_HimenoBMT = "./HimenoBMT/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_HimenoBMT, 'r') as fHimenoBMT:
    HimenoBMT = np.genfromtxt(fHimenoBMT, dtype=np.float)

fname_HimenoBMT_Optimized = "./HimenoBMT_Optimized/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_HimenoBMT_Optimized, 'r') as fHimenoBMT_Optimized:
    HimenoBMT_Optimized = np.genfromtxt(fHimenoBMT_Optimized, dtype=np.float)


fname_Kripke_representative_loop = "./Kripke_representative_loop/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_Kripke_representative_loop, 'r') as fKripke_representative_loop:
    Kripke_representative_loop = np.genfromtxt(fKripke_representative_loop, dtype=np.float)

fname_Kripke_representative_loop_Optimized = "./Kripke_representative_loop_Optimized/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_Kripke_representative_loop_Optimized, 'r') as fKripke_representative_loop_Optimized:
    Kripke_representative_loop_Optimized = np.genfromtxt(fKripke_representative_loop_Optimized, dtype=np.float)

fname_NW_Rodinia = "./NW_Rodinia/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_NW_Rodinia, 'r') as fNW_Rodinia:
    NW_Rodinia = np.genfromtxt(fNW_Rodinia, dtype=np.float)

fname_NW_Rodinia_Optimized = "./NW_Rodinia_Optimized/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_NW_Rodinia_Optimized, 'r') as fNW_Rodinia_Optimized:
    NW_Rodinia_Optimized = np.genfromtxt(fNW_Rodinia_Optimized, dtype=np.float)


fname_Tiny_DNN_representative_loop = "./Tiny_DNN_representative_loop/workspace/cacheStat_L1L_L1M_L2M_LLCM"

with open(fname_Tiny_DNN_representative_loop, 'r') as fTiny_DNN_representative_loop:
    Tiny_DNN_representative_loop = np.genfromtxt(fTiny_DNN_representative_loop, dtype=np.float)

fname_Tiny_DNN_representative_loop_Optimized = "./Tiny_DNN_representative_loop_Optimized/workspace/cacheStat_L1L_L1M_L2M_LLCM"

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

y1 = ADI_PolyBench[:]
y2 = NW_Rodinia[:]
y3 = mklFFT[:]
y4 = Tiny_DNN_representative_loop[:]
y5 = Kripke_representative_loop[:]
y6 = HimenoBMT[:]
y1_1 = ADI_PolyBench_Optimized[:]
y2_1 = NW_Rodinia_Optimized[:]
y3_1 = mklFFT_OPT[:]
y4_1 = Tiny_DNN_representative_loop_Optimized[:]
y5_1 = Kripke_representative_loop_Optimized[:]
y6_1 = HimenoBMT_Optimized[:]



with open("cacheStat.txt", "w") as myfile:
    myfile.write("\n####################################### cache miss reduction after transformation (compiler optimization level: -O3) #####################################################\n")

with open("cacheStat.txt", "a") as myfile:
    myfile.write("\nApplication\tL1-Miss-reduction\tL2-Miss-reduction\tLLC-Miss-reduction")
#
L1_Miss_reduction_y1=100*((y1[1]/y1[0])-(y1_1[1]/y1_1[0]))/(y1[1]/y1[0])
L2_Miss_reduction_y1=100*((y1[2]/y1[1])-(y1_1[2]/y1_1[1]))/(y1[2]/y1[1])
LLC_Miss_reduction_y1=100*((y1[3]/y1[2])-(y1_1[3]/y1_1[2]))/(y1[3]/y1[2])
with open("cacheStat.txt", "a") as myfile:
    myfile.write("\nPoly-ADI\t"+str(L1_Miss_reduction_y1)+"\t"+str(L2_Miss_reduction_y1)+"\t"+str(LLC_Miss_reduction_y1))


#
L1_Miss_reduction_y2=100*((y2[1]/y2[0])-(y2_1[1]/y2_1[0]))/(y2[1]/y2[0])
L2_Miss_reduction_y2=100*((y2[2]/y2[1])-(y2_1[2]/y2_1[1]))/(y2[2]/y2[1])
LLC_Miss_reduction_y2=100*((y2[3]/y2[2])-(y2_1[3]/y2_1[2]))/(y2[3]/y2[2])
with open("cacheStat.txt", "a") as myfile:
    myfile.write("\nNW_Rodinia\t"+str(L1_Miss_reduction_y2)+"\t"+str(L2_Miss_reduction_y2)+"\t"+str(LLC_Miss_reduction_y2))

#
L1_Miss_reduction_y3=100*((y3[1]/y3[0])-(y3_1[1]/y3_1[0]))/(y3[1]/y3[0])
L2_Miss_reduction_y3=100*((y3[2]/y3[1])-(y3_1[2]/y3_1[1]))/(y3[2]/y3[1])
LLC_Miss_reduction_y3=100*((y3[3]/y3[2])-(y3_1[3]/y3_1[2]))/(y3[3]/y3[2])
with open("cacheStat.txt", "a") as myfile:
    myfile.write("\nmklFFT\t"+str(L1_Miss_reduction_y3)+"\t"+str(L2_Miss_reduction_y3)+"\t"+str(LLC_Miss_reduction_y3))

#
L1_Miss_reduction_y4=100*((y4[1]/y4[0])-(y4_1[1]/y4_1[0]))/(y4[1]/y4[0])
L2_Miss_reduction_y4=100*((y4[2]/y4[1])-(y4_1[2]/y4_1[1]))/(y4[2]/y4[1])
LLC_Miss_reduction_y4=100*((y4[3]/y4[2])-(y4_1[3]/y4_1[2]))/(y4[3]/y4[2])
with open("cacheStat.txt", "a") as myfile:
    myfile.write("\nTiny_DNN\t"+str(L1_Miss_reduction_y4)+"\t"+str(L2_Miss_reduction_y4)+"\t"+str(LLC_Miss_reduction_y4))


#
L1_Miss_reduction_y5=100*((y5[1]/y5[0])-(y5_1[1]/y5_1[0]))/(y5[1]/y5[0])
L2_Miss_reduction_y5=100*((y5[2]/y5[1])-(y5_1[2]/y5_1[1]))/(y5[2]/y5[1])
LLC_Miss_reduction_y5=100*((y5[3]/y5[2])-(y5_1[3]/y5_1[2]))/(y5[3]/y5[2])
with open("cacheStat.txt", "a") as myfile:
    myfile.write("\nKripke\t"+str(L1_Miss_reduction_y5)+"\t"+str(L2_Miss_reduction_y5)+"\t"+str(LLC_Miss_reduction_y5))

#
L1_Miss_reduction_y6=100*((y6[1]/y6[0])-(y6_1[1]/y6_1[0]))/(y6[1]/y6[0])
L2_Miss_reduction_y6=100*((y6[2]/y6[1])-(y6_1[2]/y6_1[1]))/(y6[2]/y6[1])
LLC_Miss_reduction_y6=100*((y6[3]/y6[2])-(y6_1[3]/y6_1[2]))/(y6[3]/y6[2])
with open("cacheStat.txt", "a") as myfile:
    myfile.write("\nHimenoBMT\t"+str(L1_Miss_reduction_y6)+"\t"+str(L2_Miss_reduction_y6)+"\t"+str(LLC_Miss_reduction_y6))

with open("cacheStat.txt", "a") as myfile:
    myfile.write("\n############################################################################################\n")
