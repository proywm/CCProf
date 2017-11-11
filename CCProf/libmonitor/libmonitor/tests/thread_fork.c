/*
 *  Test fork() from a threaded program.
 *
 *  Normally, the child will have only one running thread, and monitor
 *  needs to understand that, or else at process exit time, monitor
 *  will get into an infinite loop trying to signal threads that don't
 *  exist.
 *
 *  Copyright (c) 2007-2013, Rice University.
 *  See the file LICENSE for details.
 *
 *  $Id: thread_fork.c 145 2013-01-02 20:34:01Z krentel $
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
#define FORK_THREAD    0

int program_time = PROGRAM_TIME;
int num_threads = NUM_THREADS;
int fork_thread = FORK_THREAD;

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

void *
my_thread(void *v)
{
    long num = (long)v;
    int ret;

    printf("pid: %d, thread: %ld\n", getpid(), num);

    wait_for_time(2);
    if (num == fork_thread) {
	printf("\nabout to fork in thread %ld\n", num);
	ret = fork();
	if (ret < 0)
	    errx(1, "fork failed");
	else if (ret == 0)
	    printf("child pid: %d\n\n", getpid());
    }

    wait_for_time(3);
    printf("pid: %d, thread: %ld\n", getpid(), num);

    wait_for_time(4);
    exit(0);
}

/*
 *  Program args: num_threads, fork_thread.
 */
int
main(int argc, char **argv)
{
    pthread_t td;
    long i;

    if (argc < 2 || sscanf(argv[1], "%d", &num_threads) < 1)
        num_threads = NUM_THREADS;
    if (argc < 3 || sscanf(argv[2], "%d", &fork_thread) < 1)
	fork_thread = FORK_THREAD;
    printf("num_threads = %d, fork_thread = %d\n\n",
	   num_threads, fork_thread);

    gettimeofday(&start, NULL);
    wait_for_time(1);

    for (i = 1; i <= num_threads; i++) {
        if (pthread_create(&td, NULL, my_thread, (void *)i) != 0)
            errx(1, "pthread_create failed");
    }

    my_thread(0);
    return (0);
}
