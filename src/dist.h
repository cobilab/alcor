#ifndef DIST_H_INCLUDED
#define DIST_H_INCLUDED

#include <stdio.h>
#include "defs.h"
#include "alphabet.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint64_t  *sym;
  uint32_t  nSym;
  }
Counts;

typedef struct{
  Counts    *C;
  uint32_t  size;
  }
Dist;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Dist      *CreateDist     (uint32_t, uint32_t);
void      PrintDist       (Dist *, uint32_t);
void      PrintDistAA     (Dist *, ALPHABET *, uint32_t);
void      PrintDistAAPerc (Dist *, ALPHABET *, uint32_t, int8_t, int8_t);
void      PrintCG         (Dist *, uint32_t, uint32_t, uint32_t);
void      RemoveDist      (Dist *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

