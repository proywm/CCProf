/*
 * Main program to link with libearly.so.
 *
 * Copyright (c) 2007-2013, Rice University.
 * See the file LICENSE for details.
 *
 * $Id: emain.c 145 2013-01-02 20:34:01Z krentel $
 */

#include <stdio.h>
#include <unistd.h>

void early_fcn(void);

int
main(int argc, char **argv)
{
    printf("==> main begin\n");
    sleep(2);
    early_fcn();
    sleep(2);
    printf("==> main end\n");

    return (0);
}
