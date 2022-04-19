#ifndef CM_TOLERANT_H_INCLUDED
#define CM_TOLERANT_H_INCLUDED

#include "defs.h"
#include "pmodels.h"
#include "buffer.h"

typedef struct
  {
  uint32_t   in;
  CBUF       *seq;           // BUFFER FOR EDITED SEQUENCE
  uint8_t    *mask;          // BUFFER FOR FAILS & HITS
  uint64_t   idx;            // INDEX FOR UPDATE
  uint64_t   idx2;           // AUXILIAR INDEX FOR UPDATE
  uint32_t   threshold;      // DISCARD ABOVE THIS VALUE
  uint32_t   ctx;            // ORDER OF CONTEXT MODEL
  uint32_t   nSym;           // NUMBER OF SYMBOLS
  uint32_t   den;            // ALPHA DENOMINATOR FOR THIS MODEL
  uint32_t   ir;             // INVERTED REPEATS
  }
TOLERANT;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TOLERANT     *CreateTolerantModel   (uint32_t, uint32_t, uint32_t, uint32_t,
                                     uint32_t);
int32_t      BestId                 (uint32_t *, uint32_t, uint32_t);
void         UpdateTolerantModel    (TOLERANT *, PMODEL *, uint8_t);
void         Fail                   (TOLERANT *);
void         Hit                    (TOLERANT *);
void         RemoveTolerantModel    (TOLERANT *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
