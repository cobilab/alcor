#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "defs.h"
#include "mem.h"
#include "common.h"
#include "dna.h"
#include "kmer.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InitHashTable(KModel *M)
  {
  M->hash.keys      = (K_KEYSMAX **) Calloc(K_HASH_SIZE, sizeof(K_KEYSMAX *));
  M->hash.entrySize = (K_ENTMAX   *) Calloc(K_HASH_SIZE, sizeof(K_ENTMAX   ));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InitArray(KModel *M)
  {
  M->array.counters = (K_ACCounter *) Calloc(M->nPModels, sizeof(K_ACCounter));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteKModel(KModel *M)
  {
  uint32_t k;
  if(M->mode == K_HASH_TABLE_MODE)
    {
    for(k = 0 ; k < K_HASH_SIZE ; ++k)
      if(M->hash.entrySize[k] != 0)
        Free(M->hash.keys[k]);
    Free(M->hash.keys);
    Free(M->hash.entrySize);
    }
  else // TABLE_MODE
    Free(M->array.counters);
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

KModel *CreateKModel(U32 ctx, U32 ir, U8 aa, U32 nSym)
  {
  KModel *M;
  U64    prod = 1, *multipliers;
  U32    n;

  M = (KModel *) Calloc(1, sizeof(KModel));

  if(ctx > K_MAX_HASH_CTX)
    {
    fprintf(stderr, "Error: context is greater than %d!\n", K_MAX_HASH_CTX);
    exit(1);
    }
 
  M->aa        = aa;
  M->nSym      = nSym;
  multipliers  = (U64*) Calloc(ctx, sizeof(U64));
  M->nPModels  = (U64) pow(M->nSym, ctx);
  M->ctx       = ctx;
  M->idx       = 0;
  M->idxIR     = M->nPModels - 1;
  M->ir        = ir == 0 ? 0 : 1;

  if(M->nPModels >= 1073741824)
    { // OR 268435456
    M->mode = K_HASH_TABLE_MODE;
    InitHashTable(M);
    }
  else
    {
    M->mode = K_ARRAY_MODE;
    InitArray(M);
    }
  
  for(n = 0 ; n < M->ctx ; ++n)
    {
    multipliers[n] = prod;
    prod *= M->nSym;
    }

  M->multiplier = multipliers[M->ctx-1];

  Free(multipliers);
  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetKIdx(KModel *M)
  {
  M->idx   = 0;
  M->idxIR = M->nPModels - 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetKIdxIR(U8 *p, KModel *M)
  {
  M->idxIR = (M->idxIR >> 2) + GetCompNum(*p) * M->multiplier;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetKIdx(U8 *p, KModel *M)
  {
  M->idx = ((M->idx - *(p - M->ctx) * M->multiplier) << 2) + *p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetKIdxAA(U8 *p, KModel *M)
  {
  M->idx = ((M->idx - *(p - M->ctx) * M->multiplier) * M->nSym) + *p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InsertKey(KHash *hash, U32 h, U64 key)
  {
  hash->keys[h] = (K_KEYSMAX *) Realloc(hash->keys[h], (hash->entrySize[h]+1) 
  * sizeof(K_KEYSMAX), sizeof(K_KEYSMAX));
  hash->keys[h][hash->entrySize[h]] = (U32) (key / K_HASH_SIZE);
  hash->entrySize[h]++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateKIR(KModel *M)
  {
  U32 n;
  U64 idx = M->idxIR;

  if(M->mode == K_HASH_TABLE_MODE)
    {
    U32 h = idx % K_HASH_SIZE;                               // The hash index
    for(n = 0 ; n < M->hash.entrySize[h] ; n++)
      if(((U64) M->hash.keys[h][n] * K_HASH_SIZE) + h == idx) 
        return;
    InsertKey(&M->hash, h, idx);                           // If key not found
    }
  else
    M->array.counters[idx] = 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateK(KModel *M)
  {
  U32 n;
  U64 idx = M->idx;

  if(M->mode == K_HASH_TABLE_MODE)
    {
    U32 h = idx % K_HASH_SIZE;                               // The hash index
    for(n = 0 ; n < M->hash.entrySize[h] ; n++)
      if(((U64) M->hash.keys[h][n] * K_HASH_SIZE) + h == idx) 
        return;
    InsertKey(&M->hash, h, idx);                           // If key not found
    }
  else
    M->array.counters[idx] = 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void NEntriesK(KModel *M)
  {
  U32 max = 0, k = 0;
  for( ; k < K_HASH_SIZE ; ++k)
    if(max < M->hash.entrySize[k])
      max = M->hash.entrySize[k];
  fprintf(stderr, "Maximum hash entry size: %u\n", max);       
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
