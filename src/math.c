#include <math.h>
#include "math.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint32_t FLog2(uint64_t i)
  {
  uint32_t n, m, k = 32, o = (i & (i - 1)) ? 1 : 0;
  static const uint64_t sizes[6] =
    { 0x0000000000000002ull, 0x000000000000000Cull, 0x00000000000000F0ull,
      0x000000000000FF00ull, 0x00000000FFFF0000ull, 0xFFFFFFFF00000000ull };

  for(n = 6 ; n-- ; )
    {
    o += (m = (i & *(sizes+n)) ? k : 0);
    i >>= m;
    k >>= 1;
    }

  return o;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double Power(double base, double exponent)
  {
  union                // Pow function from http://martin.ankerl.com/2007/10/04/
    {                  // optimized-pow-approximation-for-java-and-c-c/
    double d;
    int x[2];
    } u = { base };
  u.x[1] = (int) (exponent * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
