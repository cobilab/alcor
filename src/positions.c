#include <string.h>
#include <assert.h>

#include "mem.h"
#include "positions.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATE POSITIONS
//
POS *CreatePositions(uint64_t s)
  {
  assert(s>1);

  POS *PO  = (POS *) Calloc(1, sizeof(POS));
  PO->size = s;
  PO->init = (uint64_t *) Calloc(PO->size+1, sizeof(uint64_t));
  PO->end  = (uint64_t *) Calloc(PO->size+1, sizeof(uint64_t));
  PO->idx  = 0;
  PO->step = 5000;

  return PO;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE POSITIONS
//
void UpdatePositions(POS *PO, uint64_t init, uint64_t end)
  {
  PO->init[PO->idx] = init;
  PO->end [PO->idx] = end;
  PO->idx++;

  if(PO->idx == PO->size)
    {
    uint64_t newsize = PO->size + PO->step;
    PO->init = (uint64_t *) Realloc(PO->init, newsize * sizeof(uint64_t), 0);
    PO->end  = (uint64_t *) Realloc(PO->end,  newsize * sizeof(uint64_t), 0);
    PO->size = newsize;
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REMOVE POSITIONS
//
void RemovePositions(POS *PO)
  {
  Free(PO->init);
  Free(PO->end);
  Free(PO);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
