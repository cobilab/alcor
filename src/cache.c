#include "cache.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CACHE *CreateCache(uint32_t size, uint32_t nModels)
  {
  uint32_t x, y;
  CACHE *C = (CACHE *) Calloc(1, sizeof(CACHE));
  C->pos   = 0;
  C->size  = size;
  C->E     = (MODELS_E *) Calloc(C->size + 1, sizeof(MODELS_E));
  for(x = 0 ; x < C->size ; ++x)
    {
    C->E[x].nModels = nModels;
    C->E[x].M       = (C_ENTRY *) Calloc(C->E[x].nModels + 1, sizeof(C_ENTRY));
    for(y = 0 ; y < C->E[x].nModels ; ++y)
      {
      C->E[x].M[y].idx    = 0;
      C->E[x].M[y].idx_ir = 0;
      C->E[x].M[y].s      = 0;
      C->E[x].M[y].s_ir   = 0;
      }
    }

  return C;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateCache(CACHE *C)
  {
  if(C->pos == C->size - 1) C->pos = 0;
  else                      C->pos++;
  
  return;	
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveCache(CACHE *C)
  {
  uint32_t x;
  for(x = 0 ; x < C->E->nModels ; ++x)
    Free(C->E[x].M); 
  Free(C->E);
  Free(C);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
