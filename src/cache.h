#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint64_t idx;
  uint64_t idx_ir;
  uint8_t  s;
  uint8_t  s_ir;
  }
C_ENTRY;

typedef struct{
  C_ENTRY  *M;
  uint32_t nModels;
  }
MODELS_E;

typedef struct{
  MODELS_E *E;
  uint32_t size;
  uint32_t pos;
  }
CACHE;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CACHE *CreateCache (uint32_t, uint32_t);
void  UpdateCache  (CACHE *);
void  RemoveCache  (CACHE *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

