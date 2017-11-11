#include <cstddef>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stdio.h>
#define XSIZE 64
//#define XSIZE 2048
#define YSIZE 1024
#define ITER 60000
//typedef std::size_t int;

int main()
{
	 int out_size_ = XSIZE;
	 int in_size_ = YSIZE;
	 double in[YSIZE];
	 double W[YSIZE][XSIZE];
	 double a[XSIZE];


	 srand (time(NULL));
//	 for(int k = 0; k < ITER; k++)
	 {
	 for (int i = 0; i < in_size_; ++i) {
		 for (int c = 0; c < out_size_; c++){
			W[i][c] = (double)(rand() % 10 + 1);	
		 }
		in[i] = (double)(rand() % 10 + 1);
	 }
	 for(int k = 0; k < ITER; k++)
	 for (int i = 0; i < out_size_; ++i) {
 //               a[i] = int(0);
                for (int c = 0; c < in_size_; c++) {
                    a[i] += (W[c][i] * in[c]);
                }
	 }
	 }
	 printf("a[0] %f\n",a[0]);
	return 0;
}
