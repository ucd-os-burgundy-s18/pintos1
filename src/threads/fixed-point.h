#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#include <stdint.h>
#include <stdbool.h>

/* Values for p17.q14 fixed-point reals as (2**31 - 1)/(2**14). */
#define FXRL_P_MIN 0                     /* Lowest value of p. */
#define FXRL_P_MAX 2147483647            /* Highest value of p as (2**31 - 1) */
#define FXRL_Q_MIN 0                     /* Lowest value of q. */
#define FXRL_Q_MID 8192                  /* Median value of q. */
#define FXRL_Q_MAX 16384                 /* Highest value of q as (2**14) */
#define FXRL_59_OF_60 16110              /* 59/60*(2**14) */
#define FXRL_1_OF_60 273                 /* 1/60*(2**14) */
#define FXRL_30_OF_60 8192               /* 30/60*(2**14) */


typedef int32_t fixedreal_t;


/* Be sure to cast int to (int32_t) */ 
int64_t power (int32_t, int32_t);

fixedreal_t fxrl_from_int32 (int32_t);

int32_t fxrl_to_int32 (fixedreal_t);
int32_t fxrl_to_int32_near (fixedreal_t);


fixedreal_t fxrl_x_plus_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_plus_n (fixedreal_t, int32_t);

fixedreal_t fxrl_x_minus_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_minus_n (fixedreal_t, int32_t);

fixedreal_t fxrl_x_times_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_times_n (fixedreal_t, int32_t);

fixedreal_t fxrl_x_div_by_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_div_by_n (fixedreal_t, int32_t);

/* Special multiply specific values */ 
fixedreal_t fxrl_from_int32_times_1_60 (int32_t);
fixedreal_t fxrl_x_times_59_60 (fixedreal_t);

/* Math test driver */
void test_math(void);

#endif /* threads/fixed-point.h */
