#include <cstddef>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include<stdio.h>

#define num_zones 4096
#define num_directions 12
#define num_groups 16
#define ITER 30
//typedef std::size_t int;

int main()
{
	 double volume[num_zones];
	 double dirs[num_directions];
	 double psi[num_groups][num_directions][num_zones];
	 double part = 0;
	 
	 srand (time(NULL));
	 for (int i = 0; i < num_zones; ++i) 
		volume[i] = (double)(rand() % 10 + 1);

	 for (int i = 0; i < num_directions; ++i)
                dirs[i] = (double)(rand() % 10 + 1);

	for(int g = 0;g < num_groups;++ g)
		 for(int d = 0;d < num_directions;++ d)
			for(int z = 0;z < num_zones;++ z)
				psi[g][d][z] = (double)(rand() % 10 + 1);

	 for(int k=0;k < ITER; k++)
	 for(int g = 0;g < num_groups;++ g){
	 	for(int d = 0;d < num_directions;++ d){
	 		double w = dirs[d];
			for(int z = 0;z < num_zones;++ z){
				double vol = volume[z];
          			part += w * psi[g][d][z] * vol;
				printf("%f\n",part);
        		}
      		}
    	}


	return 0;
}
