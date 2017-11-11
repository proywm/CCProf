mkdir setsDist
rm setDist/set_cdf
for i in `seq 0 63`; 
do 
awk -v var="$i" '{if ($9==var) {print}}' sampledAccess_userspace > setsDist/set_$i
SUM="$(awk < setsDist/set_$i '{ sum += $1; n++ } END { if (n > 0) {print n}}')"
#echo $SUM
awk < setsDist/set_$i '{print $10}' | uniq -c > setsDist/set_dist_$i
awk < setsDist/set_dist_$i -v total="$SUM" -v var="$i" '{if(total>0) {sum += 100*($1/total);printf "%d\t%f\t%f\t%d\n", $2, 100*($1/total),sum,var}}'  >> setsDist/set_cdf
done
#awk '{if ($9=0) {print}}' sampledAccess_userspace > setsDist/set0
#awk < sampledAccess_userspace '{print $10}' | sort | uniq -c | sort -nr > SetsInL1
