#ifndef CM_HASH_H_INCLUDED
#define CM_HASH_H_INCLUDED

#include "defs.h"

typedef uint8_t  HCC;        // Size of context counters for hash tables
typedef uint16_t ENTMAX;     // Entry size (nKeys for each hIndex)

#define MAX_HASH_COUNTER ((1<<(sizeof(HCC)*8))-1)

#define HASH_SIZE 16777259

typedef struct{
  uint64_t   key;            // The key stored in this entry 
  HCC        *counters;      // The context counters 
  }
ENTRY;

typedef struct{
  uint32_t   size;           // Size of the hash table
  ENTMAX     *entrySize;     // Number of keys in this entry
  ENTRY      **entries;      // The heads of the hash table lists
  HCC        **zeroCounters;  
  uint32_t   nSym;           // Number of symbols
  }
HASH;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t   ZHASH                (uint64_t);
HASH       *CreateHashTable     (uint32_t);
HASH       *CloneHashTable      (HASH *);
void       InsertKey            (HASH *, uint32_t, uint64_t, uint8_t);
HCC        *GetHCCounters       (HASH *, uint64_t);
void       UpdateHashCounter    (HASH *, uint32_t, uint64_t);
void       UnUpdateHashCounter  (HASH *, uint32_t, uint64_t);
void       RemoveHashTable      (HASH *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
