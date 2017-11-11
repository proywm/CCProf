import numpy as np

fname_loop = "./loopsAll"

with open(fname_loop, 'r') as floop:
    loopData = np.genfromtxt(floop,comments="!", dtype=np.uint64, skip_header=1) #names=True)

#line id
lid = loopData[:,0]
#loop identifier
l = loopData[:,2]
#level of the tree
ll = loopData[:,1]
#get shape of loopData
ls = loopData.shape
#loop through each row of loopData and compare ll with next loop 
#fine all lines that have loop
loopList = np.array([r for r in loopData if r[2] == 1 ])
#print loopList
#print loopList[i][1]
parentLoop = -1

#adding column to loopData
loopData = np.insert(loopData,0,0,axis=1)

INNERLOOP = np.empty((0,loopData.shape[1]), int)

#print INNERLOOP.shape
#print loopList.shape
for i in range(loopList.shape[0]):
	if parentLoop != -1:
		if loopData[parentLoop][2] >= loopList[i][1]:
			loopData[parentLoop][0] = loopData[parentLoop][1]
			#np.append(INNERLOOP, loopData[parentLoop])
			print loopData[parentLoop]
			start = 0
			for j in range(loopData.shape[0]):
				if ((loopData[j][1] >= loopData[parentLoop][1]) and (loopData[j][2] > loopData[parentLoop][2])):
					start = 1
					#if ((loopData[parentLoop][0] >= loopData[j][1]) and (parentLoop != j)):
					#	break;
					loopData[j][0] = loopData[parentLoop][1]
					#np.append(INNERLOOP, loopData[j])
					#print INNERLOOP
					print loopData[j]
				else:
					if start == 1:
						break
					
	parentLoop = loopList[i][0]
#	print INNERLOOP

#print INNERLOOP
##at this point we have the inner loops and their addresses
np.save("loopsa",INNERLOOP)
