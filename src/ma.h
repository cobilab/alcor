#ifndef MA_H_INCLUDED
#define MA_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "defs.h"
#include "param.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double MovAvgInstant (double *, double *, uint32_t, uint32_t, double);
void   MovingAverage (MA_PARAMETERS *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

