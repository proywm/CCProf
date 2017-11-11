include ../../config.mk

EXTRA_FLAGS=

jacobi-2d: jacobi-2d.c jacobi-2d.h
	${VERBOSE} ${CC} -o jacobi-2d jacobi-2d.c ${CFLAGS} -I. -I../../utilities ../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f jacobi-2d

