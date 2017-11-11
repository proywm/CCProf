#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <monitor.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>

void
monitor_post_malloc(size_t size, void *handle)
{
	printf("came here");
}
