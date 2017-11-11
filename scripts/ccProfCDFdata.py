import numpy as np
import pandas as pd
from fractions import gcd
import sys

#from sklearn.externals import joblib
import pickle

from sklearn.cross_validation import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn import metrics
from sklearn.metrics import f1_score
from sklearn.model_selection import StratifiedKFold
from sklearn.model_selection import cross_val_score

DATA_SET_PATH = "./modelInput/model_training_input.csv"
#DATA_SET_PATH = "./modelInput/freq_1212.csv"
#DATA_SET_PATH = "./modelInput/freq_171.csv"
#DATA_SET_PATH = "./modelInput/freq_264.csv"
#DATA_SET_PATH = "./modelInput/freq_494.csv"
#DATA_SET_PATH = "./modelInput/freq_705.csv"
#DATA_SET_PATH = "./modelInput/freq_4812.csv"
#DATA_SET_PATH = "./modelInput/freq_10151.csv"

fname_loop = "./loops"

with open(fname_loop, 'r') as floop:
    loopData = np.genfromtxt(floop,comments="!", dtype=np.uint64, skip_header=1) #names=True)

#print loopData

fname_sample = "./sampledAccess"

with open(fname_sample, 'r') as fsample:
    sampleData = np.genfromtxt(fsample,comments="!", dtype=np.uint64) #names=True)

#adding column to hold loop information
sampleData = np.insert(sampleData,0,0,axis=1)

#adding column to hold RCD information
sampleData = np.insert(sampleData,0,0,axis=1)

#Does sample ip fall within loop bondary? Then Insert loop line number in column 3 of sampleData
#loop identifier
lid = loopData[:,0]
#lower boundary
l = loopData[:,2]
#upper boundary
u = loopData[:,3]
#sample ip
s = sampleData[:,4]
#get shape of loopData
ls = loopData.shape
#loop through each row of loopData and compare ip range with samples ip
for i in range(ls[0]):
	#if within range add loop start ip in the first column of sampleData
	sampleData[:,0] = np.where((l[i] <= s) & (s <=u[i]),lid[i],sampleData[:,0])

#now iterate by loop and in each loop iterate by cache set and calculate RCD distance and frequency.
#getting loop identifier
lid = np.unique(loopData[:,0], return_counts=False)
#print lid.shape
#getting cachesets 
cset = np.unique(sampleData[:,10], return_counts=False)
#get shape of loopData
ls = lid.shape
#get shape of allocData
csets = cset.shape
#for compulsary miss set RCD distance of INF
INF=1000000
#Set a threshold
THRESHOLD = 9
#calculate RCD irrespective of loop
#iterate over each cache set
for k in range(cset.shape[0]):
	#iterate over all samples
	prevAccess = INF
	for j in range(sampleData.shape[0]):
		if sampleData[j,10] == cset[k]:
			if(prevAccess!=INF):
				sampleData[j,1] = j - prevAccess
			else:
				sampleData[j,1] = INF
			prevAccess = j

#at this point we have calculated RCD
np.savetxt("inLoop", sampleData,fmt='%.0f')

#next we are going to per Loop analysis for RCD distribution
for i in range(ls[0]):
	#first, filter by loop
	samplePerLoop = np.array([row for row in sampleData if row[0] == lid[i] ])
	if len(samplePerLoop)==0:
		continue
	belowThreshold = 0
        aboveThreshold = 0
	sumAllSet = 0
	cdfBufferAllSet = np.zeros(shape=(cset.shape[0]+1,2), dtype=float)
	rcdBufferAllSet = np.zeros(shape=(cset.shape[0]+1,2), dtype=float)
	#then filter by set
	for k in range(cset.shape[0]):
		#iterate over all samples within this set
		samplePerLoopPerSet = np.array([r for r in samplePerLoop if r[10] == cset[k] ])
		#XXX:sanity checking: if no or one sample, skip
                if samplePerLoopPerSet.shape[0] <= 1:
                        continue
		#get histogram of RCD of this set
		uniqueRCD,counts = np.unique((samplePerLoopPerSet[:,1]), return_counts=True)
		rcdHisto = np.asarray((uniqueRCD, counts)).T

		rcdHisto.astype(int)
                rcdHisto[np.lexsort(np.transpose(rcdHisto)[:-1])]

		#now calcluate contribution factor below threshold
		for j in range(rcdHisto.shape[0]):
			index1=rcdHisto[j,0]
			index = int(index1)
			if index < (cset.shape[0]+1):
				rcdBufferAllSet[index-1,0] = index
				rcdBufferAllSet[index-1,1] = rcdBufferAllSet[index-1,1] + rcdHisto[j,1]
			else:
				rcdBufferAllSet[cset.shape[0],0] = cset.shape[0]
		                rcdBufferAllSet[cset.shape[0],1] = rcdBufferAllSet[cset.shape[0],1] + rcdHisto[j,1]
	
	#calculate %
	sumThisSet = rcdBufferAllSet.sum(axis=0)[1]
	#XXX: sanity cheking: if no data for this loop
        if sumThisSet == 0:
                continue

	for m in range(rcdBufferAllSet.shape[0]):
		rcdBufferAllSet[m,1] = 100 * float(((float(rcdBufferAllSet[m,1]) / float(sumThisSet))))

	#now calculate CDF
	cumulativeSum = 0
	for m in range(rcdBufferAllSet.shape[0]):
		cumulativeSum = cumulativeSum + rcdBufferAllSet[m,1]
		cdfBufferAllSet[m,0] = m + 1
		cdfBufferAllSet[m,1] = cumulativeSum
	
	#get the loop name by line number, it might cause disambiguity: could use IP, but will impact on reproducibility
	whichLoop =  np.array([r[1] for r in loopData if r[0]==lid[i]])
	whichLoop.astype(int)
	loopLine = int(np.sort(whichLoop)[0])

	filename = "CDF_of_Loop_at_" + str(loopLine)
	np.savetxt(filename, cdfBufferAllSet,fmt='%.1f')
	print "Done generating cdf of Loop at " + str(loopLine)
