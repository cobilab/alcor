#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "defs.h"
#include "mem.h"
#include "carray.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ARRAY *CreateArrayTable(uint64_t nSym, uint64_t length)
  {  
  ARRAY *A    = (ARRAY *) Calloc(1, sizeof(ARRAY));
  A->nSym     = nSym;
  A->length   = length;
  A->counters = (ACC *) Calloc(A->length * A->nSym, sizeof(ACC));
  return A;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ARRAY *CloneArrayTable(ARRAY *OA)
  {
  ARRAY *A    = (ARRAY *) Calloc(1, sizeof(ARRAY));
  A->nSym     = OA->nSym;
  A->length   = OA->length;
  A->counters = (ACC *) Calloc(A->length * A->nSym, sizeof(ACC));
  assert(A->length * A->nSym == OA->length * OA->nSym);
  memcpy(A->counters, OA->counters, sizeof(ACC)*A->length*A->nSym); 
  return A;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateArrayCounter(ARRAY *A, uint32_t sym, uint64_t idx)
  {
  uint32_t x;
  ACC *AC = &A->counters[idx * A->nSym];
  if(++AC[sym] == MAX_ARRAY_COUNTER)
    for(x = 0 ; x < A->nSym ; ++x)
      AC[x] /= 2;
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UnUpdateArrayCounter(ARRAY *A, uint32_t sym, uint64_t idx)
  {
  uint32_t x;
  ACC *AC = &A->counters[idx * A->nSym];
  if(AC[sym] > 0)
    --AC[sym];
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveArrayTable(ARRAY *A)
  {
  Free(A->counters);
  Free(A);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

