rm SAMP
awk '{ print $5}' tracedMisses > SetList
for var in {1..500}
do
    shuf SetList | head -10000 | sort | uniq -c | sort -nr >> SAMP
    #awk '{ sum += $1; n++ } END { if (n > 0) printf "%f\n",sum / n; }' SAMP >> MEAN
    
   # awk '{ n++; diff = $1-mean; sum += diff * diff * (n-1)/n; mean += diff / n;} END { if (n > 0) printf "%f\n",mean; }' SAMP 
    #awk '{delta = $1 - avg; avg += delta / NR; mean2 += delta * ($1 - avg); n++;} END { printf "%f\n",sqrt(mean2 / NR),1.96*(sqrt(mean2 / NR))*(sqrt(n)); }' SetsInL1
done

for var in {0..63}
do
   awk -v item="$var" '{if($2 == item) print;}' SAMP > temp
   awk -v item="$var" '{ n++; diff = $1-mean; sum += diff * diff * (n-1)/n; mean += diff / n;} END { if (n > 0) printf "Set %s Expected value is in the iterval %f [%f-%f] \n", item, mean, mean - 1.96 * (sqrt(mean / n))*(sqrt(n-1)),mean + 1.96 * (sqrt(mean / n))*(sqrt(n-1));}' temp 
#   awk -v item="$var" '{delta = $1 - avg; avg += delta / NR; mean2 += delta * ($1 - avg); n++;} END { print item; printf "%f conf %f\n",sqrt(mean2 / NR),1.96*(sqrt(mean2 / NR))*(sqrt(n)); }' temp
done
