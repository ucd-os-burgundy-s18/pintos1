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
fixedreal_t fxrl_from_int32 (int32_t n)
{
  int32_t f = FXRL_Q_MAX;
  return (int32_t) ((int64_t)n * f);
}

/* Convert x to integer (rounding toward zero):	x / f */
int32_t fxrl_to_int32_trunc (fixedreal_t x)
{
  int64_t f = FXRL_Q_MAX;
  return  (int32_t) ((int64_t)x / f);

}

/* Convert x to integer (rounding to nearest):	*/
/* (x + f / 2) / f  if  x >= 0 */
/* (x - f / 2) / f  if  x <= 0 */
int32_t fxrl_to_int32_near (fixedreal_t x)
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

fixedreal_t fxrl_from_int32_times_1_60 (int32_t n)
{
  return fxrl_x_times_n((fixedreal_t) FXRL_1_OF_60, n);
}

fixedreal_t fxrl_x_times_59_60 (fixedreal_t x)
{
  return fxrl_x_times_y(x, (fixedreal_t) FXRL_59_OF_60);
}


void test_math()
{
  printf ("DEBUG:  Adding 2 + 3 = %"PRId32" \n",  
      fxrl_to_int32_trunc(fxrl_x_plus_n(fxrl_from_int32(2), 3)));
      
  printf ("DEBUG:  Adding 4 + 5 = %"PRId32" \n",  
      fxrl_to_int32_trunc(fxrl_x_plus_y(fxrl_from_int32(4), fxrl_from_int32(5))));
      
  printf ("DEBUG:  Adding 5 - 3 = %"PRId32" \n",
      fxrl_to_int32_trunc(fxrl_x_minus_n(fxrl_from_int32(5), 3)));
      
  printf ("DEBUG:  Multiply 120 * (1/60) = %"PRId32" \n",
      fxrl_to_int32_near(fxrl_from_int32_times_1_60 (fxrl_from_int32(120))));
      
  printf ("DEBUG:  120 * (59/60) = %"PRId32" \n",
      fxrl_to_int32_near(fxrl_x_times_59_60(fxrl_from_int32(120))));
      
}

