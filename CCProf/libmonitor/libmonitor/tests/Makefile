#
#  Example test programs for libmonitor.
#
#  These tests are not integrated with autotools.  Just compile the
#  programs, run them with the monitor-run script in debug mode, and
#  observe the output.
#
#  Copyright (c) 2007-2013, Rice University.
#  See the file LICENSE for details.
#
#  $Id: Makefile 145 2013-01-02 20:34:01Z krentel $
#

CC = gcc
CFLAGS = -g -O -Wall

THREAD_PROGRAMS = cancel exit shootdown thread_fork
NONTHREAD_PROGRAMS = emain

PROGRAMS = $(THREAD_PROGRAMS) $(NONTHREAD_PROGRAMS)

.PHONY: all clean

all: $(PROGRAMS)

$(THREAD_PROGRAMS): %: %.c
	$(CC) -o $@ $(CFLAGS) $< -lpthread

emain: emain.c libearly.so
	$(CC) -o $@ $(CFLAGS) $< -L. -learly

libearly.so: early.c
	$(CC) -o $@ $(CFLAGS) -shared -fPIC $< -ldl -lpthread

clean:
	rm -f *.o *.so a.out $(PROGRAMS)

