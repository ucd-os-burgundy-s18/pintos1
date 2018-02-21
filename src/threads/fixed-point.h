#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#include <stdint.h>
#include <stdbool.h>

/* Values for p17.q14 fixed-point reals as (2**31 - 1)/(2**14). */
#define FXRL_P_MIN 0                       /* Lowest value of p. */
#define FXRL_P_MAX 2147483647            /* Highest value of p as (2**31 - 1) */
#define FXRL_Q_MIN 0                     /* Lowest value of q. */
#define FXRL_Q_MID 8192                  /* Median value of q. */
#define FXRL_Q_MAX 16384                 /* Highest value of q as (2**14) */
#define FXRL_59_OF_60 16110              /* 59/60*(2**14) */
#define FXRL_1_OF_60 273                 /* 1/60*(2**14) */
#define FXRL_30_OF_60 8192         /* 30/60*(2**14) */


typedef struct fxrl 
{
  /* Probably it will be better to implement this off magnitude alone */
  /* Negative remainders make the not happy */
  bool isnegative;
  int32_t p;
  int32_t q;
} fixedreal;


/* Be sure to cast int to (int32_t) */ 
int64_t pow (int32_t, int32_t);

fixedreal fxrl_conv_int32_fxrl (int32_t);

int32_t fxrl_conv_fxrl_int32_rd_0 (fixedreal);
int32_t fxrl_conv_fxrl_int32_rd_near (fixedreal);


fixedreal fxrl_x_plus_y (fixedreal, fixedreal);
fixedreal fxrl_x_plus_n (fixedreal, int32_t);

fixedreal fxrl_x_minus_y (fixedreal, fixedreal);
fixedreal fxrl_x_minus_n (fixedreal, int32_t);

fixedreal fxrl_x_times_y (fixedreal, fixedreal);
fixedreal fxrl_x_times_n (fixedreal, int32_t);

fixedreal fxrl_x_div_by_y (fixedreal, fixedreal);
fixedreal fxrl_x_div_by_n (fixedreal, int32_t);


#endif /* threads/fixed-point.h */
