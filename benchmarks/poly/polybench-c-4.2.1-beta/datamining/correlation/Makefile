include ../../config.mk

EXTRA_FLAGS=-lm

correlation: correlation.c correlation.h
	${VERBOSE} ${CC} -o correlation correlation.c ${CFLAGS} -I. -I../../utilities ../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f correlation

