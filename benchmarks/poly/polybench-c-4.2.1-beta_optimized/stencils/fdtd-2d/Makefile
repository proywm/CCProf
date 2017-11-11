include ../../config.mk

EXTRA_FLAGS=

fdtd-2d: fdtd-2d.c fdtd-2d.h
	${VERBOSE} ${CC} -o fdtd-2d fdtd-2d.c ${CFLAGS} -I. -I../../utilities ../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f fdtd-2d

