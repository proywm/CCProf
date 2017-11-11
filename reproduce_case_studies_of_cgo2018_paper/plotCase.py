import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

fname_mklFFT_OPT = "./MKL_FFT_Optimized/workspace/CDF_of_Loop_at_0_0"

with open(fname_mklFFT_OPT, 'r') as fmklFFT_OPT:
    mklFFT_OPT = np.genfromtxt(fmklFFT_OPT, dtype=np.float)

fname_mklFFT = "./MKL_FFT/workspace/CDF_of_Loop_at_0_0"

with open(fname_mklFFT, 'r') as fmklFFT:
    mklFFT = np.genfromtxt(fmklFFT, dtype=np.float)

fname_ADI_PolyBench = "./ADI_PolyBench/workspace/CDF_of_Loop_at_102"

with open(fname_ADI_PolyBench, 'r') as fADI_PolyBench:
    ADI_PolyBench = np.genfromtxt(fADI_PolyBench, dtype=np.float)

fname_ADI_PolyBench_Optimized = "./ADI_PolyBench_Optimized/workspace/CDF_of_Loop_at_102"

with open(fname_ADI_PolyBench_Optimized, 'r') as fADI_PolyBench_Optimized:
    ADI_PolyBench_Optimized = np.genfromtxt(fADI_PolyBench_Optimized, dtype=np.float)


fname_HimenoBMT = "./HimenoBMT/workspace/CDF_of_Loop_at_294"

with open(fname_HimenoBMT, 'r') as fHimenoBMT:
    HimenoBMT = np.genfromtxt(fHimenoBMT, dtype=np.float)

fname_HimenoBMT_Optimized = "./HimenoBMT_Optimized/workspace/CDF_of_Loop_at_296"

with open(fname_HimenoBMT_Optimized, 'r') as fHimenoBMT_Optimized:
    HimenoBMT_Optimized = np.genfromtxt(fHimenoBMT_Optimized, dtype=np.float)


fname_Kripke_representative_loop = "./Kripke_representative_loop/workspace/CDF_of_Loop_at_37"

with open(fname_Kripke_representative_loop, 'r') as fKripke_representative_loop:
    Kripke_representative_loop = np.genfromtxt(fKripke_representative_loop, dtype=np.float)

fname_Kripke_representative_loop_Optimized = "./Kripke_representative_loop_Optimized/workspace/CDF_of_Loop_at_36"

with open(fname_Kripke_representative_loop_Optimized, 'r') as fKripke_representative_loop_Optimized:
    Kripke_representative_loop_Optimized = np.genfromtxt(fKripke_representative_loop_Optimized, dtype=np.float)

fname_NW_Rodinia = "./NW_Rodinia/workspace/CDF_of_Loop_at_130"

with open(fname_NW_Rodinia, 'r') as fNW_Rodinia:
    NW_Rodinia = np.genfromtxt(fNW_Rodinia, dtype=np.float)

fname_NW_Rodinia_Optimized = "./NW_Rodinia_Optimized/workspace/CDF_of_Loop_at_200"

with open(fname_NW_Rodinia_Optimized, 'r') as fNW_Rodinia_Optimized:
    NW_Rodinia_Optimized = np.genfromtxt(fNW_Rodinia_Optimized, dtype=np.float)


fname_Tiny_DNN_representative_loop = "./Tiny_DNN_representative_loop/workspace/CDF_of_Loop_at_33"

with open(fname_Tiny_DNN_representative_loop, 'r') as fTiny_DNN_representative_loop:
    Tiny_DNN_representative_loop = np.genfromtxt(fTiny_DNN_representative_loop, dtype=np.float)

fname_Tiny_DNN_representative_loop_Optimized = "./Tiny_DNN_representative_loop_Optimized/workspace/CDF_of_Loop_at_34"

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

x = Tiny_DNN_representative_loop_Optimized[:,0]
#print x
y1 = ADI_PolyBench[:,1]
y2 = NW_Rodinia[:,1]
y3 = mklFFT[:,1]
y4 = Tiny_DNN_representative_loop[:,1]
y5 = Kripke_representative_loop[:,1]
y6 = HimenoBMT[:,1]
y1_1 = ADI_PolyBench_Optimized[:,1]
y2_1 = NW_Rodinia_Optimized[:,1]
y3_1 = mklFFT_OPT[:,1]
y4_1 = Tiny_DNN_representative_loop_Optimized[:,1]
y5_1 = Kripke_representative_loop_Optimized[:,1]
y6_1 = HimenoBMT_Optimized[:,1]

#print(x)

fig = plt.figure()

ax1 = fig.add_subplot(2,3,2)
ax1.plot(x, y1, label='Original', linewidth=3)
ax1.plot(x, y1_1, label='PADDED', linewidth=3)
ax1.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax1.grid(True)
ax1.set_title('Poly ADI',fontsize=24)
ax1.axvline(x=8,color='black', linewidth=5 , linestyle='--')
ax1.fill_between(x, y1, 0, interpolate=True, alpha=.3)
ax1.fill_between(x, y1_1, 0, interpolate=True, alpha=.3)
ax1.set_xticks(np.arange(min(x), max(x)+1, 7))
ax1.set_yticks(np.arange(0, 101, 20))
ax1.set_xlim([1,65])
ax1.set_ylim([0,101])

ax2 = fig.add_subplot(2,3,1)
ax2.plot(x, y2, label='Original', linewidth=3)
ax2.plot(x, y2_1, label='PADDED', linewidth=3)
ax2.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax2.grid(True)
ax2.set_title('Needleman-Wunsch',fontsize=24)
ax2.axvline(x=8,color='black', linewidth=5 , linestyle='--')
ax2.fill_between(x, y2, 0, interpolate=True, alpha=.3)
ax2.fill_between(x, y2_1, 0, interpolate=True, alpha=.3)
ax2.set_xticks(np.arange(min(x), max(x)+1, 7))
ax2.set_yticks(np.arange(0, 101, 20))
ax2.set_xlim([1,65])
ax2.set_ylim([0,101])

ax3 = fig.add_subplot(2,3,3)
ax3.plot(x, y3, label='Original', linewidth=3)
ax3.plot(x, y3_1, label='PADDED', linewidth=3)
ax3.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax3.grid(True)
ax3.set_title('MKL-FFT',fontsize=24)
ax3.axvline(x=8,color='black', linewidth=5 , linestyle='--')
ax3.fill_between(x, y3, 0, interpolate=True, alpha=.3)
ax3.fill_between(x, y3_1, 0, interpolate=True, alpha=.3)
ax3.set_xticks(np.arange(min(x), max(x)+1, 7))
ax3.set_yticks(np.arange(0, 101, 20))
ax3.set_xlim([1,65])
ax3.set_ylim([0,101])

ax4 = fig.add_subplot(2,3,4)
ax4.plot(x, y4, label='Original', linewidth=3)
ax4.plot(x, y4_1, label='PADDED', linewidth=3)
ax4.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax4.grid(True)
ax4.set_title('Tiny-DNN',fontsize=24)
ax4.axvline(x=8,color='black', linewidth=5 , linestyle='--')
ax4.fill_between(x, y4, 0, interpolate=True, alpha=.3)
ax4.fill_between(x, y4_1, 0, interpolate=True, alpha=.3)
ax4.set_xticks(np.arange(min(x), max(x)+1, 7))
ax4.set_yticks(np.arange(0, 101, 20))
ax4.set_xlim([1,65])
ax4.set_ylim([0,101])


ax5 = fig.add_subplot(2,3,5)
ax5.plot(x, y5, label='Original', linewidth=3)
ax5.plot(x, y5_1, label='PADDED', linewidth=3)
ax5.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax5.grid(True)
ax5.set_title('Kripke',fontsize=24)
ax5.axvline(x=8,color='black', linewidth=5 , linestyle='--')
ax5.fill_between(x, y5, 0, interpolate=True, alpha=.3)
ax5.fill_between(x, y5_1, 0, interpolate=True, alpha=.3)
ax5.set_xticks(np.arange(min(x), max(x)+1, 7))
ax5.set_yticks(np.arange(0, 101, 20))
ax5.set_xlim([1,65])
ax5.set_ylim([0,101])

ax6 = fig.add_subplot(2,3,6)
l1 = ax6.plot(x, y6, label='Original', linewidth=3)
l2 = ax6.plot(x, y6_1, label='PADDED', linewidth=3)
ax6.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax6.grid(True)
ax6.set_title('HimenoBMT',fontsize=24)
ax6.axvline(x=8,color='black', linewidth=5 , linestyle='--')
ax6.fill_between(x, y6, 0, interpolate=True, alpha=.3)
ax6.fill_between(x, y6_1, 0, interpolate=True, alpha=.3)
ax6.set_xticks(np.arange(min(x), max(x)+1, 7))
ax6.set_yticks(np.arange(0, 101, 20))
ax6.set_xlim([1,65])
ax6.set_ylim([0,101])


#ax1.legend(loc = (.76,.75), prop={'size': 24}, shadow=True)
#ax2.legend( loc = (.76, .65), prop={'size': 24}, shadow=True)

#fig.legend((l1, l2), 'upper left')

#plt.ylabel('CP', fontsize=24)
#plt.xlim((1,65))
#plt.ylim((0,101))
#plt.yticks(np.arange(0, 101, 20),fontsize=20)
#plt.xticks(np.arange(min(x), max(x)+1, 7),fontsize=20)


box = ax1.get_position()
#ax1.set_position([box.x0 , box.y0 + box.height * 0.1,
#                 box.width, box.height * 0.85])

# Put a legend below current axis
ax1.legend(loc='upper center', bbox_to_anchor=(0.5, -1.55),
          fancybox=False, shadow=True, ncol=5, prop={'size': 24})

# Set common labels
fig.text(0.5, 0.00, 'Average RCD-distance', ha='center', va='center',fontsize=24)
fig.text(0.06, 0.5, 'Cumulative probability of L1 cache miss', ha='center', va='center', rotation='vertical',fontsize=24)
fig.subplots_adjust(wspace=.5)

plt.show()
fig.savefig("CCProfPredictionOnCaseStudies_Figure9.pdf", bbox_inches='tight')
