/*
 *  Libmonitor malloc functions.
 *
 *  Copyright (c) 2007-2013, Rice University.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *  * Neither the name of Rice University (RICE) nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  This software is provided by RICE and contributors "as is" and any
 *  express or implied warranties, including, but not limited to, the
 *  implied warranties of merchantability and fitness for a particular
 *  purpose are disclaimed. In no event shall RICE or contributors be
 *  liable for any direct, indirect, incidental, special, exemplary, or
 *  consequential damages (including, but not limited to, procurement of
 *  substitute goods or services; loss of use, data, or profits; or
 *  business interruption) however caused and on any theory of liability,
 *  whether in contract, strict liability, or tort (including negligence
 *  or otherwise) arising in any way out of the use of this software, even
 *  if advised of the possibility of such damage.
 *
 *  $Id: malloc.c 145 2015-01-15 Probir $
 *
 *  Override functions:
 *
 *    malloc
 *    free
 *
 *  Support functions:
 *
 *    monitor_real_malloc
 *    monitor_real_free
 */

#include "config.h"
#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "common.h"
#include "monitor.h"
#include <pthread.h>
/*
 *----------------------------------------------------------------------
 *  GLOBAL VARIABLES and EXTERNAL SYMBOLS
 *----------------------------------------------------------------------
 */

typedef void *malloc_fcn_t(size_t);
typedef void *calloc_fcn_t(size_t, size_t);
typedef void *realloc_fcn_t(void *, size_t);
typedef void *memalign_fcn_t(size_t, size_t);
typedef void *valloc_fcn_t(size_t);
typedef int  posix_memalign_fcn_t(void**, size_t, size_t);
typedef void free_fcn_t(void *);


#ifdef MONITOR_STATIC
extern malloc_fcn_t __real_malloc;
extern free_fcn_t __real_free;
extern calloc_fcn_t __real_calloc;
extern realloc_fcn_t __real_realloc;
extern memalign_fcn_t __real_memalign;
extern valloc_fcn_t __real_valloc;
extern posix_memalign_fcn_t __real_posix_memalign;
#endif

#define real_memalign   __libc_memalign
#define real_valloc   __libc_valloc
#define real_malloc   __libc_malloc
#define real_free     __libc_free
#define real_realloc  __libc_realloc
#define real_calloc  __libc_calloc
#define real_posix_memalign __libc_posix_memalign

extern malloc_fcn_t real_malloc;
extern free_fcn_t real_free;
extern realloc_fcn_t real_realloc;
extern memalign_fcn_t real_memalign;
extern valloc_fcn_t real_valloc;
extern calloc_fcn_t real_calloc;
static posix_memalign_fcn_t *real_posix_memalign=NULL;
//static posix_memalign_fcn_t *real_posix_memalign = NULL;

void
monitor_malloc_init(void)
{
    static int init_done = 0;

    if (init_done)
	return;

 //   MONITOR_GET_REAL_NAME_WRAP(real_posix_memalign, posix_memalign);

    init_done = 1;
}

void
monitor_posix_memalign_init(void)
{
    MONITOR_RUN_ONCE(thread_name_init);

    MONITOR_GET_REAL_NAME_WRAP(real_posix_memalign, posix_memalign);
}
void *
MONITOR_WRAP_NAME(malloc)(size_t size)
{
    monitor_pre_malloc(size);
    void *ptr = real_malloc(size);
    monitor_post_malloc(size, ptr);
    return ptr;
}

void
MONITOR_WRAP_NAME(free)(void *ptr)
{
	monitor_pre_free(ptr);
        real_free(ptr);
	monitor_post_free(ptr);
}

void *
MONITOR_WRAP_NAME(calloc)(size_t nmemb, size_t size)
{
    void *ptr;
    monitor_pre_calloc(nmemb, size);
    ptr = (*real_calloc)(nmemb, size);
    monitor_post_calloc(nmemb, size, ptr);
    return (ptr);
}

void *
MONITOR_WRAP_NAME(realloc)(void *ptr, size_t size)
{
    monitor_pre_realloc(ptr, size);

    void *nptr = real_realloc(ptr, size);
    monitor_post_realloc(ptr, size, nptr);
    return nptr;
}
void *
MONITOR_WRAP_NAME(memalign)(size_t blocksize, size_t bytes)
{
    void *ptr;
    monitor_pre_memalign(blocksize, bytes);
    ptr = (real_memalign)(blocksize, bytes);
    monitor_post_memalign(blocksize, bytes, ptr);
    return (ptr);
}

void *
MONITOR_WRAP_NAME(valloc)(size_t size)
{
    void *ptr;
    monitor_pre_valloc(size);
    ptr = (real_valloc)(size);
    monitor_post_valloc(size, ptr);
    return (ptr);
}
int
MONITOR_WRAP_NAME(posix_memalign)(void** memptr, size_t alignment, size_t size)
{
    int ret;
    monitor_posix_memalign_init();
    monitor_pre_posix_memalign(memptr, alignment, size);
    MONITOR_GET_REAL_NAME(real_posix_memalign, posix_memalign);
    ret = (real_posix_memalign)(memptr, alignment, size);
    monitor_post_posix_memalign(memptr, alignment, size, ret);
    return (ret);
}

