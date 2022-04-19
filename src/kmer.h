#ifndef KMER_H_INCLUDED
#define KMER_H_INCLUDED

#include "defs.h"

#define K_ARRAY_MODE            0
#define K_HASH_TABLE_MODE       1
#define K_HASH_TABLE_BEGIN_CTX  17
#define K_HASH_SIZE             33554467      // first PRIME NUMBER after 2^25
#define K_MAX_HASH_CTX          28
#define K_MIN_HASH_CTX          1

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef U8  K_ACCounter;                // Size of context counters for arrays
typedef U16 K_ENTMAX;                    // Entry size (nKeys for each hIndex)
typedef U32 K_KEYSMAX;                                      // keys index bits

typedef struct
  {
  K_ENTMAX   *entrySize;                       // Number of keys in this entry
  K_KEYSMAX  **keys;                       // The keys of the hash table lists
  }
KHash;

typedef struct
  {
  K_ACCounter *counters;
  }
KArray;

typedef struct
  {
  U32       nSym;
  U8        aa;                                // IF 1 -> AminoAcids : DNA/RNA
  U32       ctx;                          // Current depth of context template
  U64       nPModels;                  // Maximum number of probability models
  U64       multiplier;
  U64       idx;
  U64       idxIR;
  U64       idxAA;
  U8        ir;
  KArray    array;
  KHash     hash;
  U8        mode;
  U32       id;
  }
KModel;

typedef struct
  {
  KModel    **M;
  uint32_t  nModels;
  }
KModels;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void           GetKIdx          (U8 *, KModel *);
void           GetKIdxIR        (U8 *, KModel *);
void           GetKIdxAA        (U8 *, KModel *);
KModel         *CreateKModel    (U32, U32, U8, U32);
void           DeleteKModel     (KModel *);
void           ResetKModelIdx   (KModel *);
void           ResetKIdx        (KModel *);
void           UpdateKIR        (KModel *);
void           UpdateK          (KModel *);
void           NEntriesK        (KModel *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
