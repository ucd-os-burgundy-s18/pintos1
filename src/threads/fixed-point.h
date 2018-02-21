#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#include <stdint.h>
#include <arithmetic.h>
#include <stdbool.h>

// CONSIDER DECLARING FIXED POINT MATH HERE

/* Values for p17.q14 fixed-point reals as (2**31 - 1)/(2**14). */
#define FXRL_P_MIN 0;                       /* Lowest value of p. */
#define FXRL_P_MAX = 2147483647;            /* Highest value of p as (2**31 - 1) */
#define FXRL_Q_MIN = 0;                     /* Lowest value of q. */
#define FXRL_Q_MAX = 16384;                 /* Highest value of q as (2**14) */
#define FXRL_59_OF_60 = 16110;              /* 59/60*(2**14) = 16,110 */
#define FXRL_1_OF_60 = 273;                 /* 1/60*(2**14) = 273 */

struct fixed_real 
{
  /* Probably it will be better to implement this off magnitude alone */
  /* Negative remainders make the not happy */
  bool isnegative;
  uint32_t p17whole;
  uint32_t q14fraction;
  uint32_t ftempvalue;                      /* Make it available but don't rely on this value. */
};

/* Be sure to cast int to (int32_t) */ 
fixed_real fxrl_conv_int32_fxrl (int32_t);

int32_t fxrl_conv_fxrl_int32_rd_0 (fixed_real);
int32_t fxrl_conv_fxrl_int32_rd_near (fixed_real);

fixed_real fxrl_x_plus_y (fixed_real, fixed_real);
fixed_real fxrl_x_plus_n (fixed_real, int32_t);

fixed_real fxrl_x_minus_y (fixed_real, fixed_real);
fixed_real fxrl_x_minus_n (fixed_real, int32_t);

fixed_real fxrl_x_times_y (fixed_real, fixed_real);
fixed_real fxrl_x_times_n (fixed_real, int32_t);

fixed_real fxrl_x_div_by_y (fixed_real, fixed_real);
fixed_real fxrl_x_div_by_n (fixed_real, int32_t);

#endif /* threads/fixed-point.h */
