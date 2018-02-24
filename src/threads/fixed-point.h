#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#include <stdint.h>
#include <stdbool.h>

/* INVARIANT:  Do not attempt to store a value that exceeds 17.14 fixed-point */

/* Values for p17.q14 fixed-point reals as (2**31 - 1)/(2**14). */
#define FXRL_P_MIN 0                    /* Lowest value of p. */
#define FXRL_P_MAX 2147483647           /* Highest value of p as (2**31 - 1) */
#define FXRL_Q_MIN 0                    /* Lowest value of q. */
#define FXRL_Q_MAX 16384                /* Highest value of q as (2**14) */
#define FXRL_F 16384                    /* f = 2**14 */

/* Use int32_t */
typedef int32_t fixedreal_t;

fixedreal_t fxrl_from_int32 (int32_t);

int32_t fxrl_to_int32_trunc (fixedreal_t);
int32_t fxrl_to_int32_near (fixedreal_t);

fixedreal_t fxrl_x_plus_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_plus_n (fixedreal_t, int32_t);

fixedreal_t fxrl_x_minus_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_minus_n (fixedreal_t, int32_t);
fixedreal_t fxrl_n_minus_x (int32_t, fixedreal_t);

fixedreal_t fxrl_x_times_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_times_n (fixedreal_t, int32_t);

fixedreal_t fxrl_x_div_by_y (fixedreal_t, fixedreal_t);
fixedreal_t fxrl_x_div_by_n (fixedreal_t, int32_t);

#endif /* threads/fixed-point.h */
