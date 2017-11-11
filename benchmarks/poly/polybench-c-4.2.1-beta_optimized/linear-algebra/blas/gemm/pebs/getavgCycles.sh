awk '{if(lastsec == $10){delta = $11 - last; last=$11; avg += delta; n++;};lastsec=$10;} END { printf "average cycles between samples %f\n",avg/n }' sampledAccess_userspace
