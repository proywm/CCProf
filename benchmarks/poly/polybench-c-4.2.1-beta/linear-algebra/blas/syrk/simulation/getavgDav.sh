awk '{delta = $1 - avg; avg += delta / NR; mean2 += delta * ($1 - avg); n++;} END { printf "deviation: %f confidance interval %f\n",sqrt(mean2 / NR),1.96*(sqrt(mean2 / NR))*(sqrt(n)); }' SetsInL1
awk '{ sum += $1; n++ } END { if (n > 0) printf "avg %f count %d sum %d \n",sum / n, n, sum; }' SetsInL1
