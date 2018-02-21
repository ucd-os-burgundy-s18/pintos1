#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "threads/fixed-point.h"

/* x**exp that will fit in int32 */
int64_t power (int32_t x, int32_t exp)
{
  int64_t newvar = 1;
  
  for (int index = 0; index < exp; ++index)
    newvar *= x;
  
  return newvar;
}

/* Convert n to fixed point:	n * f */
fixedreal_t fxrl_conv_int32_fxrl (int32_t n)
{
  int32_t f = FXRL_Q_MAX;
  return (int32_t) ((int64_t)n * f);
}

/* Convert x to integer (rounding toward zero):	x / f */
int32_t fxrl_conv_fxrl_int32_rd_0 (fixedreal_t x)
{
  int64_t f = FXRL_Q_MAX;
  return  (int32_t) ((int64_t)x / f);

}

/* Convert x to integer (rounding to nearest):	*/
/* (x + f / 2) / f  if  x >= 0 */
/* (x - f / 2) / f  if  x <= 0 */
int32_t fxrl_conv_fxrl_int32_rd_near (fixedreal_t x)
{
  int64_t f = FXRL_Q_MAX;

  if (x < 0)
    return (int32_t) (((int64_t)x - (f/2)) / f);
  else
    return (int32_t) (((int64_t)x + (f/2)) / f);
}


fixedreal_t fxrl_x_plus_y (fixedreal_t x, fixedreal_t y)
{
  return (x + y);
}

fixedreal_t fxrl_x_plus_n (fixedreal_t x, int32_t n)
{
  int32_t f = FXRL_Q_MAX;
  return (x + (n * f));
}

fixedreal_t fxrl_x_minus_y (fixedreal_t x, fixedreal_t y)
{
  return (x - y);
}

fixedreal_t fxrl_x_minus_n (fixedreal_t x, int32_t n)
{
  int32_t f = FXRL_Q_MAX;
  return (x - (n * f));
}

fixedreal_t fxrl_x_times_y (fixedreal_t x, fixedreal_t y)
{
  int32_t f = FXRL_Q_MAX;
  return ((int64_t) x) * y / f;
}

fixedreal_t fxrl_x_times_n (fixedreal_t x, int32_t n)
{
  return (x * n);
}

fixedreal_t fxrl_x_div_by_y (fixedreal_t x, fixedreal_t y)
{
  int32_t f = FXRL_Q_MAX;
  return ((int64_t) x) * f / y;
}

fixedreal_t fxrl_x_div_by_n (fixedreal_t x, int32_t n)
{
  return (x / n);
}

fixedreal_t fxrl_x_times_1_60 (fixedreal_t x)
{
  return fxrl_x_times_y(x, (fixedreal_t) FXRL_1_OF_60);
}

fixedreal_t fxrl_x_times_59_60 (fixedreal_t x)
{
  return fxrl_x_times_y(x, (fixedreal_t) FXRL_59_OF_60);
}


void test_math()
{
  printf ("DEBUG:  Adding 2 + 3 = %"PRId32" \n",  
      fxrl_conv_fxrl_int32_rd_0(fxrl_x_plus_n(fxrl_conv_int32_fxrl(2), 3)));
      
  printf ("DEBUG:  Adding 4 + 5 = %"PRId32" \n",  
      fxrl_conv_fxrl_int32_rd_0(fxrl_x_plus_y(fxrl_conv_int32_fxrl(4), fxrl_conv_int32_fxrl(5))));
      
  printf ("DEBUG:  Adding 5 - 3 = %"PRId32" \n",
      fxrl_conv_fxrl_int32_rd_0(fxrl_x_minus_n(fxrl_conv_int32_fxrl(5), 3)));
      
  printf ("DEBUG:  Multiply 120 * (1/60) = %"PRId32" \n",
      fxrl_conv_fxrl_int32_rd_near(fxrl_x_times_1_60 (fxrl_conv_int32_fxrl(120))));
      
  printf ("DEBUG:  120 * (59/60) = %"PRId32" \n",
      fxrl_conv_fxrl_int32_rd_near(fxrl_x_times_59_60(fxrl_conv_int32_fxrl(120))));
      
}

