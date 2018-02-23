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
  return (fixedreal_t) (n * FXRL_F);
}

/* Convert x to integer (rounding toward zero):	x / f */
int32_t fxrl_to_int32_trunc (fixedreal_t x)
{
  return (x / FXRL_F);
}

/* Convert x to integer (rounding to nearest):	*/
/* (x + f / 2) / f  if  x >= 0 */
/* (x - f / 2) / f  if  x <= 0 */
int32_t fxrl_to_int32_near (fixedreal_t x)
{
  if (x < 0)
    return ((x - (FXRL_F / 2)) / FXRL_F);
  else
    return ((x + (FXRL_F / 2)) / FXRL_F);
}

fixedreal_t fxrl_x_plus_y (fixedreal_t x, fixedreal_t y)
{
  return (x + y);
}

fixedreal_t fxrl_x_plus_n (fixedreal_t x, int32_t n)
{
  return (x + ((fixedreal_t) n * FXRL_F));
}

fixedreal_t fxrl_x_minus_y (fixedreal_t x, fixedreal_t y)
{
  return (x - y);
}

fixedreal_t fxrl_x_minus_n (fixedreal_t x, int32_t n)
{
  return (x - ((fixedreal_t) n * FXRL_F));
}

fixedreal_t fxrl_n_minus_x (int32_t n, fixedreal_t x)
{
  return (((fixedreal_t) n * FXRL_F) - x);
}

fixedreal_t fxrl_x_times_y (fixedreal_t x, fixedreal_t y)
{
  return (((int64_t) x * y) / FXRL_F);
}

fixedreal_t fxrl_x_times_n (fixedreal_t x, int32_t n)
{
  return (x * (fixedreal_t) n);
}

fixedreal_t fxrl_x_div_by_y (fixedreal_t x, fixedreal_t y)
{
  return (((int64_t) x * FXRL_F) / y);
}

fixedreal_t fxrl_x_div_by_n (fixedreal_t x, int32_t n)
{
  return (x / (fixedreal_t) n);
}

fixedreal_t fxrl_from_int32_times_1_60 (int32_t n)
{
  return ((fixedreal_t) n * FXRL_1_OF_60);
}

fixedreal_t fxrl_x_times_59_60 (fixedreal_t x)
{
  return ((x * FXRL_59_OF_60) / FXRL_F);
}

fixedreal_t fxrl_2x (fixedreal_t x)
{
  return (x * FXRL_2_N);
}

fixedreal_t fxrl_100x (fixedreal_t x)
{
  return (x * FXRL_100_N);
}

fixedreal_t fxrl_x_times_1_4 (fixedreal_t x)
{
  return ((x * FXRL_1_OF_4) / FXRL_F);
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
