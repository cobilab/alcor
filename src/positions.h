#ifndef POSITIONS_H_INCLUDED
#define POSITIONS_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint64_t *end;
  uint64_t *init;
  uint64_t size;
  uint64_t step;
  uint64_t idx;
  }
POS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

POS      *CreatePositions (uint64_t);
void     UpdatePositions  (POS *, uint64_t, uint64_t);
void     RemovePositions  (POS *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
