#include "common.h"
#include "fcmb_jacobi.h"
#include "fcmb.h"
#include "md.h"

#include <stdio.h>
#include <stdlib.h>

static double scoef[MAXDIMENSION][MAXCOEFS];
static VTYPE vcoef[MAXDIMENSION][MAXCOEFS];

void jacobi_coefs_init()
{
//  clock_gettime(0, &ts_midddd);
  for (int d = 0; d < MAXDIMENSION; d++) {
    double c = 1.0 / MAXCOEFS;
    for (int i = 0; i < MAXCOEFS; i++) {
      scoef[d][i] = c;
      vcoef[d][i] = VSET1(c);
      c += 1.0 / 65536;
    }
  }
}

static
size_t jacobi_ops(const struct autospec *as)
{
  int te = 1;
  for (int i = MAXDIMENSION - as->dimension; i < MAXDIMENSION; i++)
    te *= as->core[i] + as->scale[i];
//	fprintf(stderr,"%d", te);
  return te;
}

static
size_t jacobi_shadow_size(const struct autospec *as)
{
  size_t size = sizeof(double);
  for (int i = MAXDIMENSION - as->dimension; i < MAXDIMENSION; i++)
    size *= (1 << (as->core[i] + as->scale[i])) + 2; // should be Jacobi order
  return size;
}

static
size_t jacobi_shadow_offset(const struct autospec *as)
{
  long offset = 2;
  for (int i = MAXDIMENSION - as->dimension + 1; i < MAXDIMENSION; i++)
    offset *= (as->core[i] + as->scale[i]);
  return offset;
}

size_t jacobi_buffer_size(const struct autospec *as)
{
  size_t size = sizeof(double);
  for (int i = MAXDIMENSION - as->dimension; i < MAXDIMENSION; i++)
    size *= (1 << (as->core[i] + as->scale[i]))
      + MAX((i == MAXDIMENSION - 1 ? VF : 2), as->pad[i]);
  return size + 2 * jacobi_shadow_offset(as) * sizeof(double);
}

// Tile memory footprint for the two buffers
size_t jacobi_tile_footprint(const struct autospec *as)
{
  return 2 * sizeof(double) * jacobi_ops(as);
}

// Tile memory footprint with shadow for the two buffers
size_t jacobi_tile_shadow_footprint(const struct autospec *as)
{
  return 2 * jacobi_shadow_size(as);
}

// Tile size for non-iterated spatial and temporal locality, Jacobi 3D and more
size_t jacobi_space_tile_size(const struct autospec *as)
{
  int d = MAXDIMENSION - 1;
  size_t size = (1 << (as->core[d] + as->scale[d])) * sizeof(double)
    + md.l1_line_size;
  /* Stripe of 3 hyperplanes with shadow, 2 being simultaneously live,
     plus the hyperplane of stored values (wastefully consuming cache lines)
     hence a space tile's footprint of 3 hyperplanes. */
  for (int i = MAXDIMENSION - as->dimension + 1; i < d; i++)
    size *= (1 << (as->core[i] + as->scale[i])) + 2;
  return 3 * size;
}

// Tile size for non-iterated spatial and temporal locality, Jacobi 2D
size_t jacobi2d_space_tile_size(const struct autospec *as)
{
  /* Column-major stripe of simultaneously live values,
     plus the column of stored values (wastefully consuming cache lines)
     hence a space tile's footprint of one
     ((xu * VF + 2) * sizeof(double))-wide column for reused values,
     counting for 2 or 3 lines depending on xu, plus a
     (xu * VF * sizeof(double))-wide column for stored values,
     counting for 2 lines (for misaligned accesses). */
  int d = MAXDIMENSION - as->dimension;
  return 5 * md.l1_line_size * (1 << (as->core[d] + as->scale[d]));
}

// Tile size for time-iterated spatial and temporal locality
size_t jacobi_time_tile_size(const struct autospec *as)
{
  int d = MAXDIMENSION - 1;
  size_t size = (1 << (as->core[d] + as->scale[d])) * sizeof(double)
    + md.l1_line_size;
  /* Full tile with shadow. */
  for (int i = MAXDIMENSION - as->dimension; i < d; i++)
    size *= (1 << (as->core[i] + as->scale[i])) + 2;
  return size;
}

// Cost of additional loads when starting a new space tile
double jacobi_space_tile_additional_fraction(const struct autospec *as)
{
  int d = MAXDIMENSION - 2;
  return 1.0 / (1 << (as->core[d] + as->scale[d]));
}

double jacobi_check(const struct autospec *as)
{
  long nt = (as->core[0] + as->scale[0]);
  long nz = (as->core[1] + as->scale[1]);
  long ny = (as->core[2] + as->scale[2]);
  long nx = (as->core[3] + as->scale[3]);
  
  double (*restrict p)[nz+as->pad[1]][ny+as->pad[2]][nx+as->pad[3]] =
    as->iter == 0 ?
    (void*)(buf_b + jacobi_shadow_offset(as) + align) :
    (void*)(buf_a + jacobi_shadow_offset(as));
  
  VTYPE vcheck = VSETZERO();
  VTYPE v1, v2, v3;
  double dummy[VF];

  for (long t = 0; t < nt; t++) {
    for (long z = 0; z < nz; z++) {
      for (long y = 0; y < ny; y++) {
	for (long x = 0; x < nx - 4 * VF; x += 4 * VF) {
	  v1 = VLOADU(&p[t][z][y][x]);
	  v2 = VLOADU(&p[t][z][y][x+VF]);
	  v3 = VADD(v1, v2);
	  v1 = VLOADU(&p[t][z][y][x+2*VF]);
	  v2 = VLOADU(&p[t][z][y][x+3*VF]);
	  v1 = VADD(v1, v2);
	  v1 = VADD(v1, v3);
	  vcheck = VADD(vcheck, v1);
	}
      }
    }
  }

  VSTOREU(dummy, vcheck);
  return (dummy[0] + dummy[1] + dummy[2] + dummy[3]) / jacobi_ops(as);
}

#define ROTATEab(v, oz, ox)					\
  v ## a ## oz ## ox = v ## b ## oz ## ox;			\
  v ## b ## oz ## ox = v ## c ## oz ## ox;

#define ROTATEac(v, oz, ox)					\
  v ## b ## oz ## ox = v ## a ## oz ## ox;			\
  v ## a ## oz ## ox = v ## c ## oz ## ox;

#define ROTATEabx(v, oz)			                \
  ROTATEab(v, oz, 0);						\
  ROTATEab(v, oz, 1);						\
  ROTATEab(v, oz, 2);

#define ROTATEabzx(v)						\
  ROTATEabx(v, 0);						\
  ROTATEabx(v, 1);						\
  ROTATEabx(v, 2);

#define ROTATEacx(v, oz)			                \
  ROTATEac(v, oz, 0);						\
  ROTATEac(v, oz, 1);						\
  ROTATEac(v, oz, 2);

#define ROTATEaczx(v)						\
  ROTATEacx(v, 0);						\
  ROTATEacx(v, 1);						\
  ROTATEacx(v, 2);

#define ROTATEab_XU(i, xu)					\
  ROTATEabzx(r0 ## i);						\
  ROTATEabzx(r1 ## i);						\
  if (xu >= 4) {						\
    ROTATEabzx(r2 ## i);					\
    ROTATEabzx(r3 ## i);					\
  }
#define ROTATEac_XU(i, xu)					\
  ROTATEaczx(r0 ## i);						\
  ROTATEaczx(r1 ## i);						\
  if (xu >= 4) {						\
    ROTATEaczx(r2 ## i);					\
    ROTATEaczx(r3 ## i);					\
  }

#define J4D_PREFETCH(p, t, z, y, x) PREFETCHT2(&(p)[t][z][y][x])
#define J3D_PREFETCH(p, t, z, y, x) PREFETCHT2(&(p)[z][y][x])
#define J2D_PREFETCH(p, t, z, y, x) PREFETCHT2(&(p)[y][x])
#define J1D_PREFETCH(p, t, z, y, x) PREFETCHT2(&(p)[x])
#ifdef _AVX_
#  define J_XU_PREFETCH(jd, p, yo, xo, xu)		        \
  J ## jd ## D_PREFETCH(p, t, z, y+yo, x+xo);			\
  if (xu >= 4) {						\
    J ## jd ## D_PREFETCH(p, t, z, y+yo, x+xo+2*VF);		\
  }
#else
#  define J_XU_PREFETCH(jd, p, yo, xo, xu)	                \
  J ## jd ## D_PREFETCH(p, t, z, y+yo, x+xo);
#endif
#define J_PREFETCH(jd, p, yu, xu)				\
  J_XU_PREFETCH(jd, p, 0, mx, xu);				\
  if (yu >= 2) {						\
    J_XU_PREFETCH(jd, p, 1, mx, xu);				\
  }								\
  if (yu >= 3) {						\
    J_XU_PREFETCH(jd, p, 2, mx, xu);				\
  }								\
  if (yu >= 4) {						\
    J_XU_PREFETCH(jd, p, 3, mx, xu);				\
  }

#ifdef DEBUG
#  define J4D_VLOAD(p, t, z, y, x)		                \
  (x < 0 || y < 0 || z < 0 || t < 0 ||				\
   x >= nx || y >= ny || z >= nz || t >= nt) ?			\
  fprintf(stderr, "J4D_VLOAD %ld %ld %ld %ld\n", t, z, y, x),	\
    VLOADU(&(p)[0][0][0][0]) :					\
    VLOADU(&(p)[t][z][y][x])
#  define J3D_VLOAD(p, t, z, y, x)		                \
  (x < 0 || y < 0 || z < 0 ||					\
   x >= nx || y >= ny || z >= nz) ?				\
  fprintf(stderr, "J3D_VLOAD %ld %ld %ld\n", z, y, x),		\
    VLOADU(&(p)[0][0][0]) :					\
    VLOADU(&(p)[z][y][x])
#  define J2D_VLOAD(p, t, z, y, x)		                \
  (x < 0 || y < 0 ||						\
   x >= nx || y >= ny) ?					\
  fprintf(stderr, "J2D_VLOAD %ld %ld\n", y, x),			\
    VLOADU(&(p)[0][0]) :					\
    VLOADU(&(p)[y][x])
#  define J1D_VLOAD(p, t, z, y, x)		                \
  (x < 0 ||							\
   x >= nx) ?							\
  fprintf(stderr, "J1D_VLOAD %ld\n", x),			\
    VLOADU(&(p)[0]) :						\
    VLOADU(&(p)[x])
#  define J4D_VSTORE(p, t, z, y, x, v)				\
  (x < 0 || y < 0 || z < 0 || t < 0 ||				\
   x >= nx || y >= ny || z >= nz || t >= nt) ?			\
  fprintf(stderr, "J4D_VSTORE %ld %ld %ld %ld\n", t, z, y, x) :	\
  VSTOREU(&(p)[t][z][y][x], v)
#  define J3D_VSTORE(p, t, z, y, x, v)				\
  (x < 0 || y < 0 || z < 0 ||					\
   x >= nx || y >= ny || z >= nz) ?				\
  fprintf(stderr, "J3D_VSTORE %ld %ld %ld\n", z, y, x) :	\
  VSTOREU(&(p)[z][y][x], v)
#  define J2D_VSTORE(p, t, z, y, x, v)				\
  (x < 0 || y < 0 ||						\
   x >= nx || y >= ny) ?					\
  fprintf(stderr, "J2D_VSTORE %ld %ld\n", y, x) :		\
  VSTOREU(&(p)[y][x], v)
#  define J1D_VSTORE(p, t, z, y, x, v)				\
  (x < 0 ||							\
   x >= nx) ?							\
  fprintf(stderr, "J1D_VSTORE %ld\n", x) :			\
  VSTOREU(&(p)[x], v)
#else
#  define J4D_VLOAD(p, t, z, y, x) VLOADU(&(p)[t][z][y][x])
#  define J3D_VLOAD(p, t, z, y, x) VLOADU(&(p)[z][y][x])
#  define J2D_VLOAD(p, t, z, y, x) VLOADU(&(p)[y][x])
#  define J1D_VLOAD(p, t, z, y, x) VLOADU(&(p)[x])
#  define J4D_VSTORE(p, t, z, y, x, v) VSTOREU(&(p)[t][z][y][x], v)
#  define J3D_VSTORE(p, t, z, y, x, v) VSTOREU(&(p)[z][y][x], v)
#  define J2D_VSTORE(p, t, z, y, x, v) VSTOREU(&(p)[y][x], v)
#  define J1D_VSTORE(p, t, z, y, x, v) VSTOREU(&(p)[x], v)
#endif

#ifdef IDENTICAL
#  define J_COEF(jd, t, z, y, x)				\
  VTYPE c ## t ## z ## y ## x = vcoef[jd-1][0];			\
  double s ## t ## z ## y ## x = vcoef[jd-1][0][0];
#else
#  define J_COEF(jd, t, z, y, x)				\
  VTYPE c ## t ## z ## y ## x = vcoef[jd-1][27*t+9*z+3*y+x];	\
  double s ## t ## z ## y ## x = vcoef[jd-1][27*t+9*z+3*y+x][0];
#endif

#define J_COEFx(jd, t, z, y)			                \
  J_COEF(jd, t, z, y, 0);					\
  J_COEF(jd, t, z, y, 1);					\
  J_COEF(jd, t, z, y, 2);

#define J_COEFyx(jd, t, z)					\
  J_COEFx(jd, t, z, 0);						\
  J_COEFx(jd, t, z, 1);						\
  J_COEFx(jd, t, z, 2);

#define J_COEFzyx(jd, t)					\
  J_COEFyx(jd, t, 0);						\
  J_COEFyx(jd, t, 1);						\
  J_COEFyx(jd, t, 2);

#define J_COEFtzyx(jd)						\
  J_COEFzyx(jd, 0);						\
  J_COEFzyx(jd, 1);						\
  J_COEFzyx(jd, 2);

#define J_SIZES(jd)						\
  long nt = (as->core[0] + auto ## jd ## d.scale[0]);	\
  long nz = (as->core[1] + auto ## jd ## d.scale[1]);	\
  long ny = (as->core[2] + auto ## jd ## d.scale[2]);	\
  long nx = (as->core[3] + auto ## jd ## d.scale[3]);	\
  long mt = (as->core[0] + as->scale[0]);			\
  long mz = (as->core[1] + as->scale[1]);			\
  long my = (as->core[2] + as->scale[2]);			\
  long mx = (as->core[3] + as->scale[3]);			\
  J_ARRAY_ ## jd ## D(p) = (void*)(buf_a + jacobi_shadow_offset(as));\
  J_ARRAY_ ## jd ## D(sp) = (void*)(buf_b + jacobi_shadow_offset(as) + align);

#define J_ARRAY_4D(p)						\
  double (*restrict p)[nz+as->pad[1]][ny+as->pad[2]][nx+as->pad[3]]
#define J_ARRAY_3D(p)					        \
  double (*restrict p)[ny+as->pad[2]][nx+as->pad[3]]
#define J_ARRAY_2D(p)				                \
  double (*restrict p)[nx+as->pad[3]]
#define J_ARRAY_1D(p)				                \
  double *restrict p

#define ROTATING_REGISTERS(ux, i, oz, ox)			\
  VTYPE r ## ux ## i ## a ## oz ## ox;				\
  VTYPE r ## ux ## i ## b ## oz ## ox;				\
  VTYPE r ## ux ## i ## c ## oz ## ox;

#define ROTATING_REGISTERSx(ux, i, oz)				\
  ROTATING_REGISTERS(ux, i, oz, 0);				\
  ROTATING_REGISTERS(ux, i, oz, 1);				\
  ROTATING_REGISTERS(ux, i, oz, 2);

#define ROTATING_REGISTERSzx(ux, i)				\
  ROTATING_REGISTERSx(ux, i, 0);				\
  ROTATING_REGISTERSx(ux, i, 1);				\
  ROTATING_REGISTERSx(ux, i, 2);
  
#define ROTATING_REGISTERS_XU(i)				\
  ROTATING_REGISTERSzx(0, i);					\
  ROTATING_REGISTERSzx(1, i);					\
  ROTATING_REGISTERSzx(2, i);					\
  ROTATING_REGISTERSzx(3, i);

#define TIME_REUSE_REGISTERS(ux)				\
  VTYPE rt ## ux, rz ## ux, ry ## ux, rx ## ux;			\

#define TIME_REUSE_REGISTERS_XU					\
  TIME_REUSE_REGISTERS(0);					\
  TIME_REUSE_REGISTERS(1);					\
  TIME_REUSE_REGISTERS(2);					\
  TIME_REUSE_REGISTERS(3);

#define REGISTERS						\
  VTYPE va, vo;							\
  ROTATING_REGISTERS_XU(0);					\
  ROTATING_REGISTERS_XU(1);					\
  TIME_REUSE_REGISTERS_XU;

// Template for the generation of all Jacobi functions
#define DEF_JACOBI(pat, jd, trt, yu, xu, pre)			\
long jacobi_ ## pat ## jd ## d ## trt ## yu ## xu ## pre	\
(const struct autospec *as, struct timespec *start_ts, struct timespec *mid_ts, struct timespec *current_ts)		\
{								\
  DEBUG_INSTRUMENT(long stencil = 0);				\
  								\
  long m = (as->core[4-jd] + auto ## jd ## d.scale[4-jd]);	\
  long nd = (as->core[4-jd] + as->scale[4-jd]);		\
 /* long np = MAX(nd * 4, m / md.cores);*/			\
  long np = m / MAX (md.cores, omp_get_num_threads()); \
  /* At least 4 tiles per trapezoid */				\
  /* Exactly 1 face-down trapezoid per core */			\
								\
  /* Slope */							\
  long l = 1;							\
								\
  long nit = 1 << as->iter;					\
  long ns = nit;						\
  /* Look for the largest power of 2 <= (np - nd) / 2. */	\
  clock_gettime(0, start_ts); \
/*  while (ns < nit && 2 * ns * l <= np - nd)*/			\
/*    ns *= 2; */							\
  long mz0;								\
  long par;							\
								\
  if (ns < nit) {						\
    fprintf(stderr, "Warning: iterating in stripes of %ld instead of %ld\n",\
	    ns, nit);						\
  }								\
								\
  if (trt && ns == 1) {						\
    fcmb_abort_alarm();						\
    return 0;							\
  }								\
								\
  J_COEFtzyx(jd);						\
  J_SIZES(jd);							\
/*  fprintf(stderr, "(%ld %ld)", ns, np);*/					\
   for (long nn = 0; nn < nit; nn += ns) {			\
    mz0 = mz; \
    /* Face-down trapezoids */					\
    OMP_FOR							\
      for(par = 1; par < m - 1; par += np) {			\
	const long sl = l;					\
      \
	J ## jd ## D_MAP(pat, jd, trt, par, par + np, yu, xu, pre);\
/*	fprintf(stderr, "ok");*/ \
      }								\
    clock_gettime(0, mid_ts);					\
    mz0 = ns*2; \
    /* Face-up triangles */					\
    OMP_FOR							\
      for(par = 1; par < m + np - 2; par += np) {		\
	const long sl = -l;					\
	J ## jd ## D_TRI(pat, jd, par, yu, xu, pre);		\
      }								\
  }								\
								\
  DEBUG_INSTRUMENT(fprintf(stderr, "OPS = %ld\n", stencil));	\
  clock_gettime(0, current_ts); \
  return jacobi_ops(&auto ## jd ## d) << as->iter;		\
}

#define J4D_MAP(pat, jd, trt, tts, tte, yu, xu, pre)		\
  for (long tt = tts; tt < tte; tt += mt) {			\
    J3D_MAP(pat, jd, trt, 1, nz - 1, yu, xu, pre);		\
  }

#define J4D_TRI(pat, jd, tts, yu, xu, pre)			\
  {								\
    long tt = tts;						\
    long np = 0;						\
    J3D_MAP(pat, jd, 0, 1, nz - 1, yu, xu, pre);		\
  }

#define J3D_MAP(pat, jd, trt, zzs, zze, yu, xu, pre)		\
  for (long zz = zzs; zz < zze+ns; zz += mz) {			\
    J2D_MAP(pat, jd, trt, 1, ny - 1, yu, xu, pre);		\
  } 

#define J3D_TRI(pat, jd, zzs, yu, xu, pre)			\
  {								\
    long zz = zzs;						\
    long np = 0;						\
    J2D_MAP(pat, jd, 0, 1, ny - 1, yu, xu, pre);		\
  } 

#define J2D_MAP(pat, jd, trt, yys, yye, yu, xu, pre)		\
  for (long yy = yys; yy < yye+ns; yy += my) {			\
    J1D_MAP(pat, jd, trt, 1, nx - 1, yu, xu, pre);		\
  } 


#define J2D_TRI(pat, jd, yys, yu, xu, pre)			\
  {								\
    long yy = yys;						\
    long np = 0;						\
    J1D_MAP(pat, jd, 0, 1, nx - 1, yu, xu, pre);		\
  }

#define J1D_MAP(pat, jd, trt, xxs, xxe, yu, xu, pre)		\
  for (long xx = xxs; xx < xxe+ns; xx += mx) {			\
    REGISTERS;							\
								\
    long sh = 0;						\
								\
   for (long n = 0; n < ns; n += trt + 1) {			\
   								\
      J ## jd ## D_CORE_MAP(pat, jd, trt, J ## pat ## jd ## D_SCALAR,\
			    MAX(par + sh, 1),			\
			    MIN(par +np - sh, m - 1),		\
			    yu, xu, pre);			\
								\
      if (trt) {						\
	sh += 2 * sl;						\
      } else {							\
	sh += sl;						\
	SWAP(p, sp);						\
      }								\
    } 								\
  }

#define J1D_TRI(pat, jd, xxs, yu, xu, pre)			\
  {								\
    long xx = xxs;						\
    long np = 0;						\
    J1D_MAP(pat, jd, 0, xxs, xxs + 1, yu, xu, pre);		\
  }

#define J4D_CORE_MAP(pat, jd, trt, scal, tts, tte, yu, xu, pre)	\
  long ts = MAX(tt - n * l, tts);				\
  long te = MIN(tt + mt - n * l, tte);				\
  long t;							\
  								\
  for (t = ts; t < te; t++) {					\
    J3D_CORE_MAP(pat, jd, trt, scal, 1, nz - 1, yu, xu, pre);	\
  }

#define J3D_CORE_MAP(pat, jd, trt, scal, zzs, zze, yu, xu, pre)	\
  long zs = MAX(zz - n * l, zzs);				\
  long ze = MIN(zz + mz0 - n * l, zze);				\
  long z;							\
  								\
  for (z = ze-1; z >= zs; z--) {					\
  /* fprintf(stderr, "(%ld)", z);*/\
    J2D_CORE_MAP(pat, jd, trt, scal, 1, ny - 1, yu, xu, pre);	\
  }

// yu == 1 disables register tiling along y
// yu == 2, 4 enables register tiling with rotating registers and moves
// yu == 3 enables register tiling with moveless rotating registers
#define J2D_CORE_MAP(pat, jd, trt, scal, yys, yye, yu, xu, pre)	\
  long xs = MAX(xx - n * l, 1);					\
  long xe = MIN(xx + mx - n * l, nx - 1);			\
  long x;							\
/* fprintf(stderr, "(%ld %ld %ld)", xs, xe, xu*VF);*/		\
  for (x = xs; x < xe - xu * VF + 1; x += xu * VF) {		\
   \
    long ys = MAX(yy - n * l, yys);				\
    long ye = MIN(yy + my - n * l, yye);			\
    long y;							\
   						 		\
    for (y = ys; y < ye - yu + 1; y += yu) {			\
      \
/*	fprintf(stderr, "(%ld %ld)", y, x);*/\
      if (yu == 1 || y == ys) {					\
	J_XU_ROTATE_VLOAD(pat, jd, p, 0, t, z, y, x, xu);	\
      }								\
      if (pre) {						\
	J_PREFETCH(jd, p, yu, xu);				\
      }								\
/*	fprintf(stderr, "phase1");*/				\
      J_TPL(pat, jd, p, sp, 0, t, z, y, x, yu, xu);		\
/*	fprintf(stderr, "phase2");*/				\
      /* FIXME: missing bounds check on (t,z,y,x)-l */		\
      if (trt) {						\
	if (yu == 1 || y == ys) {				\
	  J_XU_ROTATE_VLOAD(pat, jd, sp, 1, t-l, z-l, y-l, x-l, xu);\
	}							\
	if (pre) {						\
	  J_PREFETCH(jd, sp, yu, xu);				\
	}							\
	J_TPL(pat, jd, sp, p, 1, t-l, z-l, y-l, x-l, yu, xu);	\
      }								\
    }								\
    								\
    for (; y < ye; y++) {					\
      J_XU_ROTATE_VLOAD(pat, jd, p, 0, t, z, y, x, xu);		\
      J_XU(pat, jd, p, sp, 0, a, b, c, t, z, y, x, xu);		\
								\
      if (trt) {						\
	J_XU_ROTATE_VLOAD(pat, jd, sp, 1, t-l, z-l, y-l, x-l, xu);\
	J_XU(pat, jd, sp, p, 1, a, b, c, t-l, z-l, y-l, x-l, xu);\
      }								\
    }								\
  }								\
    								\
  for (; x < xe - VF + 1; x += VF) {				\
    long ys = MAX(yy - n * l, yys);				\
    long ye = MIN(yy + my - n * l, yye);			\
    long y = ys;						\
								\
    for (y = ys; y < ye; y++) {					\
      J_ROTATE_VLOAD(pat, jd, p, r0, 0, t, z, y, x);		\
      J ## pat ## jd ## D0(p, sp, rt0, rz0, ry0, rx0,		\
			   r00a, r00b, r00c, t, z, y, x);	\
      DEBUG_INSTRUMENT(stencil += VF);				\
								\
      if (trt) {						\
	J_ROTATE_VLOAD(pat, jd, sp, r0, 1, t-l, z-l, y-l, x-l);	\
	J ## pat ## jd ## D1(sp, p, rt0, rz0, ry0, rx0,		\
			     r01a, r01b, r01c, t-l, z-l, y-l, x-l);\
	DEBUG_INSTRUMENT(stencil += VF);		        \
      }								\
    }								\
  }								\
								\
  long xp = x;							\
  long ys = MAX(yy - n * l, yys);				\
  long ye = MIN(yy + my - n * l, yye);				\
  long y = ys;							\
  for (y = ys; y < ye; y++) {					\
    for (x = xp; x < xe; x++) {					\
      scal(p, sp);						\
								\
      if (trt) {						\
	scal(sp, p);						\
      }								\
    }								\
  }

#define J1D_CORE_MAP(pat, jd, trt, scal, xxs, xxe, yu, xu, pre)	\
  long xs = MAX(xx - n * l, xxs);				\
  long xe = MIN(xx + mx - n * l, xxe);				\
  long x;							\
								\
  for (x = xs; x < xe - xu * VF + 1; x += xu * VF) {		\
    if (pre) {							\
      J_XU_PREFETCH(1, p, , mx, xu);				\
    }								\
    J_XU(pat, 1, p, sp, 0, , , , , , , x, xu);			\
								\
    /* FIXME: missing bounds check on x-l */			\
    if (trt) {							\
      if (pre) {						\
	J_XU_PREFETCH(1, sp, , mx, xu);				\
      }								\
      J_XU(pat, 1, sp, p, 1, , , , , , , x-l, xu);		\
    }								\
  }								\
								\
  for (; x < xe - VF + 1; x += VF) {				\
    J ## pat ## 1D0(p, sp, rt0, rz0, ry0, rx0, , , , , , , x);	\
    DEBUG_INSTRUMENT(stencil += VF);				\
								\
    if (trt) {							\
      J ## pat ## 1D1(sp, p, rt0, rz0, ry0, rx0, , , , , , , x-l);\
      DEBUG_INSTRUMENT(stencil += VF);				\
    }								\
  }								\
								\
  for (; x < xe; x++) {						\
    scal(p, sp);						\
    DEBUG_INSTRUMENT(stencil++);				\
								\
    if (trt) {							\
      scal(sp, p);						\
      DEBUG_INSTRUMENT(stencil++);				\
    }								\
  }

// Template for register rotation along y, Jacobi 2D and above
#define J_TPL(pat, jd, p, sp, i, t, z, y, x, yu, xu)		\
  J_XU(pat, jd, p, sp, i, a, b, c, t, z, y, x, xu);		\
  if (yu >= 2) {						\
    J_XU(pat, jd, p, sp, i, c, a, b, t, z, y+1, x, xu);		\
    if (yu == 2) {						\
      ROTATEab_XU(i, xu);					\
    }								\
  }								\
  if (yu >= 3) {						\
    J_XU(pat, jd, p, sp, i, b, c, a, t, z, y+2, x, xu);		\
  }								\
  if (yu >= 4) {						\
    J_XU(pat, jd, p, sp, i, a, b, c, t, z, y+3, x, xu);		\
    ROTATEac_XU(i, xu);						\
  }

// Preload rotating registers
#define J_ROTATE_VLOAD(pat, jd, p, r, i, t, z, y, x)		\
  r ## i ## a00 = J ## jd ## D_VLOAD(p, t, z-1, y, x-1);	\
  r ## i ## b00 = J ## jd ## D_VLOAD(p, t, z-1, y-1, x-1);	\
  r ## i ## a01 = J ## jd ## D_VLOAD(p, t, z-1, y, x);		\
  r ## i ## b01 = J ## jd ## D_VLOAD(p, t, z-1, y-1, x);	\
  r ## i ## a02 = J ## jd ## D_VLOAD(p, t, z-1, y, x+1);	\
  r ## i ## b02 = J ## jd ## D_VLOAD(p, t, z-1, y-1, x+1);	\
  r ## i ## a10 = J ## jd ## D_VLOAD(p, t, z, y, x-1);		\
  r ## i ## b10 = J ## jd ## D_VLOAD(p, t, z, y-1, x-1);	\
  r ## i ## a11 = J ## jd ## D_VLOAD(p, t, z, y, x);		\
  r ## i ## b11 = J ## jd ## D_VLOAD(p, t, z, y-1, x);		\
  r ## i ## a12 = J ## jd ## D_VLOAD(p, t, z, y, x+1);		\
  r ## i ## b12 = J ## jd ## D_VLOAD(p, t, z, y-1, x+1);	\
  r ## i ## a20 = J ## jd ## D_VLOAD(p, t, z+1, y, x-1);	\
  r ## i ## b20 = J ## jd ## D_VLOAD(p, t, z+1, y-1, x-1);	\
  r ## i ## a21 = J ## jd ## D_VLOAD(p, t, z+1, y, x);		\
  r ## i ## b21 = J ## jd ## D_VLOAD(p, t, z+1, y-1, x);	\
  r ## i ## a22 = J ## jd ## D_VLOAD(p, t, z+1, y, x+1);	\
  r ## i ## b22 = J ## jd ## D_VLOAD(p, t, z+1, y-1, x+1);	

// Preload rotating registers unrolled along x
#define J_XU_ROTATE_VLOAD(pat, jd, p, i, t, z, y, x, xu)	\
  J_ROTATE_VLOAD(pat, jd, p, r0, i, t, z, y, x);		\
  J_ROTATE_VLOAD(pat, jd, p, r1, i, t, z, y, x+VF);		\
  if (xu >= 4) {						\
    J_ROTATE_VLOAD(pat, jd, p, r2, i, t, z, y, x+2*VF);		\
    J_ROTATE_VLOAD(pat, jd, p, r3, i, t, z, y, x+3*VF);		\
  }

// Template for unrolling along x
#define J_XU(pat, jd, p, sp, i, a, b, c, t, z, y, x, xu)	\
  J ## pat ## jd ## D ## i(p, sp, rt0, rz0, ry0, rx0,		\
			   r0 ## i ## a, r0 ## i ## b, r0 ## i ## c,\
			   t, z, y, x);				\
  J ## pat ## jd ## D ## i(p, sp, rt1, rz1, ry1, rx1,		\
			   r1 ## i ## a, r1 ## i ## b, r1 ## i ## c,\
			   t, z, y, x+VF);			\
  DEBUG_INSTRUMENT(stencil += 2 * VF);				\
  if (xu >= 4) {						\
    J ## pat ## jd ## D ## i(p, sp, rt2, rz2, ry2, rx2,		\
			     r2 ## i ## a, r2 ## i ## b, r2 ## i ## c,\
			     t, z, y, x+2*VF);			\
    J ## pat ## jd ## D ## i(p, sp, rt3, rz3, ry3, rx3,		\
			     r3 ## i ## a, r3 ## i ## b, r3 ## i ## c,\
			     t, z, y, x+3*VF);			\
    DEBUG_INSTRUMENT(stencil += 2 * VF);			\
  }

#define JC4D0(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 11 = J4D_VLOAD(p, t, z, y+1, x);			\
  va = VMUL(c1111, rc ## 11);					\
  va = VFMADD(c1101, rs ## 11, va);				\
  vo = J4D_VLOAD(p, t, z, y, x-1);				\
  va = VFMADD(c1110, vo, va);					\
  va = VFMADD(c1121, rn ## 11, va);				\
  vo = J4D_VLOAD(p, t, z, y, x+1);				\
  va = VFMADD(c1112, vo, va);					\
  vo = J4D_VLOAD(p, t, z-1, y, x);				\
  va = VFMADD(c1011, vo, va);					\
  vo = J4D_VLOAD(p, t, z+1, y, x);				\
  va = VFMADD(c1211, vo, va);					\
  vo = J4D_VLOAD(p, t-1, z, y, x);				\
  va = VFMADD(c0111, vo, va);					\
  vo = J4D_VLOAD(p, t+1, z, y, x);				\
  rt = rz = ry = rx = VFMADD(c2111, vo, va);			\
  J4D_VSTORE(sp, t, z, y, x, rx);

#define JC4D1(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 11 = ry;						\
  va = VMUL(c1111, rc ## 11);					\
  va = VFMADD(c1101, rs ## 11, va);				\
  vo = J4D_VLOAD(p, t, z, y, x-1);				\
  va = VFMADD(c1110, vo, va);					\
  va = VFMADD(c1121, rn ## 11, va);				\
  va = VFMADD(c1112, rx, va);					\
  vo = J4D_VLOAD(p, t, z-1, y, x);				\
  va = VFMADD(c1011, vo, va);					\
  va = VFMADD(c1211, rz, va);					\
  vo = J4D_VLOAD(p, t-1, z, y, x);				\
  va = VFMADD(c0111, vo, va);					\
  rx = VFMADD(c2111, rt, va);					\
  J4D_VSTORE(sp, t, z, y, x, rx);

#define JC4D_SCALAR(p, sp)					\
  sp[t][z][y][x] = s1110 * p[t][z][y][x-1]			\
    + s1111 * p[t][z][y][x]					\
    + s1112 * p[t][z][y][x+1]					\
    + s1101 * p[t][z][y-1][x]					\
    + s1121 * p[t][z][y+1][x]					\
    + s1011 * p[t][z-1][y][x]					\
    + s1211 * p[t][z+1][y][x]					\
    + s0111 * p[t-1][z][y][x]				        \
    + s2111 * p[t+1][z][y][x]

#define JB3D0(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 00 = J3D_VLOAD(p, , z-1, y+1, x-1);			\
  rn ## 01 = J3D_VLOAD(p, , z-1, y+1, x);			\
  rn ## 02 = J3D_VLOAD(p, , z-1, y+1, x+1);			\
  rn ## 10 = J3D_VLOAD(p, , z, y+1, x-1);			\
  rn ## 11 = J3D_VLOAD(p, , z, y+1, x);				\
  rn ## 12 = J3D_VLOAD(p, , z, y+1, x+1);			\
  rn ## 20 = J3D_VLOAD(p, , z+1, y+1, x-1);			\
  rn ## 21 = J3D_VLOAD(p, , z+1, y+1, x);			\
  rn ## 22 = J3D_VLOAD(p, , z+1, y+1, x+1);			\
  va = VMUL(c0111, rc ## 11);					\
  va = VFMADD(c0110, rc ## 10, va);				\
  va = VFMADD(c0112, rc ## 12, va);				\
  va = VFMADD(c0100, rs ## 10, va);				\
  va = VFMADD(c0101, rs ## 11, va);				\
  va = VFMADD(c0102, rs ## 12, va);				\
  va = VFMADD(c0120, rn ## 10, va);				\
  va = VFMADD(c0121, rn ## 11, va);				\
  va = VFMADD(c0122, rn ## 12, va);				\
  va = VFMADD(c0000, rs ## 00, va);				\
  va = VFMADD(c0001, rs ## 01, va);				\
  va = VFMADD(c0002, rs ## 02, va);				\
  va = VFMADD(c0010, rc ## 00, va);				\
  va = VFMADD(c0011, rc ## 01, va);				\
  va = VFMADD(c0012, rc ## 02, va);				\
  va = VFMADD(c0020, rn ## 00, va);				\
  va = VFMADD(c0021, rn ## 01, va);				\
  va = VFMADD(c0022, rn ## 02, va);				\
  va = VFMADD(c0200, rs ## 20, va);				\
  va = VFMADD(c0201, rs ## 21, va);				\
  va = VFMADD(c0202, rs ## 22, va);				\
  va = VFMADD(c0210, rc ## 20, va);				\
  va = VFMADD(c0211, rc ## 21, va);				\
  va = VFMADD(c0212, rc ## 22, va);				\
  va = VFMADD(c0220, rn ## 20, va);				\
  va = VFMADD(c0221, rn ## 21, va);				\
  rz = ry = rx = VFMADD(c0222, rn ## 22, va);			\
  J3D_VSTORE(sp, , z, y, x, rx);

#define JB3D1(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 00 = J3D_VLOAD(p, , z-1, y+1, x-1);			\
  rn ## 01 = J3D_VLOAD(p, , z-1, y+1, x);			\
  rn ## 02 = J3D_VLOAD(p, , z-1, y+1, x+1);			\
  rn ## 10 = J3D_VLOAD(p, , z, y+1, x-1);			\
  rn ## 11 = ry;						\
  rn ## 12 = J3D_VLOAD(p, , z, y+1, x+1);			\
  rn ## 20 = J3D_VLOAD(p, , z+1, y+1, x-1);			\
  rn ## 21 = J3D_VLOAD(p, , z+1, y+1, x);			\
  rn ## 22 = J3D_VLOAD(p, , z+1, y+1, x+1);			\
  va = VMUL(c0111, rc ## 11);					\
  va = VFMADD(c0110, rc ## 10, va);				\
  va = VFMADD(c0112, rc ## 12, va);				\
  va = VFMADD(c0100, rs ## 10, va);				\
  va = VFMADD(c0101, rs ## 11, va);				\
  va = VFMADD(c0102, rs ## 12, va);				\
  va = VFMADD(c0120, rn ## 10, va);				\
  va = VFMADD(c0121, rn ## 11, va);				\
  va = VFMADD(c0122, rn ## 12, va);				\
  va = VFMADD(c0000, rs ## 00, va);				\
  va = VFMADD(c0001, rs ## 01, va);				\
  va = VFMADD(c0002, rs ## 02, va);				\
  va = VFMADD(c0010, rc ## 00, va);				\
  va = VFMADD(c0011, rc ## 01, va);				\
  va = VFMADD(c0012, rc ## 02, va);				\
  va = VFMADD(c0020, rn ## 00, va);				\
  va = VFMADD(c0021, rn ## 01, va);				\
  va = VFMADD(c0022, rn ## 02, va);				\
  va = VFMADD(c0200, rs ## 20, va);				\
  va = VFMADD(c0201, rs ## 21, va);				\
  va = VFMADD(c0202, rs ## 22, va);				\
  va = VFMADD(c0210, rc ## 20, va);				\
  va = VFMADD(c0211, rz, va);					\
  va = VFMADD(c0212, rc ## 22, va);				\
  va = VFMADD(c0220, rn ## 20, va);				\
  va = VFMADD(c0221, rn ## 21, va);				\
  rz = ry = rx = VFMADD(c0222, rn ## 22, va);			\
  J3D_VSTORE(sp, , z, y, x, rx);

#define JB3D_SCALAR(p, sp)					\
  sp[z][y][x] = s0110 * p[z][y][x-1]				\
    + s0111 * p[z][y][x]					\
    + s0112 * p[z][y][x+1]					\
    + s0100 * p[z][y-1][x-1]					\
    + s0101 * p[z][y-1][x]					\
    + s0102 * p[z][y-1][x+1]					\
    + s0120 * p[z][y+1][x-1]					\
    + s0121 * p[z][y+1][x]					\
    + s0122 * p[z][y+1][x+1]					\
    + s0000 * p[z-1][y-1][x-1]					\
    + s0001 * p[z-1][y-1][x]					\
    + s0002 * p[z-1][y-1][x+1]					\
    + s0010 * p[z-1][y][x-1]					\
    + s0011 * p[z-1][y][x]					\
    + s0012 * p[z-1][y][x+1]					\
    + s0020 * p[z-1][y+1][x-1]					\
    + s0021 * p[z-1][y+1][x]					\
    + s0022 * p[z-1][y+1][x+1]					\
    + s0200 * p[z+1][y-1][x-1]					\
    + s0201 * p[z+1][y-1][x]					\
    + s0202 * p[z+1][y-1][x+1]					\
    + s0210 * p[z+1][y][x-1]					\
    + s0211 * p[z+1][y][x]					\
    + s0212 * p[z+1][y][x+1]					\
    + s0220 * p[z+1][y+1][x-1]					\
    + s0221 * p[z+1][y+1][x]					\
    + s0222 * p[z+1][y+1][x+1]

#define JC3D0(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 11 = J3D_VLOAD(p, , z, y+1, x);				\
  va = VMUL(c0111, rc ## 11);					\
  va = VFMADD(c0101, rs ## 11, va);				\
  vo = J3D_VLOAD(p, , z, y, x-1);				\
  va = VFMADD(c0110, vo, va);					\
  va = VFMADD(c0121, rn ## 11, va);				\
  vo = J3D_VLOAD(p, , z, y, x+1);				\
  va = VFMADD(c0112, vo, va);					\
  vo = J3D_VLOAD(p, , z-1, y, x);				\
  va = VFMADD(c0011, vo, va);					\
  vo = J3D_VLOAD(p, , z+1, y, x);				\
  rz = ry = rx = VFMADD(c0211, vo, va);				\
  J3D_VSTORE(sp, , z, y, x, rx);

#define JC3D1(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 11 = ry;						\
  va = VMUL(c0111, rc ## 11);					\
  va = VFMADD(c0101, rs ## 11, va);				\
  vo = J3D_VLOAD(p, , z, y, x-1);				\
  va = VFMADD(c0110, vo, va);					\
  va = VFMADD(c0121, rn ## 11, va);				\
  va = VFMADD(c0112, rx, va);					\
  vo = J3D_VLOAD(p, , z-1, y, x);				\
  va = VFMADD(c0011, vo, va);					\
  rx = VFMADD(c0211, rz, va);					\
  J3D_VSTORE(sp, , z, y, x, rx);

#define JC3D_SCALAR(p, sp)					\
  sp[z][y][x] = s0110 * p[z][y][x-1]				\
    + s0111 * p[z][y][x]					\
    + s0112 * p[z][y][x+1]					\
    + s0101 * p[z][y-1][x]					\
    + s0121 * p[z][y+1][x]					\
    + s0011 * p[z-1][y][x]					\
    + s0211 * p[z+1][y][x]

#define JB2D0(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 10 = J2D_VLOAD(p, , , y+1, x-1);			\
  rn ## 11 = J2D_VLOAD(p, , , y+1, x);				\
  rn ## 12 = J2D_VLOAD(p, , , y+1, x+1);			\
  va = VMUL(c0010, rc ## 10);					\
  va = VFMADD(c0011, rc ## 11, va);				\
  va = VFMADD(c0012, rc ## 12, va);				\
  va = VFMADD(c0000, rs ## 10, va);				\
  va = VFMADD(c0001, rs ## 11, va);				\
  va = VFMADD(c0002, rs ## 12, va);				\
  va = VFMADD(c0020, rn ## 10, va);				\
  va = VFMADD(c0021, rn ## 11, va);				\
  ry = rx = VFMADD(c0022, rn ## 12, va);			\
  J2D_VSTORE(sp, , , y, x, rx);

#define JB2D1(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 10 = J2D_VLOAD(p, , , y+1, x-1);			\
  rn ## 11 = ry;						\
  rn ## 12 = J2D_VLOAD(p, , , y+1, x+1);			\
  va = VMUL(c0010, rc ## 10);					\
  va = VFMADD(c0011, rc ## 11, va);				\
  va = VFMADD(c0012, rc ## 12, va);				\
  va = VFMADD(c0000, rs ## 10, va);				\
  va = VFMADD(c0001, rs ## 11, va);				\
  va = VFMADD(c0002, rs ## 12, va);				\
  va = VFMADD(c0020, rn ## 10, va);				\
  va = VFMADD(c0021, rn ## 11, va);				\
  ry = rx = VFMADD(c0022, rn ## 12, va);			\
  J2D_VSTORE(sp, , , y, x, rx);

#define JB2D_SCALAR(p, sp)					\
  sp[y][x] = s0010 * p[y][x-1]					\
    + s0011 * p[y][x]						\
    + s0012 * p[y][x+1]						\
    + s0000 * p[y-1][x-1]					\
    + s0001 * p[y-1][x]				                \
    + s0002 * p[y-1][x+1]					\
    + s0020 * p[y+1][x-1]					\
    + s0021 * p[y+1][x]						\
    + s0022 * p[y+1][x+1]

/*
Vectorized 2D Jacobi register-tiling pattern

n == 1
1[y][x+1] reuses STORE 0[y][x]
1[y+1][x] reuses STORE 0[y][x]

y

4  --------
3 -+++**+++-
2 -+++**+++-
1  --------
0
 0123456789AB x

n == 0

y

4
3   --------
2  -+++**+++-
1  -+++**+++-
0   --------
 0123456789AB x
*/

#define JC2D0(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 11 = J2D_VLOAD(p, , , y+1, x);				\
  va = VMUL(c0011, rc ## 11);					\
  va = VFMADD(c0021, rs ## 11, va);				\
  vo = J2D_VLOAD(p, , , y, x-1);				\
  va = VFMADD(c0010, vo, va);					\
  va = VFMADD(c0001, rn ## 11, va);				\
  vo = J2D_VLOAD(p, , , y, x+1);				\
  ry = rx = VFMADD(c0012, vo, va);				\
  J2D_VSTORE(sp, , , y, x, rx);

#define JC2D1(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  rn ## 11 = ry;						\
  va = VMUL(c0011, rc ## 11);					\
  va = VFMADD(c0021, rs ## 11, va);				\
  vo = J2D_VLOAD(p, , , y, x-1);				\
  va = VFMADD(c0010, vo, va);					\
  va = VFMADD(c0001, rn ## 11, va);				\
  rx = VFMADD(c0012, rx, va);					\
  J2D_VSTORE(sp, , , y, x, rx);

#define JC2D_SCALAR(p, sp)					\
  sp[y][x] = s0010 * p[y][x-1]					\
    + s0011 * p[y][x]						\
    + s0012 * p[y][x+1]						\
    + s0001 * p[y-1][x]				                \
    + s0021 * p[y+1][x]

#define J1D0(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  va = J1D_VLOAD(p, , , , x);					\
  va = VMUL(c0001, va);						\
  vo = J1D_VLOAD(p, , , , x-1);					\
  va = VFMADD(c0000, vo, va);					\
  vo = J1D_VLOAD(p, , , , x+1);					\
  rx = VFMADD(c0002, vo, va);					\
  J1D_VSTORE(sp, , , , x, rx);

#define J1D1(p, sp, rt, rz, ry, rx, rc, rs, rn, t, z, y, x)	\
  va = J1D_VLOAD(p, , , , x);					\
  va = VMUL(c0001, va);						\
  vo = J1D_VLOAD(p, , , , x-1);					\
  va = VFMADD(c0000, vo, va);					\
  rx = VFMADD(c0002, rx, va);					\
  J1D_VSTORE(sp, , , , x, rx);

#define J1D_SCALAR(p, sp)					\
  sp[x] = s0000 * p[x-1]					\
    + s0001 * p[x]						\
    + s0001 * p[x+1]

/* Jacobi box functions. */

// Vectorized 3D Jacobi
// DEF_JACOBI(B, 3, 0, 1, 2, 0)
// DEF_JACOBI(B, 3, 0, 1, 4, 0)
// DEF_JACOBI(B, 3, 0, 2, 2, 0)
// DEF_JACOBI(B, 3, 0, 2, 4, 0)
DEF_JACOBI(B, 3, 0, 3, 2, 0)
DEF_JACOBI(B, 3, 0, 3, 4, 0)
// DEF_JACOBI(B, 3, 0, 4, 2, 0)
// DEF_JACOBI(B, 3, 0, 4, 4, 0)

// Time register tiled
// DEF_JACOBI(B, 3, 1, 1, 2, 0)
// DEF_JACOBI(B, 3, 1, 1, 4, 0)
// DEF_JACOBI(B, 3, 1, 2, 2, 0)
// DEF_JACOBI(B, 3, 1, 2, 4, 0)
DEF_JACOBI(B, 3, 1, 3, 2, 0)
DEF_JACOBI(B, 3, 1, 3, 4, 0)
// DEF_JACOBI(B, 3, 1, 4, 2, 0)
// DEF_JACOBI(B, 3, 1, 4, 4, 0)

// Vectorized 2D Jacobi
// DEF_JACOBI(B, 2, 0, 1, 2, 0)
// DEF_JACOBI(B, 2, 0, 1, 4, 0)
// DEF_JACOBI(B, 2, 0, 2, 2, 0)
// DEF_JACOBI(B, 2, 0, 2, 4, 0)
DEF_JACOBI(B, 2, 0, 3, 2, 0)
DEF_JACOBI(B, 2, 0, 3, 4, 0)
// DEF_JACOBI(B, 2, 0, 4, 2, 0)
// DEF_JACOBI(B, 2, 0, 4, 4, 0)

// Time register tiled
// DEF_JACOBI(B, 2, 1, 1, 2, 0)
// DEF_JACOBI(B, 2, 1, 1, 4, 0)
// DEF_JACOBI(B, 2, 1, 2, 2, 0)
// DEF_JACOBI(B, 2, 1, 2, 4, 0)
DEF_JACOBI(B, 2, 1, 3, 2, 0)
DEF_JACOBI(B, 2, 1, 3, 4, 0)
// DEF_JACOBI(B, 2, 1, 4, 2, 0)
// DEF_JACOBI(B, 2, 1, 4, 4, 0)

/* Jacobi cross functions. */

// Vectorized 4D Jacobi
// DEF_JACOBI(C, 4, 0, 1, 2, 0)
// DEF_JACOBI(C, 4, 0, 1, 4, 0)
// DEF_JACOBI(C, 4, 0, 2, 2, 0)
// DEF_JACOBI(C, 4, 0, 2, 4, 0)
DEF_JACOBI(C, 4, 0, 3, 2, 0)
DEF_JACOBI(C, 4, 0, 3, 4, 0)
// DEF_JACOBI(C, 4, 0, 4, 2, 0)
// DEF_JACOBI(C, 4, 0, 4, 4, 0)

// Time register tiled
// DEF_JACOBI(C, 4, 1, 1, 2, 0)
// DEF_JACOBI(C, 4, 1, 1, 4, 0)
// DEF_JACOBI(C, 4, 1, 2, 2, 0)
// DEF_JACOBI(C, 4, 1, 2, 4, 0)
DEF_JACOBI(C, 4, 1, 3, 2, 0)
DEF_JACOBI(C, 4, 1, 3, 4, 0)
// DEF_JACOBI(C, 4, 1, 4, 2, 0)
// DEF_JACOBI(C, 4, 1, 4, 4, 0)

// Vectorized 3D Jacobi
 DEF_JACOBI(C, 3, 0, 1, 2, 0)
 DEF_JACOBI(C, 3, 0, 1, 4, 0)
 DEF_JACOBI(C, 3, 0, 2, 2, 0)
 DEF_JACOBI(C, 3, 0, 2, 4, 0)
DEF_JACOBI(C, 3, 0, 3, 2, 0)
DEF_JACOBI(C, 3, 0, 3, 4, 0)
 DEF_JACOBI(C, 3, 0, 4, 2, 0)
 DEF_JACOBI(C, 3, 0, 4, 4, 0)

// Time register tiled
// DEF_JACOBI(C, 3, 1, 1, 2, 0)
// DEF_JACOBI(C, 3, 1, 1, 4, 0)
// DEF_JACOBI(C, 3, 1, 2, 2, 0)
// DEF_JACOBI(C, 3, 1, 2, 4, 0)
DEF_JACOBI(C, 3, 1, 3, 2, 0)
DEF_JACOBI(C, 3, 1, 3, 4, 0)
// DEF_JACOBI(C, 3, 1, 4, 2, 0)
// DEF_JACOBI(C, 3, 1, 4, 4, 0)

// Vectorized 2D Jacobi
// DEF_JACOBI(C, 2, 0, 1, 2, 0)
// DEF_JACOBI(C, 2, 0, 1, 4, 0)
// DEF_JACOBI(C, 2, 0, 2, 2, 0)
// DEF_JACOBI(C, 2, 0, 2, 4, 0)
DEF_JACOBI(C, 2, 0, 3, 2, 0)
DEF_JACOBI(C, 2, 0, 3, 4, 0)
// DEF_JACOBI(C, 2, 0, 4, 2, 0)
// DEF_JACOBI(C, 2, 0, 4, 4, 0)

// Time register tiled
// DEF_JACOBI(C, 2, 1, 1, 2, 0)
// DEF_JACOBI(C, 2, 1, 1, 4, 0)
// DEF_JACOBI(C, 2, 1, 2, 2, 0)
// DEF_JACOBI(C, 2, 1, 2, 4, 0)
DEF_JACOBI(C, 2, 1, 3, 2, 0)
DEF_JACOBI(C, 2, 1, 3, 4, 0)
// DEF_JACOBI(C, 2, 1, 4, 2, 0)
// DEF_JACOBI(C, 2, 1, 4, 4, 0)

// Vectorized 1D Jacobi
// DEF_JACOBI(, 1, 0, 1, 2, 0)
// DEF_JACOBI(, 1, 0, 1, 4, 0)
// DEF_JACOBI(, 1, 0, 2, 2, 0)
// DEF_JACOBI(, 1, 0, 2, 4, 0)
DEF_JACOBI(, 1, 0, 3, 2, 0)
DEF_JACOBI(, 1, 0, 3, 4, 0)
// DEF_JACOBI(, 1, 0, 4, 2, 0)
// DEF_JACOBI(, 1, 0, 4, 4, 0)

// Time register tiled
// DEF_JACOBI(, 1, 1, 1, 2, 0)
// DEF_JACOBI(, 1, 1, 1, 4, 0)
// DEF_JACOBI(, 1, 1, 2, 2, 0)
// DEF_JACOBI(, 1, 1, 2, 4, 0)
DEF_JACOBI(, 1, 1, 3, 2, 0)
DEF_JACOBI(, 1, 1, 3, 4, 0)
// DEF_JACOBI(, 1, 1, 4, 2, 0)
// DEF_JACOBI(, 1, 1, 4, 4, 0)

