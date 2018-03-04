#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "threads/fixed-point.h"

/* Convert n to fixed point:	n * f */
fixedreal_t fxrl_from_int32 (int32_t n)
{
  return (n * FXRL_F);
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
  if (x >= 0)
    return ((x + (FXRL_F / 2)) / FXRL_F);
  else
    return ((x - (FXRL_F / 2)) / FXRL_F);
}

fixedreal_t fxrl_x_plus_y (fixedreal_t x, fixedreal_t y)
{
  return (x + y);
}

fixedreal_t fxrl_x_plus_n (fixedreal_t x, int32_t n)
{
  return (x + (n * FXRL_F));
}

fixedreal_t fxrl_x_minus_y (fixedreal_t x, fixedreal_t y)
{
  return (x - y);
}

fixedreal_t fxrl_x_minus_n (fixedreal_t x, int32_t n)
{
  return (x - (n * FXRL_F));
}

fixedreal_t fxrl_n_minus_x (int32_t n, fixedreal_t x)
{
  return ((n * FXRL_F) - x);
}

fixedreal_t fxrl_x_times_y (fixedreal_t x, fixedreal_t y)
{
  return ((((int64_t) x) * y) / FXRL_F);
}

fixedreal_t fxrl_x_times_n (fixedreal_t x, int32_t n)
{
  return (x * n);
}

fixedreal_t fxrl_x_div_by_y (fixedreal_t x, fixedreal_t y)
{
  return ((((int64_t) x) * FXRL_F) / y);
}

fixedreal_t fxrl_x_div_by_n (fixedreal_t x, int32_t n)
{
  return (x / n);
}
