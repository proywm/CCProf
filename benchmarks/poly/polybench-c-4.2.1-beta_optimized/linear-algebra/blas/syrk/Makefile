include ../../../config.mk

EXTRA_FLAGS=

syrk: syrk.c syrk.h
	${VERBOSE} ${CC} -o syrk syrk.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f syrk

