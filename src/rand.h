#ifndef RAND_H_INCLUDED
#define RAND_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  uint64_t seed;
  uint64_t next;
  uint64_t a;
  uint64_t b;
  uint64_t module;
  }
G_RAND;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

G_RAND     *CreateRand   (uint64_t, uint64_t, uint64_t, uint64_t);
void       Srand         (G_RAND *, uint64_t);
uint32_t   RandValue     (G_RAND *);
void       RemoveRand    (G_RAND *);         

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
