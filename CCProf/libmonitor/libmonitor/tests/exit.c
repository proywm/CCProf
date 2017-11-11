/*
 *  Test process exit callbacks by having multiple threads all call
 *  process exit() or pthread_exit() at the same time.
 *
 *  There should be one call to monitor_fini_thread() for each thread,
 *  followed by exactly one call to monitor_fini_process() and no
 *  segfaults.
 *
 *  Copyright (c) 2007-2013, Rice University.
 *  See the file LICENSE for details.
 *
 *  $Id: exit.c 145 2013-01-02 20:34:01Z krentel $
 */

#include <sys/time.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROGRAM_TIME   3
#define NUM_THREADS    3
#define NUM_PTHREAD_EXIT  0

int program_time = PROGRAM_TIME;
int num_threads =  NUM_THREADS;
int num_pthread_exit = NUM_PTHREAD_EXIT;

struct timeval start;

void
wait_for_time(int secs)
{
    struct timeval now;

    for (;;) {
	gettimeofday(&now, NULL);
	if (now.tv_sec >= start.tv_sec + secs)
	    break;
	usleep(1);
    }
}

/*
 *  Threads 1..k call pthread_exit() and k+1..n call exit(),
 *  where k = num_pthread_exit.
 */
void *
my_thread(void *v)
{
    long num = (long)v;
    int is_exit = (num > num_pthread_exit);

    printf("start thread: %ld\n", num);
    wait_for_time(program_time);
    printf("end thread: %ld (%s)\n", num, is_exit ? "exit" : "pthread_exit");

    if (is_exit)
	exit(0);
    else
	pthread_exit(NULL);

    return (NULL);
}

/*
 *  Program args: num_threads, num_pthread_exit.
 */
int
main(int argc, char **argv)
{
    pthread_t td;
    long i;

    if (argc < 2 || sscanf(argv[1], "%d", &num_threads) < 1)
        num_threads = NUM_THREADS;
    if (argc < 3 || sscanf(argv[2], "%d", &num_pthread_exit) < 1)
	num_pthread_exit = NUM_PTHREAD_EXIT;
    printf("num_threads = %d, num_pthread_exit = %d\n\n",
	   num_threads, num_pthread_exit);

    gettimeofday(&start, NULL);

    for (i = 1; i <= num_threads; i++) {
        if (pthread_create(&td, NULL, my_thread, (void *)i) != 0)
            errx(1, "pthread_create failed");
    }

    wait_for_time(program_time - 1);
    printf("----------------------------------------\n");
    wait_for_time(program_time);

    printf("main exit (return)\n");
    return (0);
}
