#ifndef CM_ARRAY_H_INCLUDED
#define CM_ARRAY_H_INCLUDED

#include "defs.h"

typedef uint16_t ACC; // Size of context counters for arrays

#define MAX_ARRAY_COUNTER ((1<<(sizeof(ACC)*8))-1)

typedef struct
  {
  ACC      *counters;
  uint64_t nSym;
  uint64_t length;
  }
ARRAY;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ARRAY      *CreateArrayTable     (uint64_t, uint64_t);
ARRAY      *CloneArrayTable      (ARRAY *);
void       UpdateArrayCounter    (ARRAY *, uint32_t, uint64_t);
void       UnUpdateArrayCounter  (ARRAY *, uint32_t, uint64_t);
void       RemoveArrayTable      (ARRAY *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
