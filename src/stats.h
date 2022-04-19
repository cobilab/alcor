#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED

#include <stdio.h>
#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  double    *average;
  double    *variance;
  double    *stddev;
  uint64_t  size;
  }
Stats;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Stats     *CreateStats  (uint32_t);
void      PrintStats    (Stats *, uint32_t);
void      RemoveStats   (Stats *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

