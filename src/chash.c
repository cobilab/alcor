#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mem.h"
#include "chash.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t ZHASH(uint64_t z)
  {
  z = (~z) + (z << 21);
  z = z    ^ (z >> 24);
  z = (z   + (z << 3)) + (z << 8);
  z = z    ^ (z >> 14);
  z = (z   + (z << 2)) + (z << 4);
  z = z    ^ (z >> 28);
  z = z    + (z << 31);
  return z;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HASH *CreateHashTable(uint32_t nSym)
  { 
  HASH *H         = (HASH *) Calloc(1, sizeof(HASH));
  H->size         = HASH_SIZE;
  H->nSym         = nSym;
  H->entries      = (ENTRY  **) Calloc(H->size, sizeof(ENTRY *));
  H->entrySize    = (ENTMAX  *) Calloc(H->size, sizeof(ENTMAX));
  H->zeroCounters = (HCC    **) Calloc(H->nSym, sizeof(HCC *));
  return H;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HASH *CloneHashTable(HASH *HS)
  {
  HASH *H         = (HASH *) Calloc(1, sizeof(HASH));
  H->size         = HS->size;
  H->nSym         = HS->nSym;
  H->entries      = (ENTRY  **) Calloc(H->size, sizeof(ENTRY *));
  H->entrySize    = (ENTMAX  *) Calloc(H->size, sizeof(ENTMAX));
  H->zeroCounters = (HCC    **) Calloc(H->nSym, sizeof(HCC *));
  
  assert(H->size == HS->size);
  memcpy(H->entrySize, HS->entrySize, sizeof(ENTMAX)*HS->size);
 
  fprintf(stderr, "here!1\n");

  memcpy(H->entries, HS->entries, sizeof(ENTRY *)*HS->size);
  
  fprintf(stderr, "here!2\n");
  //FIXME: hereeeeeeeeeeeeeeeee
  for(uint64_t x = 0 ; x < H->size ; ++x)
    {
    if(H->entrySize[x] != 0)
      {
      memcpy(H->entries[x], HS->entries[x], sizeof(ENTRY)*HS->entrySize[x]);
      for(uint64_t y = 0 ; y < H->entrySize[x] ; ++y)
        memcpy(H->entries[x][y].counters, HS->entries[x][y].counters,
	sizeof(ENTRY));
      }
    }

  fprintf(stderr, "here!5\n");
  
  return H;  
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void InsertKey(HASH *H, uint32_t hi, uint64_t key, uint8_t s)
  {
  H->entries[hi] = (ENTRY *) Realloc(H->entries[hi], (H->entrySize[hi] + 1) 
		   * sizeof(ENTRY), 1);
  H->entries[hi][H->entrySize[hi]].counters = (HCC *) Calloc(s, sizeof(HCC));
  H->entries[hi][H->entrySize[hi]].key = key;
  H->entrySize[hi]++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HCC *GetHCCounters(HASH *H, uint64_t key)
  {
  key = ZHASH(key);
  uint64_t x, hi = key % H->size;              //The hash index

  for(x = 0 ; x < H->entrySize[hi] ; ++x)
    if(H->entries[hi][x].key == key)     // If key found
      return H->entries[hi][x].counters;

  return NULL;
  } 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateHashCounter(HASH *H, uint32_t sym, uint64_t im)
  {
  uint32_t x;
  uint64_t idx = ZHASH(im);
  uint64_t s, hIndex = idx % H->size;

  for(x = 0 ; x < H->entrySize[hIndex] ; ++x)
    if(H->entries[hIndex][x].key == idx)
      {
      if(++H->entries[hIndex][x].counters[sym] == MAX_HASH_COUNTER) 
        for(s = 0 ; s < H->nSym ; ++s)
          H->entries[hIndex][x].counters[s] /= 2;
      return;
      }

  InsertKey(H, hIndex, idx, H->nSym);
  H->entries[hIndex][x].counters[sym]++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UnUpdateHashCounter(HASH *H, uint32_t sym, uint64_t im)
  {
  uint32_t x;
  uint64_t idx = ZHASH(im);
  uint64_t s, hIndex = idx % H->size;

  for(x = 0 ; x < H->entrySize[hIndex] ; ++x)
    if(H->entries[hIndex][x].key == idx)
      {
      if(H->entries[hIndex][x].counters[sym] > 0)
        H->entries[hIndex][x].counters[sym]--;
      return;
      }
  
  InsertKey(H, hIndex, idx, H->nSym);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveHashTable(HASH *H)
  {
  uint64_t x, y;
  for(x = 0 ; x < H->size ; ++x)
    {
    if(H->entrySize[x] != 0)
      {
      for(y = 0 ; y < H->entrySize[x] ; ++y)
        Free(H->entries[x][y].counters);
      Free(H->entries[x]);
      }
    }
  Free(H->entries);
  Free(H->entrySize);
  Free(H);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

