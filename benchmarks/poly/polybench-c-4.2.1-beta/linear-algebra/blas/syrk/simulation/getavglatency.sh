awk '{ sum += $5; n++ } END { if (n > 0) print sum / n; print n; }' sampledAccess

