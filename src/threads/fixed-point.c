#include <stdint.h>
#include <stdbool.h>
#include "threads/fixed-point.h"

/* x**exp that will fit in int32 */
int32_t pow (int32_t x, int32_t exp)
{
  int32_t newvar = 1;
  
  for (int index = 0; index < exp; ++index)
    newvar *= x;
  
  return newvar;
}

/* Convert n to fixed point:	n * f */
fixedreal fxrl_conv_int32_fxrl (int32_t n)
{
  fixedreal newvar;
  int32_t f = FXRL_Q_MAX;

  if (n < 0)
  {
    newvar.isnegative = true;
    n *= -1;
  }
  
  newvar.p = (int32_t) ((int64_t)n * (int64_t)f);
  newvar.q = FXRL_Q_MAX;
  return newvar;
}

/* Convert x to integer (rounding toward zero):	x / f */
int32_t fxrl_conv_fxrl_int32_rd_0 (fixedreal x)
{
  int64_t f = (int64_t) pow(2, x.q);

  if (x.isnegative)
    return  (int32_t) (-1 * ((int64_t)x.p / f));
  else
    return  (int32_t) ((int64_t)x.p / f);

}

/* Convert x to integer (rounding to nearest):	*/
/* (x + f / 2) / f  if  x >= 0 */
/* (x - f / 2) / f  if  x <= 0 */
int32_t fxrl_conv_fxrl_int32_rd_near (fixedreal x)
{
  int64_t f = (int64_t) pow(2, x.q);

  if (x.isnegative)
    return (int32_t) (-1 * (((int64_t)x.p + (f/2)) / f));
  else
    return (int32_t) (((int64_t)x.p - (f/2)) / f);
}

fixedreal fxrl_x_plus_y (fixedreal x, fixedreal y)
{
  fixedreal newvar;

  return newvar;
}

fixedreal fxrl_x_plus_n (fixedreal x, int32_t n)
{
  fixedreal newvar;
  
  
  if (n < 0)
  {
    newvar.isnegative = true;
    n *= -1;
  }

  newvar.p = x.p + n;
  return newvar;
}

fixedreal fxrl_x_minus_y (fixedreal x, fixedreal y)
{
  fixedreal newvar;

  return newvar;
}

fixedreal fxrl_x_minus_n (fixedreal x, int32_t n)
{
  fixedreal newvar;

  return newvar;
}

fixedreal fxrl_x_times_y (fixedreal x, fixedreal y)
{
  fixedreal newvar;

  return newvar;
}

fixedreal fxrl_x_times_n (fixedreal x, int32_t n)
{
  fixedreal newvar;

  return newvar;
}

fixedreal fxrl_x_div_by_y (fixedreal x, fixedreal y)
{
  fixedreal newvar;

  return newvar;
}

fixedreal fxrl_x_div_by_n (fixedreal x, int32_t y)
{
  fixedreal newvar;

  return newvar;
}

