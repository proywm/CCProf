import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

fname_Kripke_representative_loop = "./Kripke_representative_loop/workspace/CDF_of_Loop_at_37"

with open(fname_Kripke_representative_loop, 'r') as fKripke_representative_loop:
    Kripke_representative_loop = np.genfromtxt(fKripke_representative_loop, dtype=np.float)

fname_Kripke_representative_loop_Optimized = "./Kripke_representative_loop_Optimized/workspace/CDF_of_Loop_at_36"

with open(fname_Kripke_representative_loop_Optimized, 'r') as fKripke_representative_loop_Optimized:
    Kripke_representative_loop_Optimized = np.genfromtxt(fKripke_representative_loop_Optimized, dtype=np.float)

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

x = Kripke_representative_loop[:,0]
#print x
y1 = Kripke_representative_loop[:,1]
y1_1 = Kripke_representative_loop_Optimized[:,1]

#print(x)

#fig = plt.figure()

#ax1 = fig.add_subplot(2,3,2)

fig, ax1 = plt.subplots()


ax1.plot(x, y1, label='Original', linewidth=3)
ax1.plot(x, y1_1, label='PADDED', linewidth=3)
ax1.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
ax1.grid(True)
ax1.set_title('Kripke',fontsize=24)
ax1.axvline(x=8,color='black', linewidth=5 , linestyle='--')
ax1.fill_between(x, y1, 0, interpolate=True, alpha=.3)
ax1.fill_between(x, y1_1, 0, interpolate=True, alpha=.3)
ax1.set_xticks(np.arange(min(x), max(x)+1, 7))
ax1.set_yticks(np.arange(0, 101, 20))
ax1.set_xlim([1,65])
ax1.set_ylim([0,101])



box = ax1.get_position()
ax1.set_position([box.x0 , box.y0 + box.height * 0.1,
                 box.width, box.height * 0.85])

# Put a legend below current axis
ax1.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15),
          fancybox=False, shadow=True, ncol=5, prop={'size': 24})

# Set common labels
fig.text(0.5, 0.00, 'Average RCD-distance', ha='center', va='center',fontsize=24)
fig.text(0.06, 0.5, 'Cumulative probability of L1 cache miss', ha='center', va='center', rotation='vertical',fontsize=24)
fig.subplots_adjust(wspace=.5)

#plt.show()
fig.savefig("CCProf_CDF_RCD_Kripke_Figure9.pdf", bbox_inches='tight')
