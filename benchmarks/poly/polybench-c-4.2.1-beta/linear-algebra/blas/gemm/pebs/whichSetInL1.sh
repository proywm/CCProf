awk < sampledAccess '{print $9}' | sort | uniq -c | sort -nr > SetsInL1
