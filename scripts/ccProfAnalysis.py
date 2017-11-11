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
print ls
#loop through each row of loopData and compare ip range with samples ip
for i in range(ls[0]):
	#if within range add loop start ip in the first column of sampleData
	sampleData[:,0] = np.where((l[i] <= s) & (s <=u[i]),lid[i],sampleData[:,0])
#	print i
#	print l[i]
#	print any((l[i] <= s) & (s <=u[i]))

#now iterate by loop and in each loop iterate by cache set and calculate RCD distance and frequency.
#getting loop identifier
lid = np.unique(loopData[:,0], return_counts=False)
#print lid
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
	#			print "set %d pos %d"%(k,j)
				sampleData[j,1] = INF
			prevAccess = j

#at this point we have calculated RCD
np.savetxt("inLoop", sampleData,fmt='%.0f')

#Going to train a model; we will use to anwser existance of conflict cache miss
filename = 'CCProf_Trained_model.sav'
dataset = pd.read_csv(DATA_SET_PATH)

training_features = ['contributionFactor']
target = 'IsConflict'
X = dataset[training_features].as_matrix()
y = dataset[target].as_matrix()

skf = StratifiedKFold(n_splits=8, shuffle=True)
logistic_regression_model = LogisticRegression(C=0.80,penalty='l1')

scores = cross_val_score(logistic_regression_model,X,y,cv=skf)
print("Accuracy: %0.2f (+/- %0.2f)" % (scores.mean(), scores.std() * 2))

roc_auc = cross_val_score(logistic_regression_model,X,y,cv=skf,scoring='roc_auc')
print("roc_auc: %0.2f (+/- %0.2f)" % (roc_auc.mean(), roc_auc.std() * 2))

average_precision = cross_val_score(logistic_regression_model,X,y,cv=skf,scoring='average_precision')
print("average_precision: %0.2f (+/- %0.2f)" % (average_precision.mean(), average_precision.std() * 2))

precision = cross_val_score(logistic_regression_model,X,y,cv=skf,scoring='precision')
print("precision: %0.2f (+/- %0.2f)" % (precision.mean(), precision.std() * 2))

recall = cross_val_score(logistic_regression_model,X,y,cv=skf,scoring='recall')
print("recall: %0.2f (+/- %0.2f)" % (recall.mean(), recall.std() * 2))

f1 = cross_val_score(logistic_regression_model,X,y,cv=skf,scoring='f1')
print("F1-score: %0.2f (+/- %0.2f)" % (f1.mean(), f1.std() * 2))

logistic_regression_model.fit(X,y)

A = np.ndarray(shape=(1,1), dtype=float)
#done training

#next we are going to per Loop analysis for RCD distribution
for i in range(ls[0]):
	#first, filter by loop
	samplePerLoop = np.array([row for row in sampleData if row[0] == lid[i] ])
	if len(samplePerLoop)==0:
		continue
	belowThreshold = 0
        aboveThreshold = 0
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
		#now calcluate contribution factor below threshold
		for j in range(rcdHisto.shape[0]):
			if rcdHisto[j,0] < THRESHOLD:
				belowThreshold = belowThreshold + rcdHisto[j,1]
			else:
				aboveThreshold = aboveThreshold + rcdHisto[j,1]
	
	#XXX: sanity cheking: if no data for this loop
	if (belowThreshold+aboveThreshold) == 0:
		continue
	contributionBelowThreshold = 100*(float(belowThreshold)/float(belowThreshold+aboveThreshold))
	contributionAboveThreshold = 100*(float(aboveThreshold)/float(belowThreshold+aboveThreshold))
	loopContribution = 100*(float(samplePerLoop.shape[0])/ float(sampleData.shape[0]))
	
#	print "In Loop at 0x%x, on an average %0.2f of L1 misses have a RCD below Threshold. Loops contribution to total L1 miss is %0.2f"%(l[i],contributionBelowThreshold,loopContribution)
	A[0][0] = contributionBelowThreshold
	decision = logistic_regression_model.predict(A)
	#print lid
#	print lid[i]
#	print i
	loopAddress = np.array([c for c in loopData if c[0] == lid[i]])
	#print loopAddress[0,0]
	if(decision[0]==1):
		if(loopContribution < 2):
			print "CCPROF PREDICTS >>> ***     CONFLICT MISS     *** in loop(line: %d :ip 0x%x) ***      BUT     *** loop contribution is ***    LOW    ***  %0.2f percent"%(loopAddress[0,1],loopAddress[0,2],loopContribution)
		else:
			print "CCPROF PREDICTS >>> ***     CONFLICT MISS     *** in LOOP(line: %d :ip 0x%x) ***      AND     *** loop contribution is ***    HIGH    ***  %0.2f percent"%(loopAddress[0,1],loopAddress[0,2],loopContribution)
	else:
		print "CCPROF PREDICTS >>> ***  NO CONFLICT MISS     *** in loop(line: %d :ip 0x%x). Loop's contribution to total L1 miss: %0.2f percent"%(loopAddress[0,1],loopAddress[0,2],loopContribution)
	
	#sys.stdout=open("test.txt","w")
#	print "In Loop at 0x%x, on an average %0.2f of L1 misses have a RCD below Threshold. Loops contribution to total L1 miss is %0.2f"%(l[i],contributionBelowThreshold,100*(float(samplePerLoop.shape[0])/ float(sampleData.shape[0])))
	#sys.stdout.close()
	
	#np.where(rcdHisto[],l[i],sampleData[:,0])
	#print np.asarray((uniqueRCD, counts)).T
