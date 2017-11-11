PADD=0
for var in {1..512}
do
	NPADD=$PADD+4
	sed -ie "s/PAD $PADD/PAD $NPADD/g" needle.cpp
#	PADD=$PADD+8
	grep -Ri -C 0 "PAD 0" needle.cpp
	make clean
	make
	grep -Ri -C 0 "PAD 0" needle.cpp &>> runtime
	time ./run &>> runtime
done
