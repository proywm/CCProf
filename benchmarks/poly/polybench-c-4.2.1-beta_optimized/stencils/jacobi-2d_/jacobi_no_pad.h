#ifndef FCMB_JACOBI_H
#define FCMB_JACOBI_H

#include "common.h"

extern const struct autospec auto4d;
extern const struct autospec auto3d;
extern const struct autospec auto2d;
extern const struct autospec auto1d;

void jacobi_coefs_init();

size_t jacobi_buffer_size(const struct autospec *as);

size_t jacobi_tile_footprint(const struct autospec *as);

size_t jacobi_tile_shadow_footprint(const struct autospec *as);

size_t jacobi_space_tile_size(const struct autospec *as);

size_t jacobi2d_space_tile_size(const struct autospec *as);

size_t jacobi_time_tile_size(const struct autospec *as);

double jacobi_space_tile_additional_fraction(const struct autospec *as);

double jacobi_check(const struct autospec *as);

#define PROTO_JACOBI(pat, jd, trt, yu, xu, pre)			\
long jacobi_ ## pat ## jd ## d ## trt ## yu ## xu ## pre	\
(const struct autospec *as, struct timespec *start_ts, struct timespec *mid_ts, struct timespec *current_ts);

// Vectorized 4D Jacobi
PROTO_JACOBI(B, 4, 0, 1, 2, 0)
PROTO_JACOBI(B, 4, 0, 1, 4, 0)
PROTO_JACOBI(B, 4, 0, 2, 2, 0)
PROTO_JACOBI(B, 4, 0, 2, 4, 0)
PROTO_JACOBI(B, 4, 0, 3, 2, 0)
PROTO_JACOBI(B, 4, 0, 3, 4, 0)
PROTO_JACOBI(B, 4, 0, 4, 2, 0)
PROTO_JACOBI(B, 4, 0, 4, 4, 0)

// Vectorized 3D Jacobi
PROTO_JACOBI(B, 3, 0, 1, 2, 0)
PROTO_JACOBI(B, 3, 0, 1, 4, 0)
PROTO_JACOBI(B, 3, 0, 2, 2, 0)
PROTO_JACOBI(B, 3, 0, 2, 4, 0)
PROTO_JACOBI(B, 3, 0, 3, 2, 0)
PROTO_JACOBI(B, 3, 0, 3, 4, 0)
PROTO_JACOBI(B, 3, 0, 4, 2, 0)
PROTO_JACOBI(B, 3, 0, 4, 4, 0)

// Time register tiled
PROTO_JACOBI(B, 3, 1, 1, 2, 0)
PROTO_JACOBI(B, 3, 1, 1, 4, 0)
PROTO_JACOBI(B, 3, 1, 2, 2, 0)
PROTO_JACOBI(B, 3, 1, 2, 4, 0)
PROTO_JACOBI(B, 3, 1, 3, 2, 0)
PROTO_JACOBI(B, 3, 1, 3, 4, 0)
PROTO_JACOBI(B, 3, 1, 4, 2, 0)
PROTO_JACOBI(B, 3, 1, 4, 4, 0)

// Vectorized 2D Jacobi
PROTO_JACOBI(B, 2, 0, 1, 2, 0)
PROTO_JACOBI(B, 2, 0, 1, 4, 0)
PROTO_JACOBI(B, 2, 0, 2, 2, 0)
PROTO_JACOBI(B, 2, 0, 2, 4, 0)
PROTO_JACOBI(B, 2, 0, 3, 2, 0)
PROTO_JACOBI(B, 2, 0, 3, 4, 0)
PROTO_JACOBI(B, 2, 0, 4, 2, 0)
PROTO_JACOBI(B, 2, 0, 4, 4, 0)

// Time register tiled
PROTO_JACOBI(B, 2, 1, 1, 2, 0)
PROTO_JACOBI(B, 2, 1, 1, 4, 0)
PROTO_JACOBI(B, 2, 1, 2, 2, 0)
PROTO_JACOBI(B, 2, 1, 2, 4, 0)
PROTO_JACOBI(B, 2, 1, 3, 2, 0)
PROTO_JACOBI(B, 2, 1, 3, 4, 0)
PROTO_JACOBI(B, 2, 1, 4, 2, 0)
PROTO_JACOBI(B, 2, 1, 4, 4, 0)

// Time register tiled
PROTO_JACOBI(C, 4, 1, 1, 2, 0)
PROTO_JACOBI(C, 4, 1, 1, 4, 0)
PROTO_JACOBI(C, 4, 1, 2, 2, 0)
PROTO_JACOBI(C, 4, 1, 2, 4, 0)
PROTO_JACOBI(C, 4, 1, 3, 2, 0)
PROTO_JACOBI(C, 4, 1, 3, 4, 0)
PROTO_JACOBI(C, 4, 1, 4, 2, 0)
PROTO_JACOBI(C, 4, 1, 4, 4, 0)

// Vectorized 4D Jacobi
PROTO_JACOBI(C, 4, 0, 1, 2, 0)
PROTO_JACOBI(C, 4, 0, 1, 4, 0)
PROTO_JACOBI(C, 4, 0, 2, 2, 0)
PROTO_JACOBI(C, 4, 0, 2, 4, 0)
PROTO_JACOBI(C, 4, 0, 3, 2, 0)
PROTO_JACOBI(C, 4, 0, 3, 4, 0)
PROTO_JACOBI(C, 4, 0, 4, 2, 0)
PROTO_JACOBI(C, 4, 0, 4, 4, 0)

// Time register tiled
PROTO_JACOBI(C, 4, 1, 1, 2, 0)
PROTO_JACOBI(C, 4, 1, 1, 4, 0)
PROTO_JACOBI(C, 4, 1, 2, 2, 0)
PROTO_JACOBI(C, 4, 1, 2, 4, 0)
PROTO_JACOBI(C, 4, 1, 3, 2, 0)
PROTO_JACOBI(C, 4, 1, 3, 4, 0)
PROTO_JACOBI(C, 4, 1, 4, 2, 0)
PROTO_JACOBI(C, 4, 1, 4, 4, 0)

// Vectorized 3D Jacobi
PROTO_JACOBI(C, 3, 0, 1, 2, 0)
PROTO_JACOBI(C, 3, 0, 1, 4, 0)
PROTO_JACOBI(C, 3, 0, 2, 2, 0)
PROTO_JACOBI(C, 3, 0, 2, 4, 0)
PROTO_JACOBI(C, 3, 0, 3, 2, 0)
PROTO_JACOBI(C, 3, 0, 3, 4, 0)
PROTO_JACOBI(C, 3, 0, 4, 2, 0)
PROTO_JACOBI(C, 3, 0, 4, 4, 0)

// Time register tiled
PROTO_JACOBI(C, 3, 1, 1, 2, 0)
PROTO_JACOBI(C, 3, 1, 1, 4, 0)
PROTO_JACOBI(C, 3, 1, 2, 2, 0)
PROTO_JACOBI(C, 3, 1, 2, 4, 0)
PROTO_JACOBI(C, 3, 1, 3, 2, 0)
PROTO_JACOBI(C, 3, 1, 3, 4, 0)
PROTO_JACOBI(C, 3, 1, 4, 2, 0)
PROTO_JACOBI(C, 3, 1, 4, 4, 0)

// Vectorized 2D Jacobi
PROTO_JACOBI(C, 2, 0, 1, 2, 0)
PROTO_JACOBI(C, 2, 0, 1, 4, 0)
PROTO_JACOBI(C, 2, 0, 2, 2, 0)
PROTO_JACOBI(C, 2, 0, 2, 4, 0)
PROTO_JACOBI(C, 2, 0, 3, 2, 0)
PROTO_JACOBI(C, 2, 0, 3, 4, 0)
PROTO_JACOBI(C, 2, 0, 4, 2, 0)
PROTO_JACOBI(C, 2, 0, 4, 4, 0)

// Time register tiled
PROTO_JACOBI(C, 2, 1, 1, 2, 0)
PROTO_JACOBI(C, 2, 1, 1, 4, 0)
PROTO_JACOBI(C, 2, 1, 2, 2, 0)
PROTO_JACOBI(C, 2, 1, 2, 4, 0)
PROTO_JACOBI(C, 2, 1, 3, 2, 0)
PROTO_JACOBI(C, 2, 1, 3, 4, 0)
PROTO_JACOBI(C, 2, 1, 4, 2, 0)
PROTO_JACOBI(C, 2, 1, 4, 4, 0)

// Vectorized 1D Jacobi
PROTO_JACOBI(, 1, 0, 1, 2, 0)
PROTO_JACOBI(, 1, 0, 1, 4, 0)
PROTO_JACOBI(, 1, 0, 2, 2, 0)
PROTO_JACOBI(, 1, 0, 2, 4, 0)
PROTO_JACOBI(, 1, 0, 3, 2, 0)
PROTO_JACOBI(, 1, 0, 3, 4, 0)
PROTO_JACOBI(, 1, 0, 4, 2, 0)
PROTO_JACOBI(, 1, 0, 4, 4, 0)

// Time register tiled
PROTO_JACOBI(, 1, 1, 1, 2, 0)
PROTO_JACOBI(, 1, 1, 1, 4, 0)
PROTO_JACOBI(, 1, 1, 2, 2, 0)
PROTO_JACOBI(, 1, 1, 2, 4, 0)
PROTO_JACOBI(, 1, 1, 3, 2, 0)
PROTO_JACOBI(, 1, 1, 3, 4, 0)
PROTO_JACOBI(, 1, 1, 4, 2, 0)
PROTO_JACOBI(, 1, 1, 4, 4, 0)
  
#endif

