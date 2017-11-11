PADD=36
NPADD=$PADD+8
sed -ie "s/PAD $PADD/PAD $NPADD/" needle.cpp
echo 
