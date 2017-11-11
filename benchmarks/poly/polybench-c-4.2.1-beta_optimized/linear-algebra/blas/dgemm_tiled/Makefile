include ../../../config.mk

EXTRA_FLAGS=

gemm: gemm_tiled.c gemm.h
	${VERBOSE} ${CC} -o gemm gemm_tiled.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f gemm

