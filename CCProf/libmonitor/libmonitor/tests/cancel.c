/*
 *  From the main thread, pthread_create() a side thread, then
 *  immediately pthread_cancel() it and exit.  This can expose a race
 *  condition in monitor where the thread is canceled before monitor
 *  finishes initializing the thread.  If that happens, then thread
 *  shootdown can get stuck trying to access a thread that no longer
 *  exists.
 *
 *  Copyright (c) 2007-2013, Rice University.
 *  See the file LICENSE for details.
 *
 *  $Id: cancel.c 145 2013-01-02 20:34:01Z krentel $
 */

#include <sys/time.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

struct timeval start;

void *
my_thread(void *arg)
{
    struct timeval now;
    long num = (long)arg;

    printf("side: start thread: %ld\n", num);

    for (;;) {
	gettimeofday(&now, NULL);
	if (now.tv_sec >= start.tv_sec + 5)
	    break;
	usleep(1);
    }

    return NULL;
}

int
main(int argc, char **argv)
{
    pthread_t td;

    printf("main: start\n");
    gettimeofday(&start, NULL);

    if (pthread_create(&td, NULL, my_thread, (void *)42) != 0)
	errx(1, "pthread_create failed");

    if (pthread_cancel(td) != 0)
	errx(1, "pthread_cancel failed");

    _exit(0);
    return 0;
}
