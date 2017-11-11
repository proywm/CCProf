include ../../../config.mk

EXTRA_FLAGS=

2mm: 2mm.c 2mm.h
	${VERBOSE} ${CC} -o 2mm 2mm.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f 2mm

